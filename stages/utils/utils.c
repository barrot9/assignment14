#include "utils.h"
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
/* Array of supported commands */
const char *commands[] = {
    "mov", "cmp", "add", "sub", "lea", "clr", "not", "inc",
    "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop"
};

/* Number of supported commands */
int numCommands = sizeof(commands) / sizeof(commands[0]);

const char *reservedWords[] = {
    "mov", "cmp", "add", "sub", "lea", 
    "clr", "not", "inc", "dec", "jmp", 
    "bne", "red", "prn", "jsr", "rts", "stop", 
    ".data", ".string", ".entry", ".extern"
};
const int numReservedWords = sizeof(reservedWords) / sizeof(reservedWords[0]);

bool isReservedWord(const char *word) {
    
    for (int i = 0; i < numReservedWords; i++) {
        if (strcmp(word, reservedWords[i]) == 0) {
            return true;
        }
    }
    return false;
}

/* Implementation of trimWhitespace function */
char *trimWhitespace(char *str) {
    char *end;

    /* Trim leading space */
    while (isspace((unsigned char)*str)) str++;

    if (*str == 0)  /* All spaces? */
        return str;

    /* Trim trailing space */
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    /* Write new null terminator */
    end[1] = '\0';

    return str;
}


/* Function to get the number of commands */
int getNumCommands() {
    return numCommands;
}

/* Function to get a command by index */
const char *getCommand(int index) {
    if (index >= 0 && index < numCommands) {
        return commands[index];
    }
    return NULL;
}
