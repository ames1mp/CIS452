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

#define SIZE 4096
#define MAX_CHILDREN 10
#define FILE_NOT_FOUND 0
#define MEM_ALLOCATION 1
#define FILE_CLOSE_IN 2
#define INVALID_INPUT 3
#define FORK_FAIL 3
#define CHILD_EXECUTABLE_PATH "./child.exe"

int readFile(char* fileName, char* query);
int getFileSize(char* fileName);
void handleError(int errorCode);
void sigHandler(int sigNum);
void createChild(char* fileName, pid_t* childrenIds, char* childFileName, int index);


int main() {

    char fileNames[SIZE] = "\0";
    char* tok;
    char fileTokens[MAX_CHILDREN][SIZE];
    int numFiles = 0;

    pid_t parentId;
    pid_t pid;
    pid_t childrenIds[MAX_CHILDREN] = {0};

    parentId = getpid();

    //signal(SIGINT, sigHandler);

    printf("Please enter up to 10 files names, separated by whitespace\n");

    fgets(fileNames, SIZE, stdin);

    tok = strtok(fileNames, " ");

    //Tokenize the string. Credit: https://www.tutorialspoint.com/c_standard_library/c_function_strtok.htm
    while(tok != NULL) {
        char temp[SIZE]; 
        strcpy(fileTokens[numFiles], tok);
        ++numFiles;
        tok = strtok(NULL, " ");      
    }
 
     for(int i = 0; i < numFiles; ++i) {
        printf("Parent: Spawning a process to scan the file: %s\n", fileTokens[i]);
        
        pid = fork();
    
        if(pid < 0) {
            perror("Failed to fork.");
            exit(1);
        }
        else if(pid == 0) {

            char* args[] = {CHILD_EXECUTABLE_PATH, fileTokens[i], NULL};
            execvp(args[0], args);
        }    
    }

    for(int i = 0; i < numFiles; ++i) {
        wait(NULL);
    }
   




    return 0;
}



/***********************************************************************
 	Dynamically allocates memory equal to the size of the input file.
 	Opens the input file and reads the text into the allocated memory.
    @param filename the file from which to read.
    @param query the word to search for.
    @return count the number of times the word occurs in the file.
***********************************************************************/
int readFile(char* fileName, char* query) {

    int fileSize;
    char line[SIZE];
    int count = 0;
    char* pointer;

    fileSize = getFileSize(fileName);

    char* buffer = (char*) malloc(sizeof(char) * fileSize);

    //Based on code used in my final project for CIS 361.
    //Original code provided by Dr. Vijay Bhuse.
    if(buffer == NULL)
        handleError(MEM_ALLOCATION);

    FILE* f = fopen(fileName, "r");

    if (f == NULL)
        handleError(FILE_NOT_FOUND);

    //Adapted from https://stackoverflow.com/questions/20253177/how-to-count-how-many-times-a-word-appears-in-a-txt-file?lq=1
    while( fgets(line, SIZE, f)) {
        pointer = line;
        while( (pointer = (strstr(pointer, query)))) {
            count++;
            ++pointer;
        }
    }

    if ( (fclose(f)) !=0 )
        handleError(FILE_CLOSE_IN);

    free(buffer);

    return count;
}

/***********************************************************************
 	Creates a stat struct, populates it with data about the input file
 	and returns the file's size.
    @param filename the input file
    @return size the file's size in bytes.
***********************************************************************/
int getFileSize(char* fileName) {

    struct stat st;

    if( stat( fileName, &st) == -1 )
        handleError(FILE_NOT_FOUND);
    int size = st.st_size;

}

void sigHandler(int sigNum) {

    if(sigNum == SIGINT) {
        printf("Received an interrupt.");
        //TODO: close all pipes, files, sockets, terminate all child processes

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
            fprintf( stderr, "You must enter a file name.\n" );
            break;
        case 4 :
            fprintf( stderr, "Failed to fork.\n" );
            break;

    }

    exit(1);
}

