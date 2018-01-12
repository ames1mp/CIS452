/************************************
* Michael Ames
* Program 1
* CIS 452: Operating System Concepts
* Winter 2018 - GVSU
* Instructor: Greg Wolffe
************************************/

/************************************************************************************************
* Some code is from the IBM Knowledge Center articles:
* https://www.ibm.com/support/knowledgecenter/en/SSLTBW_2.3.0/com.ibm.zos.v2r3.bpxbd00/rttcga.htm
* https://www.ibm.com/support/knowledgecenter/en/SSLTBW_2.3.0/com.ibm.zos.v2r3.bpxbd00/rttcsa.htm
************************************************************************************************/

#define _POSIX_SOURCE
#include <stdio.h>
#include <termios.h>
#include <unistd.h>

#define SIZE 1024

int main(int argc, char *argv[]) {

    char passord[SIZE] = "";

    struct termios termStruct;

    //Populate the termios struct with the attributes of the console.
    if(tcgetattr(STDIN_FILENO, &termStruct) != 0) {
        printf("An error occured. Unable to populate the termios struct");
        return 0;
    }

    printf("Disabling echo.\n");
    
    //Determine whether echo is on or off.
    if( (term.c_lflag & ECHO) ) {
        //turn off the echo bit
        termStruct.c_lflag = termStruct & ~ECHO;
        if(tcsetattr(STDIN_FILENO, TCSANOW, &termStruct) != 0) {
        printf("An error occured. Unable to change the terminal's attribute(s)");
        return 0;
        }      
    } 
    
    printf("Enter secret word/phrase: ");
    scanf(&password);

    printf("You entered: %s\n", &password);

    termStruct.c_lflag = termStruct | ECHO;
    if(tcsetattr(STDIN_FILENO, TCSANOW, &termStruct) != 0) {
        printf("An error occured. Unable to change the terminal's attribute(s)");
        return 0;
        }
    
    printf("Default behavior restored.\n");
    printf("Enter visible word/phrase: ");
    scanf(&password);  
    
    return 0;
}