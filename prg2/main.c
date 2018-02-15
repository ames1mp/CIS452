/*******************************************************************************************
* Michael Ames
* Program 2
* CIS 452: Operating System Concepts
* Winter 2018 - GVSU
* Instructor: Greg Wolffe
*
* This program demonstrates knowledge of process management and IPC.
* It allows the user to scan multiple files and find the number of times a word occurs.
* Each file is scanned by a seperate process, which is spawned by the main process.
*
* The code for opening and reading files is adapted from a project I wrote for CIS 343.
*****************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#define SIZE 4096
#define READ 0
#define WRITE 1
#define MAX_CHILDREN 10
#define FILE_NOT_FOUND 0
#define MEM_ALLOCATION 1
#define FILE_CLOSE_IN 2
#define PIPE_ERROR 3
#define FORK_FAIL 4
#define CHILD_EXECUTABLE_PATH "./child.exe"

int readFile(char* fileName, char* query);
int getFileSize(char* fileName);
void handleError(int errorCode);
void sigHandler(int sigNum);
void createChild(char* fileName, pid_t* childrenIds, char* childFileName, int index);

int parentWriteFds[MAX_CHILDREN][2];
int parentReadFds[MAX_CHILDREN][2];
pid_t childrenIds[MAX_CHILDREN] = {0};
int numFiles = 0;


int main() {

    char fileNames[SIZE] = "\0";
    char* tok;
    char fileTokens[MAX_CHILDREN][SIZE];
    

    //pid_t parentId;
      pid_t pid;
      

    char query[SIZE];
    
    //parentId = getpid();

    signal(SIGINT, sigHandler);

    printf("Please enter up to 10 files names, separated by whitespace\n");

    fgets(fileNames, SIZE, stdin);

    tok = strtok(fileNames, " ");

    //Tokenize the string. Credit: https://www.tutorialspoint.com/c_standard_library/c_function_strtok.htm
    while(tok != NULL) {
        //char temp[SIZE]; 
        strcpy(fileTokens[numFiles], tok);
        ++numFiles;
        tok = strtok(NULL, " ");      
    }
    

    

    //create pipes
    for (int i = 0; i < numFiles; ++i) {
        if(pipe(parentWriteFds[i]) < 0)
            handleError(PIPE_ERROR);
        if(pipe(parentReadFds[i]) < 0)
            handleError(PIPE_ERROR);
    }
  
    
    //fork and exec children
     for(int i = 0; i < numFiles; ++i) {
        
        fflush(stdout);
        printf("Parent: Spawning a process to scan the file: %s\n", fileTokens[i]);
        fflush(stdout);
        
        pid = fork();
        childrenIds[i] = pid;
    
        if(pid < 0) {
            perror("Failed to fork.");
            exit(1);
        }
        else if(pid == 0) {
            //dup2(parentWriteFds[i][READ], STDIN_FILENO);
            //dup2(parentReadFds[i][WRITE], STDOUT_FILENO);
            //child closes the write end of the parent's write pipe.
            close(parentWriteFds[i][WRITE]);
            //close(parentWriteFds[i][READ]);
            //child closes the read end of the parent's read pipe.
            close(parentReadFds[i][READ]);
            //close(parentReadFds[i][WRITE]);

            //child read pipe = parent write pipe
            char childReadPipe[SIZE];
            sprintf(childReadPipe, "%d", parentWriteFds[i][READ]);
            
            char childWritePipe[SIZE];
            sprintf(childWritePipe, "%d", parentReadFds[i][WRITE]);
            
            char childNo[SIZE];
            sprintf(childNo, "%d", i);
            
            char* args[] = {CHILD_EXECUTABLE_PATH, fileTokens[i], childReadPipe, childWritePipe, childNo, NULL};
            execv(args[0], args);
        }    
    }

    //Parent closes write end of read pipes and vice-versa.  
    for(int i = 0; i < numFiles; ++i) {
        close(parentReadFds[i][WRITE]);
        close(parentWriteFds[i][READ]);
    }
    
    sleep(1);
    
    while(1) {
        printf("Enter your search word: ");
        fgets(query, SIZE, stdin);

        char buffer[SIZE];
        
        //write query to children
        for(int i = 0; i < numFiles; ++i) {
            fflush(stdout);
            printf("Parent: Sending query to child %d\n", i);
            fflush(stdout);
            write(parentWriteFds[i][WRITE], query, SIZE);
        }
        
        sleep(1);
        int results[numFiles];
        for(int i = 0; i < numFiles; ++i) { 
            read(parentReadFds[i][READ], buffer, SIZE);
            results[i] = (int) strtol(buffer, (char **)NULL, 10);
            //close(parentReadFds[i][READ]);      
        }

        int total = 0;
        for(int i = 0; i < numFiles; ++i) { 
            //printf("%d\n", results[i]);
            total += results[i];
        }
        printf("The total occurances of the word \"%s\" in %d files is: %d\n", query, numFiles, total);
        }
    
  
    
    //read(parentReadFds[0][READ], buffer, SIZE);
   // printf("%s", buffer);
   // close(parentReadFds[0][READ]);
   // close(parentWriteFds[0][WRITE]);


    return 0;
}





void sigHandler(int sigNum) {

    if(sigNum == SIGINT) {
        printf("Received an interrupt.\n");

        printf("Parent: Closing pipes.\n");
        for(int i = 0; i < numFiles; ++i) {
            close(parentReadFds[i][READ]);
            close(parentReadFds[i][WRITE]);

            close(parentWriteFds[i][READ]);
            close(parentWriteFds[i][WRITE]);

            printf("Parent: Sending signal to child: %d.\n", childrenIds[i]);
            kill(childrenIds[i], SIGUSR1);
        }

         for(int i = 0; i < numFiles; ++i) {
            wait(NULL);
         };

        printf("Parent: exiting.\n");
        exit(0);
    }
}



/***********************************************************************
    Prints an error message to stderr and exits the program with '1'
    @param error_code a macro defined integer which indicates what type
    of error occurred.
***********************************************************************/
void handleError(int errorCode) {

    switch(errorCode) {

        case 0 :
            fprintf( stderr, "No such file or directory\n" );
        break;
        case 1 :
            fprintf( stderr, "Memory allocation error.\n" );
        break;
        case 2 :
            fprintf( stderr, "Error closing the IN file.\n" );
        break;
        case 3 :
            fprintf( stderr, "Pipe error.\n" );
            break;
        case 4 :
            fprintf( stderr, "Failed to fork.\n" );
            break;

    }

    exit(1);
}

