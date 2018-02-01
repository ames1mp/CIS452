#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

void signalHandler(int);


int main()
{
    pid_t parentPid;
    parentPid = getpid();

    pid_t childPid = 0;

    if( (childPid = fork()) < 0) {
       perror("Fork failed.");
        exit(1);
    }
    else if (!childPid) {
        srand(time(NULL));
        int randTime;
        int randProcess;

        while(1) {
            fflush(stdout);

            randTime = rand() % 4 + 1;
            sleep(randTime);

            randProcess = rand() % 2 + 1;

            printf("%d", randTime);

            if(randProcess == 1)
                kill(parentPid, SIGUSR1);
            else
                kill(parentPid, SIGUSR2);
        }

    } else {

        while(1) {

        printf("waiting . . .");
        fflush(stdout);

        signal(SIGINT, signalHandler);
        signal(SIGUSR1, signalHandler);
        signal(SIGUSR2, signalHandler);

        pause();

        }

    }


    return 0;
}

void signalHandler(int signalNumber) {

        //CTRL-C interrupt signal SIGINT

        if(signalNumber == SIGINT){

            printf (" received an interrupt.\n");

            // this is where shutdown code would be inserted
            sleep (1);

            printf ("outta here.\n");

            exit(0);

        }

        //Check for SIGUSR1 interrupt

        if(signalNumber == SIGUSR1) {

         printf("RECEIVED SIGUSR1 SIGNAL\n");

        }

        //Check for SIGUSR2 interrupt

        if(signalNumber == SIGUSR2) {

         printf("RECEIVED SIGUSR2 SIGNAL\n");

        }

}


