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

    char password[SIZE] = "";

    struct termios termStruct;

    //Populate the termios struct with the attributes of the console.
    if(tcgetattr(STDIN_FILENO, &termStruct) != 0) {
        printf("An error occured. Unable to populate the termios struct");
        return 0;
    }

    printf("\nDisabling echo.\n");
    
    //Determine whether echo is on or off.
    if( (termStruct.c_lflag & ECHO) ) {
        //turn off the echo bit
        termStruct.c_lflag = termStruct.c_lflag & ~ECHO;
        if(tcsetattr(STDIN_FILENO, TCSANOW, &termStruct) != 0) {
        printf("An error occured. Unable to change the terminal's attribute(s)");
        return 0;
        }      
    } 
    
    printf("Enter secret word/phrase: ");
    scanf("%[^\n]s", password);

    printf("\nYou entered: %s\n", password);

    termStruct.c_lflag = termStruct.c_lflag | ECHO;
    if(tcsetattr(STDIN_FILENO, TCSANOW, &termStruct) != 0) {
        printf("An error occured. Unable to change the terminal's attribute(s)");
        return 0;
        }
    
    printf("\nDefault behavior restored.\n");
    printf("Enter visible word/phrase: ");
    scanf("%s", password);
    printf("\n\n");

    printf("OHH YEAH! EXTRA CREDIT\n");
    
    termStruct.c_iflag |= IUCLC;
    if(tcsetattr(STDIN_FILENO, TCSANOW, &termStruct) != 0) {
        printf("An error occured. Unable to change the terminal's attribute(s)");
        return 0;
    }

    printf("Enter a word/phrase in capital letters, go ahead try: ");
    scanf("%s", password);
    printf("You entered: %s", password);

    termStruct.c_iflag &= ~IUCLC;
    	if(tcsetattr(STDIN_FILENO, TCSANOW, &termStruct) != 0) {
       printf("An error occured. Unable to change the terminal's attribute(s)");
       return 0;
    }

    printf("\n\nDefault behavior restored.\n");
    printf("OK, try now: ");
    scanf("%s", password);
    printf("\nYou entered: %s", password);
    printf("\n\n");


    
    return 0;
}
