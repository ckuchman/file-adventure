#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <time.h>

struct room
{
    char name[9];
    struct room *conRooms[6];
    int numConRooms;
    char type[4];
};

void getRecentFolder (char roomDir[]);
void storeRoomArray (struct room roomList[], char roomDir[]);
struct room *getStartRoom (struct room roomList[]);
struct room *getRoomByName (struct room *roomList, int roomNum, char name[]);
void adventureLoop (struct room *startRoom);
void displayRoom (struct room *curRoom);

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
    adventureLoop(startRoom);

    return 0;
}


void getRecentFolder (char roomDir[]){
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

        if ((int)statBuffer.st_mtime > recentTime && \
            strstr(entry->d_name, "kuchmanc.rooms") != NULL) {
            recentTime = (int)statBuffer.st_mtime;
            sprintf(roomDir, entry->d_name); 
        }
    }

    //Close current directory
    closedir(directory);
}


void storeRoomArray (struct room roomList[], char roomDir[]){
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


struct room *getStartRoom (struct room roomList[]){
    //Get the starting room
    struct room *startRoom;    

    int roomCount = 0;
    while (strcmp(roomList[roomCount].type, "STR") != 0){
        roomCount++;
    }
    startRoom = &roomList[roomCount];

    return startRoom; 
}


struct room *getRoomByName (struct room *roomList, int roomNum, char name[]){
    struct room *correctRoom = NULL;
    int roomCount = 0;

    while (correctRoom == NULL && roomCount < roomNum){

        if (strcmp((roomList + roomCount)->name, name) == 0){
            correctRoom = roomList + roomCount;
        }

        roomCount++;
    }

    return correctRoom;
}


void adventureLoop (struct room *startRoom){

    struct room *currentRoom = startRoom;
    struct room *nextRoom;

    struct room *roomPath[100];

    char nextRoomStr[10];
    memset(nextRoomStr, '\0', 10*sizeof(char));

    int validRoom;
    int i;
    int steps = 0;

    //game loop
    while (strcmp(currentRoom->type, "END") != 0) {    

        //Get valid input from user
        do {

            //Display contents of Room
            displayRoom(currentRoom);

            //TODO Clear Buffer for double
            scanf("%s", nextRoomStr);

            validRoom = 0;

            for (i = 0; i < currentRoom->numConRooms; i++) {

                if (strcmp(nextRoomStr, currentRoom->conRooms[i]->name) == 0) {
                    validRoom = 1;
                    nextRoom = currentRoom->conRooms[i];
                }
            }

            printf("\n");

            if (validRoom == 0) {
                printf("HUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n\n");
            }
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


void displayRoom (struct room *curRoom){
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
