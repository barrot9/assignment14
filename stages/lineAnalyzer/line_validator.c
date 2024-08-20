/*
   This module is responsible for validating the lines of the assembly code. 
   It includes functions to check the correctness of labels, commands, 
   operands, and directives in the assembly source file.
   This module plays the role in detecting 
   and reporting errors during the first pass of the assembler.
*/

#include "line_validator.h"
#include "line_recognizer.h"
#include "../utils/utils.h"
#include "../preProcessor/macro_validation.h"
#include "line_info.h"
#include <ctype.h>
#include <string.h>
#include <stdio.h>

#define MAX_LINE_LENGTH 80

int getExpectedOperandCount(int opcode) {
    if (opcode >= 0 && opcode <= 4) {  /* mov, cmp, add, sub, lea */
        return 2;
    } else if (opcode >= 5 && opcode <= 13) {  /* clr, not, inc, dec, jmp, bne, red, prn, jsr */
        return 1;
    } else if (opcode >= 14 && opcode <= 15) {  /* rts, stop */
        return 0;
    } else {
        return -1;  /* Invalid opcode */
    }
}


/* Check if a string is a valid integer */
bool isValidInteger(const char *str) {
    if (*str == '-' || *str == '+') {
        str++;
    }
    while (*str) {
        if (!isdigit(*str)) {
            return false;
        }
        str++;
    }
    return true;
}

/* Check if the operand is a valid register */
bool isValidRegister(const char *operand) {
    int i;
    for (i = 0; i < numRegisters; i++) {
        if (strcmp(operand, registers[i]) == 0) {
            return true;
        }
    }
    return false;
}

bool isValidLabel(const char *label) {
    int i;
    /* Ensure that label is not empty*/
    if (strlen(label) == 0) {
        return false;
    }

    if (isMacroName(label)) {
        printf("Label conflicts with a macro name: %s\n", label);
        return false;
    }
    /* Check that the label starts with a letter */
    if (!isalpha(label[0])) {
        printf("Label: %s does not start with an alphabet character.\n", label);
        return false;
    }

    /* Check that the rest of the label is alphanumeric */
    for (i = 1; label[i] != '\0'; i++) {
        if (!isalnum(label[i])) {
            printf("Non-alphanumeric character found: %c\n", label[i]);
            return false;
        }
    }

    /* Check label length */
    if (strlen(label) > MAX_LABEL_LENGTH) {
        printf("Label exceeds max length.\n");
        return false;
    }

    /* Check if the label is a reserved word */
    if (isReservedWord(label)) {
        printf("Error: Label '%s' is a reserved word.\n", label);
        return false;
    }

    return true;
}


/* Function to check if a command is in lowercase */
bool isCommandLowercase(const char *command) {
    int i;  /* Declare the loop variable at the beginning */
    
    for (i = 0; command[i] != '\0'; i++) {
        if (!islower(command[i])) {
            return false;
        }
    }
    return true;
}

bool isCommandRecognized(const char *command, int *opcode) {
    int i, numCommands;  
    numCommands = getNumCommands();  /* Ensure getNumCommands is declared */

    for (i = 0; i < numCommands; i++) {
        if (strcmp(command, getCommand(i)) == 0) {
            *opcode = i;  /* Store the index as the opcode */
            return true;
        }
    }
    return false;
}


