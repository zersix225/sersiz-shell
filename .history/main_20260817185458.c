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

int main() {
    char input[MAX_INPUT];
    char *args1[MAX_ARGS], *args2[MAX_ARGS];

        printBanner();  // Added banner here (runs once)

    while (1) {
        printDir();        // Show current directory
        takeInput(input);  // Get user input

        if (strlen(input) == 0) continue; // Ignore empty input

        char *left, *right;

        // Check if input contains a pipe
        if (parsePipe(input, &left, &right)) {
            parseInput(left, args1);   // Parse left command
            parseInput(right, args2);  // Parse right command

            executePiped(args1, args2); // Execute piped commands
        } else {
            parseInput(input, args1); // Parse normal command

            // Handle built-in commands first
            if (handleBuiltIn(args1)) continue;

            executeCommand(args1); // Execute external command
        }
    }

    return 0;
}