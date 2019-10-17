// TODO
#include<stdio.h>
#include<string.h>
#include<time.h>

int main(void)
{
    int i, j;

    //Create a directory called kuchmanc.rooms.<PROCESS ID>
    int pid = getpid();
    char dirName[50];
    memset(dirName, '\0', 50);
    sprintf(dirName, "./kuchmanc.rooms.%d", pid);

    mkdir(dirName, 0700);

    //Generate 7 different room files

    //Create list of 10 file names possible
    char roomNames[10][10];

    //TODO put i somewhere better?
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
    
    //Randomly create 7 rooms (no duplicates)
    srand(time(0));

    int nameOpts[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    int pickedNames[7];

    //Increment through shrinking list of options picking names
    int randPos;
    for (i = 0; i < 7; i++)
    {
        //Generate random number between the current i and end of list
        randPos = (rand() % (10 - i)) + i;

        pickedNames[i] = nameOpts[randPos];
        
        //make the picked name "removed"
        nameOpts[randPos] = nameOpts[i];
    }

    //Creates the seven rooms
    FILE *roomFiles[7];
    char filePath[60];
    memset(filePath, '\0', 60);

    for (i = 0; i < 7; i++) {
        //Creates and open files
        sprintf(filePath, "./%s/%s_room", dirName, roomNames[pickedNames[i]]);
        roomFiles[i] = fopen(filePath, "w+");

        fprintf(roomFiles[i], "ROOM NAME: %s\n", roomNames[pickedNames[i]]);
    }

    //Generate all the connections until the requirements are achieved
    int roomConnections[7][7] = { 0 };
    int roomConCount[7] = { 0 };

    for (i = 0; i < 7; i++){
        roomConnections[i][i] = -1;
    }

    //Randomly assigns connections
    int row, col;
    while (!ValidNumConnections(&roomConCount[0], 7)) {
        row = 0;
        col = 0;

        //Generate random connections until one is valid
        while (roomConnections[row][col] != 0 || roomConCount[row] == 6 || roomConCount[col] == 6) {
            row = rand() % 7;
            col = rand() % 7; 
        }

        //Make connection and store number of connections per room
        roomConnections[row][col] = 1;
        roomConnections[col][row] = 1;
        roomConCount[row]++;
        roomConCount[col]++;

        //TODO Remove as just for testing
        for (i = 0; i < 7; i++) {
            for (j = 0; j < 7; j++) {
                printf("%d ", roomConnections[i][j]);
            }
            printf("\n");
        }
    }

    //Add connection text to files
    int roomNum;
    for (i = 0; i < 7; i++) {
        roomNum = 1;
        
        for (j = 0; j < 7; j++) {
            if (roomConnections[i][j] == 1) {
                fprintf(roomFiles[i], "CONNECTION %d: %s\n", roomNum, roomNames[pickedNames[j]]);
                roomNum++;
            }
        }
    }

    //TODO Add room type

    //Close the rooms
    for (i = 0; i < 7; i++) {
        fclose(roomFiles[i]);
    }

    return 0; 
}

int ValidNumConnections(int *conCount, int numRooms) {

    int validNum = 1;

    int i;
    for (i = 0; i < numRooms; i++) {
        printf("Number of room connections for room %d: %d\n", i, conCount[i]);
        if (conCount[i] < 3) {
            validNum = 0;
        }
    }   

    return validNum;
}