bool validateLine(const char *line, char *label, LineType *type, int *opcode, int lineNumber) {
    char command[MAX_LINE_LENGTH];
    char modifiableLine[MAX_LINE_LENGTH + 1]; /* Create a modifiable copy of the line */
    char *ptr, *token, *stringStart, *stringEnd, *dataPtr;
    int operandCount = 0;
    int i, expectedOperands;
    char *operands[MAX_OPERANDS];

    /* Check if line length is above 80 characters */
    if (strlen(line) > MAX_LINE_LENGTH) {
        fprintf(stderr, "Error on line %d: Line exceeds 80 characters.\n", lineNumber);
        return false;
    }

    /* Create a modifiable copy of the line */
    strncpy(modifiableLine, line, MAX_LINE_LENGTH);
    modifiableLine[MAX_LINE_LENGTH] = '\0';

    /* Reset label */
    label[0] = '\0';

    /* Check if the line contains a label */
    ptr = modifiableLine;
    if (strchr(modifiableLine, ':') != NULL) {
        /* Extract the label up to the colon */
        sscanf(modifiableLine, "%[^:]", label);
        if (!isValidLabel(label)) {
            fprintf(stderr, "Error on line %d: Invalid label '%s'.\n", lineNumber, label);
            return false;
        }

        /* Move the pointer past the label and colon */
        ptr = strchr(modifiableLine, ':') + 1;
        ptr = trimWhitespace(ptr);  /* Trim any leading whitespace */
    }

    /* Detect the line type */
    *type = detectLineType(modifiableLine, label);
    
    if (*type == LINE_EMPTY || *type == LINE_COMMENT) {
        return true;
    } else if (*type == LINE_INSTRUCTION) {
        /* Extract the command */
        sscanf(ptr, "%s", command);

        /* Remove trailing comma, if present */
        if (command[strlen(command) - 1] == ',') {
            command[strlen(command) - 1] = '\0';
        }

        /* Move the pointer past the command */
        ptr += strlen(command);

        /* Skip any whitespace */
        while (isspace((unsigned char)*ptr)) ptr++;

        /* Parse operands */
        while (*ptr != '\0' && operandCount < MAX_OPERANDS) {
            /* Skip any leading whitespace */
            while (isspace((unsigned char)*ptr)) {
                ptr++;
            }

            /* Start of an operand */
            token = ptr;
            /* Find the end of the operand */
            while (*ptr != ',' && !isspace((unsigned char)*ptr) && *ptr != '\0') {
                ptr++;
            } 

            if (*ptr == ',') {
                *ptr++ = '\0';  /* Null-terminate the operand and move past the comma */
            }

            /* Trim the operand */
            token = trimWhitespace(token);

            if (strlen(token) > 0) {
                operands[operandCount++] = token;
            }
        }

        /* Check if command is recognized and in lowercase */
        if (!isCommandLowercase(command)) {
            fprintf(stderr, "Error on line %d: Command '%s' is not in lowercase.\n", lineNumber, command);
            return false;
        }
        if (!isCommandRecognized(command, opcode)) {
            fprintf(stderr, "Error on line %d: Command '%s' is not recognized.\n", lineNumber, command);
            return false;
        }

        /* Validate operands */
        for (i = 0; i < operandCount; i++) {
            if (operands[i][0] == '#') {
                /* Immediate operand */
                if (!isValidInteger(operands[i] + 1)) {
                    fprintf(stderr, "Error on line %d: Invalid immediate operand '%s'.\n", lineNumber, operands[i]);
                    return false;
                }
            } else if (operands[i][0] == '*') {
                /* Register operand */
                if (!isValidRegister(operands[i] + 1)) {
                    fprintf(stderr, "Error on line %d: Invalid register operand '%s'.\n", lineNumber, operands[i]);
                    return false;
                }
            } else {
                /* Potential Label: Check if it starts with an alphabetic character */
                if (isalpha(operands[i][0])) {
                    /* Check if it's a valid label */
                    if (!isValidLabel(operands[i])) {
                        fprintf(stderr, "Error on line %d: Invalid label '%s'.\n", lineNumber, operands[i]);
                        return false;
                    }
                } else {
                    /* If it doesn't start with a letter, it can't be a label */
                    fprintf(stderr, "Error on line %d: Invalid operand '%s'.\n", lineNumber, operands[i]);
                    return false;
                }
            }
        }


        /* Check operand count based on the command */
        expectedOperands = getExpectedOperandCount(*opcode);
        if (operandCount < expectedOperands) {
            fprintf(stderr, "Error on line %d: Command '%s' expects %d operands but got %d.\n", lineNumber, command, expectedOperands, operandCount);
            return false;
        }

        /* Check for extra text after operands */
        if (*ptr != '\0') {
            fprintf(stderr, "Error on line %d: Extra text after operands in command '%s'.\n", lineNumber, command);
            return false;
        }

    } else if (*type == LINE_DIRECTIVE) {
        if (strncmp(ptr, ".data", 5) == 0) {
            /* Validate .data operands */
            dataPtr = ptr + 5;

            while (*dataPtr != '\0') {
                token = dataPtr;
                while (*dataPtr != ',' && *dataPtr != '\0') dataPtr++;

                if (*dataPtr == ',') {
                    *dataPtr++ = '\0';  /* Null-terminate the operand and move past the comma */
                }

                token = trimWhitespace(token);
                if (!isValidInteger(token)) {
                    fprintf(stderr, "Error on line %d: Invalid integer '%s' in .data directive.\n", lineNumber, token);
                    return false;
                }
            }
        } else if (strncmp(ptr, ".string", 7) == 0) {
            /* Validate .string operand */
            stringStart = strchr(ptr, '"');
            stringEnd = strrchr(ptr, '"');
            if (!stringStart || stringStart == stringEnd) {
                fprintf(stderr, "Error on line %d: Invalid string in .string directive.\n", lineNumber);
                return false;
            }

            /* Check for extra text after the closing quote */
            stringEnd++;
            while (isspace((unsigned char)*stringEnd)) {
                stringEnd++;
            }
            if (*stringEnd != '\0') {
                fprintf(stderr, "Error on line %d: Extra text after closing quote in .string directive.\n", lineNumber);
                return false;
            }
        } else if (strncmp(ptr, ".entry", 6) == 0 || strncmp(ptr, ".extern", 7) == 0) {
            /* .entry and .extern should have valid labels */
            /* Move the pointer past the directive */
            if (strncmp(ptr, ".entry", 6) == 0) {
                ptr += 6;
            } else {
                ptr += 7;
            }
            
            /* Move past any whitespace */
            while (isspace((unsigned char)*ptr)) {
                ptr++;
            }

            /* Extract the label */
            i = 0;
            while (isalnum((unsigned char)ptr[i])) {
                label[i] = ptr[i];
                i++;
            }
            
            label[i] = '\0';
            if (strlen(label) == 0) {
                fprintf(stderr, "Error on line %d: Label is missing after directive.\n", lineNumber);
                return false;
            }
            
            if (!isValidLabel(label)) {
                fprintf(stderr, "Error on line %d: Invalid label '%s' in directive.\n", lineNumber, label);
                return false;
            }

            /* Check for extra text after the label */
            ptr += i;  /* Move the pointer past the label */
            while (isspace((unsigned char)*ptr)) {
                ptr++;
            }
           
            
            if (*ptr != '\0') {
                fprintf(stderr, "Error on line %d: Extra text after label in directive '%s'.\n", lineNumber, line);
                return false;
            }
        } else {
            fprintf(stderr, "Error on line %d: Unknown directive '%s'.\n", lineNumber, ptr);
            return false;
        }
    } else {
        /* Handle unexpected or undefined keywords */
        fprintf(stderr, "Error on line %d: Unexpected or undefined keyword in line: '%s'.\n", lineNumber, line);
        return false;
    }
    
    return true; /* Line is valid */
}



