#include <stdio.h>
#include <stdlib.h>
#include<unistd.h>

int main(int argc, char* argv[]) {

    //printf("I am the child, and my file is: %s\n", argv[1]);
    //printf("I am the child, and my read pipe is: %s\n", argv[2]);
    //printf("I am the child, and my write pipe is: %s\n", argv[3]);
    //printf("I am the child, and my number is: %s\n", argv[4]);

    //int readPipeFd;
    //readPipeFd = (int) strtol(argv[2], (char **)NULL, 10);

    //printf("Im the child and the read pipe is %d", readPipeFd);
    
    char buffer[4096];
    fgets(buffer, 4096, stdin);
    

    printf("The message is: %s\n", buffer);


    return 0;
}