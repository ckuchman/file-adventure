#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <time.h>

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

    printf("Final Dir: %s", roomDir);

    //Close current directory
    closedir(directory);

    //Open rooms directory
    DIR *roomDirectory;

    roomDirectory = opendir(roomDir);

    
    closedir(roomDirectory);

    //Game loop while not at end room



    //Display end room results


    return 0;
}


