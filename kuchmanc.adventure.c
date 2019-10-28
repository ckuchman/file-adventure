#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <time.h>
#include <pthread.h>
#include <assert.h>

struct room
{
    char name[9];
    struct room *conRooms[6];
    int numConRooms;
    char type[4];
};

void getRecentFolder(char roomDir[]);
void storeRoomArray(struct room roomList[], char roomDir[]);
struct room *getStartRoom(struct room roomList[]);
struct room *getRoomByName(struct room *roomList, int roomNum, char name[]);
void *adventureLoop(void *arg);
void displayRoom(struct room *curRoom);
void *displayTime(void *arg);

int main() {
    //Find the most recent rooms folder
    char roomDir[50];
    memset(roomDir, '\0', 50*sizeof(char));

    getRecentFolder(roomDir);

    //Store information of rooms
    struct room roomList[7];

    storeRoomArray(roomList, roomDir);

    //Get the starting room
    struct room *startRoom;

    startRoom = getStartRoom(roomList);
   
    //Run game loop
    pthread_t adventureThread;
    int adventureThreadCode;

    //Starts up the adventure thread
    adventureThreadCode = pthread_create(&adventureThread, NULL, adventureLoop, (void *) &startRoom);
    assert(0 == adventureThreadCode);

    //holds the end of program until the adventure finishes
    adventureThreadCode = pthread_join(adventureThread, NULL);
    assert(0 == adventureThreadCode);

    return 0;
}


void getRecentFolder(char roomDir[]){
    //This function takes in a char array to store the directory name that start in kuchmanc.rooms,
    //is in the same folder as the executing program, and was most recently changed.

    //Open current directory
    //Code source taken from https://c-for-dummies.com/blog/?p=3246
    DIR *directory;
    struct dirent *entry;

    directory = opendir(".");

    struct stat statBuffer;
    int recentTime = -1; 

    //Look through directory to find newest room folder
    while (entry = readdir(directory)){
        stat(entry->d_name, &statBuffer);

        //If the folder was modified more recent than the previous ones inspected then store the folder name
        if ((int)statBuffer.st_mtime > recentTime && \
            strstr(entry->d_name, "kuchmanc.rooms") != NULL) {
            recentTime = (int)statBuffer.st_mtime;
            sprintf(roomDir, entry->d_name); 
        }
    }

    //Close current directory
    closedir(directory);
}


void storeRoomArray(struct room roomList[], char roomDir[]){
    //This function takes in a directory name to search and looks through a directory of that name in the same
    //folder as the porgram for all the files that end in _room. The files have their contents read into a struct
    //that contains the name, connecting rooms, and the room type. This is stored in the passed in room array.

    //Store information of rooms
    DIR *roomDirectory;
    struct dirent *entry;

    char fileStr[50];
    memset(fileStr, '\0', 50*sizeof(char));

    int roomCount = 0;
    int len = 0;

    roomDirectory = opendir(roomDir);

    //Increment through files in room directory to store names
    while (entry = readdir(roomDirectory)){

        //Only look at files that end in _room
        if (strstr(entry->d_name, "_room") != NULL){
            
            //Copy the file name into the struct
            strcpy(fileStr, entry->d_name);

            //Set the end of string earlier to not include _room
            len = strlen(fileStr);
            fileStr[len - 5] = '\0';

            strcpy(roomList[roomCount].name, fileStr);
            roomCount++;
        }
    }

    //Open rooms directory to find Start and End
    FILE *roomFile;

    char lineStr[50];
    memset(lineStr, '\0', 50*sizeof(char));

    char roomFileStr[50];
    memset(roomFileStr, '\0', 50*sizeof(char));

    struct room *curRoom; 

    //Set the directory pointer back to the start of the directory
    rewinddir(roomDirectory);
    roomCount = 0;

    int conRoomCount = 0;

    //Increment through files in room directory
    while (entry = readdir(roomDirectory)){
    
        //Only look at files that end in _room
        if (strstr(entry->d_name, "_room") != NULL){

            //Concat the proper file path and open it
            sprintf(roomFileStr, "%s/%s", roomDir, entry->d_name);
            roomFile = fopen(roomFileStr, "r");

            fgets(lineStr, 50, roomFile);
           
            //Gets rid of new line and gets the room struct
            lineStr[strlen(lineStr) - 1] = '\0';
            curRoom = getRoomByName(roomList, 7, lineStr + 11);

            //Store all of the room connections
            conRoomCount = 0;
            curRoom->numConRooms = 0;
           
            fgets(lineStr, 50, roomFile);

            //While more connection lines exist add the connections to the room struct
            do {  
                 lineStr[strlen(lineStr) - 1] = '\0';
                 curRoom->conRooms[conRoomCount] = getRoomByName(roomList, 7, lineStr + 14);
                 curRoom->numConRooms++;

                 conRoomCount++;

                 fgets(lineStr, 50, roomFile);
            } while (strstr(lineStr, "CONNECTION") != NULL); 

            //Store the end type
            if (strstr(lineStr, "START") != NULL) {
                strcpy(curRoom->type, "STR");
            } else if (strstr(lineStr, "END") != NULL) {
                strcpy(curRoom->type, "END");
            } else {
                strcpy(curRoom->type, "MID");
            }

            fclose(roomFile);
        }
    }
    
    closedir(roomDirectory);
}


struct room *getStartRoom(struct room roomList[]){
    //This function looks through the passed in room struct array and returns the room that
    //is of the start type.

    //Get the starting room
    struct room *startRoom;    

    //Loops through the rooms until the one with the start type is found
    int roomCount = 0;
    while (strcmp(roomList[roomCount].type, "STR") != 0){
        roomCount++;
    }

