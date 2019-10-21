#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <time.h>

void AdventureLoop (FILE *startRoom, FILE *endRoom);
void DisplayRoom (FILE *room);

int main() {

    //Find the most recent rooms folder
    char roomDir[50];
    memset(roomDir, '\0', 50*sizeof(char));

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

    //Open rooms directory to find Start and End
    DIR *roomDirectory;
    FILE *room;

    char lineStr[50];
    memset(lineStr, '\0', 50*sizeof(char));

    char roomFile[50];
    memset(roomFile, '\0', 50*sizeof(char));

    char startRoomPath[50];
    memset(startRoomPath, '\0', 50*sizeof(char));

    char endRoomPath[50];
    memset(endRoomPath, '\0', 50*sizeof(char));

    roomDirectory = opendir(roomDir);

    //Increment through files in room directory
    while (entry = readdir(roomDirectory)){
    
        //Only look at files that end in _room
        if (strstr(entry->d_name, "_room") != NULL){

            //Concat the proper file path and open it
            sprintf(roomFile, "%s/%s", roomDir, entry->d_name);
            room = fopen(roomFile, "r");

            //Check if the room is the start
            fseek(room, -11, SEEK_END);

	    if (strstr(fgets(lineStr, 50, room), "START") != NULL) {
                sprintf(startRoomPath, "%s", roomFile); 
	    }
 
            //Check if the room is the end
            fseek(room, -9, SEEK_END);

	    if (strstr(fgets(lineStr, 50, room), "END") != NULL) {
                sprintf(endRoomPath, "%s", roomFile);
	    }           

            fclose(room);
        }
    }
    
    closedir(roomDirectory);

    //Run game loop
    FILE *startRoom, *endRoom;

    startRoom = fopen(startRoomPath, "r");
    endRoom = fopen(endRoomPath, "r");

    AdventureLoop(startRoom, endRoom);

    fclose(startRoom); 
    fclose(endRoom);

    return 0;
}

void AdventureLoop (FILE *startRoom, FILE *endRoom){

    FILE *currentRoom;

    currentRoom = startRoom;    

    //game loop
    
    //Display contents of Room
    DisplayRoom(currentRoom);

    //Get valid input from user

    //increment step count and add new room to list, set current room to new room

    //Check if the end room has been reached, if not then start loop again

    //Print end message
}


void DisplayRoom (FILE *room){
    char lineStr[50];
    memset(lineStr, '\0', 50*sizeof(char));

    //Print the current location
    fgets(lineStr, 50, room);
            
    //Increment the string to reach room name
    printf("CURRENT LOCATION: %s", lineStr + 11);
            
    //Add the connections
    printf("POSSIBLE CONNECTIONS: ");            
           
    fgets(lineStr, 50, room);
            
    while (strstr(lineStr, "CONNECTION") != NULL){
                
        //Gets rid of new line
        lineStr[strlen(lineStr) - 1] = '\0';

        printf("%s" , lineStr + 14);

        if (strstr(fgets(lineStr, 50, room), "CONNECTION") != NULL){
            printf(", ");
        }
    }

    printf(".\n");
    printf("WHERE TO? >");
}
