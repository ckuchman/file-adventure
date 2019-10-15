// TODO
#include<stdio.h>
#include<string.h>
#include<time.h>

int main(void)
{
    //TODO Create a directory called kuchmanc.rooms.<PROCESS ID>
    int pid = getpid();
    char dirName[100];
    memset(dirName, '\0', 100);
    sprintf(dirName, "./kuchmanc.rooms.%d", pid);

    mkdir(dirName, 0700);

    //TODO Generate 7 different room files

    //Create list of 10 file names possible
    char roomNames[10][10];

    //TODO put i somewhere better?
    int i;
    for (i = 0; i < 10; i++)
    {
        memset(roomNames[i], '\0', 10);
    }

    //Hardcode diffetent room names
    sprintf(roomNames[0], "TODO1");
    sprintf(roomNames[1], "TODO2");
    sprintf(roomNames[2], "TODO3");
    sprintf(roomNames[3], "TODO4");
    sprintf(roomNames[4], "TODO5");
    sprintf(roomNames[5], "TODO6");
    sprintf(roomNames[6], "TODO7");
    sprintf(roomNames[7], "TODO8");
    sprintf(roomNames[8], "TODO9");
    sprintf(roomNames[9], "TODO10");
    
    //TODO Can remove, just for testing
    //for (i = 0; i < 10; i++)
    //{
    //    printf("Room %d: %s\n", i, roomNames[i]);
    //}

    //Randomly create 7 rooms (no duplicates)
    srand(time(0));

    int nameOpts[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    int pickedNames[7];

    //Increment through shrinking list of options picking names
    int randPos;
    printf("test1");
    for (i = 0; i < 7; i++)
    {
        //Generate random number between the current i and end of list
        randPos = (rand() % (10 - i)) + i;

        printf("%d", randPos);

        pickedNames[i] = nameOpts[randPos];
        
        //Swap picked name with pos in list to avoid duplicates
        nameOpts[i] = nameOpts[randPos];
    }

    //TODO Can remove, just for testing
    for (i = 0; i < 7; i++)
    {
        printf("Room %d: %d\n", i, pickedNames[i]);
    } 

    return 0; 
}
