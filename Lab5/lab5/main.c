#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SIZE 4096

int main()
{

    int flag = 0;
    int shmId;
    char *shmPtr;
    char userIn[SIZE];

    if ( (shmId = shmget(IPC_PRIVATE, SIZE, IPC_CREAT|S_IRUSR|S_IWUSR) ) < 0) {
        perror("Error creating shared memory.\n");
        exit(1);
    }

    if ( (shmPtr = shmat(shmId, 0, 0)) == (void*) -1) {
        perror("Can't attach shared memory to address.\n");
        exit(1);
    }

    printf("Please enter a phrase.\n");
    scanf("%[^\n]s", userIn);

    sprintf(shmPtr, userIn);

    printf("%s", shmPtr);

    flag = 1;




    return 0;
}
