#include <stdio.h>
#include <string.h>
#include <dirent.h>

int main() {

    //Find the most recent rooms folder
    char *roomDir[50];
    memset(roomDir, '\0', 50*sizeof(char));

    //Open current directory
    //Code source taken from https://c-for-dummies.com/blog/?p=3246
    DIR *directory;
    struct dirent *entry;
    directory = opendir(".");

    //Look through directory to find newest room folder
    while (entry = readdir(directory)){
        printf("File: %s\n", entry->d_name);
    }

    //Close current directory
    closedir(directory);


    //Find the starting room
    

    //Game loop while not at end room



    //Display end room results


    return 0;
}


