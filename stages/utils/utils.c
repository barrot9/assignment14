#include "utils.h"
#include <ctype.h>
#include <string.h>

/* Array of supported commands */
const char *commands[] = {
    "mov", "cmp", "add", "sub", "lea", "clr", "not", "inc",
    "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop"
};

/* Number of supported commands */
int numCommands = sizeof(commands) / sizeof(commands[0]);

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