    //Store the start room
    startRoom = &roomList[roomCount];

    return startRoom; 
}


struct room *getRoomByName(struct room *roomList, int roomNum, char name[]){
    //This function takes in a room list and room name and returns a pointer to the struct
    //that shares the room name.

    struct room *correctRoom = NULL;
    int roomCount = 0;

    //Increments over the room structs until one matching the name is found
    while (correctRoom == NULL && roomCount < roomNum){

        if (strcmp((roomList + roomCount)->name, name) == 0){
            correctRoom = roomList + roomCount;
        }

        roomCount++;
    }

    return correctRoom;
}


void *adventureLoop(void *arg){
    //This function takes in a pointer to a room struct that is the starting room. The program
    //displays the contents of the room and then prompts the user to input a room to go to. If time is 
    //input, then the current time and date is displayed. Once a valid room name is entered the process repeats
    //until a room with the END type is reached. The program displays the path of rooms to get the end and the 
    //number of steps.

    //Converts the *void arg required for thread creation into a room pointer
    struct room *startRoom = *((struct room **) arg); 

    struct room *currentRoom = startRoom;
    struct room *nextRoom;

    struct room *roomPath[100];
    //TODO default values?

    char nextRoomStr[10];
    memset(nextRoomStr, '\0', 10*sizeof(char));

    int validRoom;
    int i;
    int steps = 0;

    //Create mutex and lock the adventure process
    pthread_mutex_t advMutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_lock(&advMutex);

    //Start time thread
    pthread_t timeThread;
    int timeThreadCode;

    timeThreadCode = pthread_create(&timeThread, NULL, displayTime, (void *) &advMutex);
    assert(0 == timeThreadCode);

    //game loop
    while (strcmp(currentRoom->type, "END") != 0) {    

        //Get valid input from user
        do {

            //Display contents of Room
            displayRoom(currentRoom);

            //TODO Clear Buffer for double
            scanf("%s", nextRoomStr);

            validRoom = 0;

            //Checks the user input to see if a valid room name or 'time' is entered
            if (strstr(nextRoomStr, "time") != 0) {

                //Frees the thread so the time thread can take over, the adventure thread is held until 
                //the time function finishes
                pthread_mutex_unlock(&advMutex);

                timeThreadCode = pthread_join(timeThread, NULL);
                assert(0 == timeThreadCode);

                //Starts a new time thread to run next time the thread can gain access
                timeThreadCode = pthread_create(&timeThread, NULL, displayTime, (void *) &advMutex);
                assert(0 == timeThreadCode);

                pthread_mutex_lock(&advMutex);
            } else {

                //Checks the connections to see if the room input is valid
                for (i = 0; i < currentRoom->numConRooms; i++) {

                    if (strcmp(nextRoomStr, currentRoom->conRooms[i]->name) == 0) {
                        validRoom = 1;
                        nextRoom = currentRoom->conRooms[i];
                    }
                }

                printf("\n");

                //Prints a message when a invalid room name is input
                if (validRoom == 0) {
                    printf("HUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n");
                }
            }

            printf("\n");
        } while(validRoom == 0);

        currentRoom = nextRoom;

        //increment step count and add new room to list, set current room to new room
        roomPath[steps] = currentRoom;
        steps++;
    }

    //Print end message
    printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
    printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", steps);

    for (i = 0; i < steps; i++) {

        printf("%s\n", roomPath[i]->name);
    }
}


void displayRoom(struct room *curRoom){
    //This function displays the information of the room to the screen
    //and writes a prompt for input but does not take in a value.

    //Increment the string to reach room name
    printf("CURRENT LOCATION: %s\n", curRoom->name);
            
    //Add the connections
    printf("POSSIBLE CONNECTIONS: ");            
            
    int i;
    for (i = 0; i < curRoom->numConRooms; i++) {

        printf("%s", curRoom->conRooms[i]->name);

        if (i < curRoom->numConRooms - 1) {
            printf(", ");
        } else {
            printf(".\n");
        }
    }

    printf("WHERE TO? >");
}

void *displayTime(void *arg){
    //This function takes in a mutex to lock when being run. The current time is 
    //written to a currentTime.txt function and then read back for the file and
    //printed to the screen.

    pthread_mutex_t *mutex = (pthread_mutex_t *) arg; 
    int mutexCode;

    FILE *timeFile;

    time_t curTime;
    struct tm *timeInfo;

    char timeBuffer[80];
    memset(timeBuffer, '\0', 80*sizeof(char));

    char readBuffer[80];
    memset(readBuffer, '\0', 80*sizeof(char));

    //Locks the mutex for the process
    mutexCode = pthread_mutex_lock(mutex);

    //Opens up (or creates) the file to write the time to
    timeFile = fopen("currentTime.txt", "w");

    //Reads in the time and structures the format
    time(&curTime);
    timeInfo = localtime(&curTime);
    strftime(timeBuffer, 80, "%I:%M%P, %A, %B %d, %Y", timeInfo);

    //Writes the time to the file (removes the starting 0 if hour is one digit
    if (timeBuffer[0] == '0') {
        fprintf(timeFile, "%s\n", timeBuffer + 1);
    } else {
        fprintf(timeFile, "%s\n", timeBuffer);
    }

    //Closes the file for writing and opened for reading
    fclose(timeFile);
    timeFile = fopen("currentTime.txt", "r");

    //Prints the contents of the time file
    fgets(readBuffer, 80, timeFile);
    printf("%s", readBuffer);

    fclose(timeFile);

    pthread_mutex_unlock(mutex);
}
