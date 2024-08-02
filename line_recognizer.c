#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "line_recognizer.h"

/* Function prototypes */
void decimalToBinary(int decimal, char *binary);
void commandToBinary(const char *command, char *binary);
int isEmptyLine(const char *line);
int isCommentLine(const char *line);
int isDirectiveSentence(const char *line);
int isInstructionLine(const char *line);
void processDirective(const char *line);
void processInstruction(const char *line, char *binary);

/* Array of command names */
const char *commands[] = {
    "mov", "cmp", "add", "sub", "lea", "clr", "not", "inc",
    "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop"
};

/* Entry point for line processing */
void translateLine(const char *line) {
    char binary[5]; /* Array to store binary string (4 bits + null terminator) */

    if (isEmptyLine(line)) {
        printf("Empty line.\n");
        return;
    }

    if (isCommentLine(line)) {
        printf("Comment line.\n");
        return;
    }

    if (isDirectiveSentence(line)) {
        printf("Directive line.\n");
        processDirective(line);
        return;
    }

    if (isInstructionLine(line)) {
        printf("Instruction line.\n");
        processInstruction(line, binary);
        return;
    }

    printf("Unknown line type.\n");
}

/* Convert a decimal number to a 4-bit binary string */
void decimalToBinary(int decimal, char *binary) {
    for (int i = 3; i >= 0; i--) {
        binary[i] = (decimal % 2) + '0';
        decimal /= 2;
    }
    binary[4] = '\0';
}

/* Convert command name to a 4-bit binary string */
void commandToBinary(const char *command, char *binary) {
    int numCommands = sizeof(commands) / sizeof(commands[0]);

    for (int i = 0; i < numCommands; i++) {
        if (strcmp(command, commands[i]) == 0) {
            decimalToBinary(i, binary);
            return;
        }
    }
    strcpy(binary, "0000");
}

int isEmptyLine(const char *line) {
    while (*line) {
        if (!isspace(*line)) {
            return 0;
        }
        line++;
    }
    return 1;
}

int isCommentLine(const char *line) {
    return (line[0] == ';');
}

int isDirectiveSentence(const char *line) {
    return (strncmp(line, ".data", 5) == 0 || strncmp(line, ".text", 5) == 0 ||
            strncmp(line, ".byte", 5) == 0 || strncmp(line, ".word", 5) == 0);
}

void processDirective(const char *line) {
    if (strncmp(line, ".data", 5) == 0) {
        printf("Data section detected.\n");
    } else if (strncmp(line, ".text", 5) == 0) {
        printf("Text section detected.\n");
    }
}

int isInstructionLine(const char *line) {
    int numCommands = sizeof(commands) / sizeof(commands[0]);

    for (int i = 0; i < numCommands; i++) {
        if (strncmp(line, commands[i], strlen(commands[i])) == 0) {
            return 1;
        }
    }
    return 0;
}

void processInstruction(const char *line, char *binary) {
    char command[10];
    sscanf(line, "%s", command);
    commandToBinary(command, binary);
    printf("Instruction: %s -> Binary: %s\n", command, binary);
}
