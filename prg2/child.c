/*******************************************************************************************
* Michael Ames
* Program 2
* CIS 452: Operating System Concepts
* Winter 2018 - GVSU
* Instructor: Greg Wolffe
*
* This program is spawned by a parent process. It is assigned a text file to scan. It
* counts the number of times a search query occurs in that file, and writes the value
* to a pipe connected to it's parent.  
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
#include <ctype.h>
#include <signal.h>

#define SIZE 4096
#define READ 0
#define WRITE 1
#define FILE_NOT_FOUND 0
#define MEM_ALLOCATION 1
#define FILE_CLOSE_IN 2
#define PIPE_ERROR 3
#define FORK_FAIL 4


void sigHandler(int sigNum);
int readFile(char* fileName, char* query);
int getFileSize(char* fileName);
void sigHandler(int sigNum);
void handleError(int errorCode);
char *trimwhitespace(char *str);

int readPipe;
int writePipe;

/***********************************************************************
 	Main driver for the child class. Prints logs to screen, calls the
    readFile function on it's file, and writes the results to the pipe
    to communicate the total matches to the parent.
***********************************************************************/
int main(int argc, char* argv[]) {

    signal(SIGUSR1, sigHandler);

    char query[SIZE];
    char * trimmedFileName;
    char * trimmedQuery;
    int total;
    char stringTotal[SIZE];     
    int childNo;
    
    readPipe = (int) strtol(argv[2], (char **)NULL, 10);
    writePipe = (int) strtol(argv[3], (char **)NULL, 10);
    childNo = (int) strtol(argv[4], (char **)NULL, 10);
    
    while(1) {

       read(readPipe, query, SIZE);
       
       trimmedFileName = trimwhitespace(argv[1]);
       
       trimmedQuery = trimwhitespace(query);
       
       fflush(stdout); 
       sleep(.3);
       printf("\nChild %d: Scanning file - %s\n", childNo, argv[1]);
       fflush(stdout);
       
       total = readFile(trimmedFileName, trimmedQuery);
       
       sprintf(stringTotal, "%d", total);
       
       fflush(stdout);
       sleep(.3);
       printf("\nChild %d: Writing total of %d to pipe.\n", childNo, total);
       fflush(stdout);
       
       write(writePipe, stringTotal, SIZE);
    }
        
    return 0;
}

/***********************************************************************
 	Dynamically allocates memory equal to the size of the input file.
 	Opens the input file and reads the text into the allocated memory.
    Scans the file and counts the number of times the query occurs.
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
    return size;
}

/***********************************************************************
 	Signal handler for ending the process. Closes all pipes,
    and exits the process.
    @param signum the signal code.
***********************************************************************/
void sigHandler(int sigNum) {

    if(sigNum == SIGUSR1)  {
    
    printf("Child: closing pipes.\n");
    fflush(stdout);

    close(readPipe);
    close(writePipe);
    printf("Child: exiting.\n");
    fflush(stdout);
    exit(0);
    }  
}

//credit: https://stackoverflow.com/questions/122616/how-do-i-trim-leading-trailing-whitespace-in-a-standard-way
/***********************************************************************
 	Trims the leading and trailing whitespace from a string. 
    @param *str a pointer to the string to trim.
    @return str a pointer to the modified string.
***********************************************************************/
char *trimwhitespace(char *str)
{
  char *end;

  // Trim leading space
  while(isspace((unsigned char)*str)) str++;

  if(*str == 0)  // All spaces?
    return str;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace((unsigned char)*end)) end--;

  // Write new null terminator
  *(end+1) = 0;

  return str;
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