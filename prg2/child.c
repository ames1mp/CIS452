#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>

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

int main(int argc, char* argv[]) {

    char query[SIZE];
    char * trimmedFileName;
    char * trimmedQuery;
    int total;
    char stringTotal[SIZE];
    int len = 0;
    int readPipe;
    int writePipe;
    int childNo;
    

    readPipe = (int) strtol(argv[2], (char **)NULL, 10);
    writePipe = (int) strtol(argv[3], (char **)NULL, 10);
    childNo = (int) strtol(argv[4], (char **)NULL, 10);
    pid_t pid;
    pid = getpid();
    
    while(1) {

       read(readPipe, query, SIZE);
       
       trimmedFileName = trimwhitespace(argv[1]);
       printf("Trimmed %s\n", trimmedFileName);

       trimmedQuery = trimwhitespace(query);
       
       fflush(stdout); 
       printf("Child %d: Scanning file - %s\n", childNo, argv[1]);
       fflush(stdout);
       
       total = readFile(trimmedFileName, trimmedQuery);
       
       sprintf(stringTotal, "%d", total);


       //fflush(stdout);
       //printf("Child %d: Scanning file - %s\n", childNo, argv[2]); //TEST
       //fflush(stdout);
       
       fflush(stdout);
       printf("Child %d: Writing total to pipe.\n", childNo);
       fflush(stdout);
       
       write(writePipe, stringTotal, SIZE);
       //write("%s", childNo, SIZE);
       //write(writePipe, "JHHG", SIZE);//TEST
    }
        
    //printf("I am the child, and my file is: %s\n", argv[1]);
    //printf("I am the child, and my read pipe is: %s\n", argv[2]);
    //printf("I am the child, and my write pipe is: %s\n", argv[3]);
    //printf("I am the child, and my number is: %s\n", argv[4]);

    //int readPipeFd;
    //readPipeFd = (int) strtol(argv[2], (char **)NULL, 10);

    //printf("Im the child and the read pipe is %d", readPipeFd);
    


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
    return size;

}


void sigHandler(int sigNum) {

    //free malloc
    //close file
    //exit
    return;
}

//credit: https://stackoverflow.com/questions/122616/how-do-i-trim-leading-trailing-whitespace-in-a-standard-way
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

