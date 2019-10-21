#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <time.h>

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

    //Open rooms directory to find Start
    DIR *roomDirectory;
    FILE *room;
    char lineStr[50];
    memset(lineStr, '\0', 50*sizeof(char));

    char roomFile[50];
    memset(roomFile, '\0', 50*sizeof(char));

    roomDirectory = opendir(roomDir);

    while (entry = readdir(roomDirectory)){
    
        stat(entry->d_name, &statBuffer);

        if (strstr(entry->d_name, "_room") != NULL){

            sprintf(roomFile, "%s/%s", roomDir, entry->d_name);

            room = fopen(roomFile, "r");

            DisplayRoom(room);

            fclose(room);
        }
    }
    
    closedir(roomDirectory);

    //Game loop while not at end room
    FILE *currentRoom;




    //Display end room results


    return 0;
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
