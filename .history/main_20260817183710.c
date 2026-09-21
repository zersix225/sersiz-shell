#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_INPUT 1024   // Maximum length of user input
#define MAX_ARGS 64      // Maximum number of arguments

void printBanner() {
    printf("\n**********************************************\n\n");
    printf("           ********** My Shell ************\n\n");
    printf("      \"Focus beats talent when talent doesn’t focus.\"  \n\n");
    printf("**********************************************\n\n");
}