// TODO
#include<stdio.h>
#include<string.h>

int main(void)
{
    //TODO Create a directory called kuchmanc.rooms.<PROCESS ID>
    int pid = getpid();
    char dirName[100];
    memset(dirName, '\0', 100);
    sprintf(dirName, "./kuchmanc.rooms.%d", pid);

    mkdir(dirName, 0700);

    //TODO Generate 7 different room files

    return 0; 
}
