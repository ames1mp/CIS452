#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#define SIZE 1024

void sigIntHandler(int);
void sigHandlerUsr1(int);
void sigHandlerUsr2(int);

int main()
{
    signal(SIGINT, sigIntHandler);
    signal(SIGUSR1, sigHandlerUsr1);
    signal(SIGUSR2, sigHandlerUsr2);

    printf("waiting . . . \n");
    pause();

    return 0;
}

void sigIntHandler(int sigNum) {

    printf("^C received. That's it, I'm shutting you down . . .\n");
    exit(0);

}

void sigHandlerUsr1(int sigNum) {

    printf("received a SIGUSR1 signal.");
    fflush(stdout);
    return;

}

void sigHandlerUsr2(int sigNum) {

    printf("received a SIGUSR2 signal.");
    fflush(stdout);
    return;

}

