#include "line_recognizer.h"
#include "line_info.h"
#include "../utils/utils.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define NUM_REGISTERS 8

const char *registers[] = {
    "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"
};

const int numRegisters = sizeof(registers) / sizeof(registers[0]);

/* Checks if a given line is empty */
bool isEmptyLine(const char *line) {
    while (*line) {
        if (!isspace(*line)) {
            return false; /* Not empty */
        }
        line++;
    }
    return true; /* Empty line */
}

/* Checks if a given line is a comment line */
bool isCommentLine(const char *line) {
    return (line[0] == ';');
}

/* Checks if a given line is a directive sentence */
bool isDirectiveSentence(const char *line) {
    /* Trim leading whitespace */
    while (isspace(*line)) {
        line++;
    }
    /* Check for directive keywords */
    return (strncmp(line, ".data", 5) == 0 || strncmp(line, ".string", 7) == 0 ||
            strncmp(line, ".entry", 6) == 0 || strncmp(line, ".extern", 7) == 0);
}

/* Checks if a given line is an instruction line */
bool isInstructionLine(const char *line, int *opcode) {
    const char *commands[] = {
        "mov", "cmp", "add", "sub", "lea", "clr", "not", "inc",
        "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop"
    };
    int numCommands = sizeof(commands) / sizeof(commands[0]), i;
    size_t commandLength;

    /* Trim leading whitespace */
    while (isspace(*line)) {
        line++;
    }

    for (i = 0; i < numCommands; i++) {
        commandLength = strlen(commands[i]);
        if  (strncmp(line, commands[i], commandLength) == 0 &&
            (isspace(line[commandLength]) || line[commandLength] == '\0' || line[commandLength] == ',')) {
            *opcode = i;  /* Assign the index as the opcode */
            return true; /* Instruction line found */
        }
    }
    *opcode = -1;  /* Not an instruction line, set opcode to -1 */
    return false; /* Not an instruction line */
}

/* Detects the type of a line and extracts the label if present */
LineType detectLineType(const char *line, char *label) {
    char *colonPos;
    int opcode;
    char *lineCopy,*trimmedLine;
    /* Initialize label */
    label[0] = '\0';
    
    /* Make a copy of the line to work with */
    lineCopy = (char *)malloc(strlen(line) + 1);  /* Allocate memory for the copy */
    if (!lineCopy) {
        perror("Failed to allocate memory for line copy");
        exit(EXIT_FAILURE);
    }
    strcpy(lineCopy, line);  /* Copy the line content */

    /* Trim whitespace from the line */
    trimmedLine = trimWhitespace(lineCopy);

    /* Check for empty line */
    if (isEmptyLine(trimmedLine)) {
        free(lineCopy);
        return LINE_EMPTY;
    }

    /* Check for comment line */
    if (isCommentLine(trimmedLine)) {
        free(lineCopy);
        return LINE_COMMENT;
    }

    /* Check for label at the beginning of the line */
    colonPos = strchr(trimmedLine, ':');
    if (colonPos) {
        size_t labelLength = colonPos - trimmedLine;
        if (labelLength <= MAX_LABEL_LENGTH) {
            strncpy(label, trimmedLine, labelLength);
            label[labelLength] = '\0'; /* Null-terminate the label */

            /* Move past the label and colon for further processing */
            trimmedLine = colonPos + 1;
            /* Trim leading whitespace after the label */
            trimmedLine = trimWhitespace(trimmedLine);
        }
    }

    /* Check for directive sentence */
    if (isDirectiveSentence(trimmedLine)) {
        free(lineCopy);
        return LINE_DIRECTIVE;
    }

    /* Check for instruction line */
    opcode = -1;
    if (isInstructionLine(trimmedLine, &opcode)) {
        free(lineCopy);
        return LINE_INSTRUCTION;
    }

    free(lineCopy);
    return LINE_UNKNOWN;
}

/* Creates a LineInfo struct for a given line */
LineInfo *createLineInfo(const char *line, LineType type, const char *label) {
    int opcode = -1;
    char *commandStart;
    LineInfo *info = (LineInfo *)malloc(sizeof(LineInfo));
    if (!info) {
        perror("Failed to allocate memory for line information");
        exit(EXIT_FAILURE);
    }

    strncpy(info->line, line, MAX_LINE_LENGTH);
    info->type = type;
    strncpy(info->label, label, MAX_LABEL_LENGTH);
    info->opcode = -1;  /* Opcode handling to be implemented */
    info->operands[0][0] = '\0';  /* Initialize operand storage */
     info->operandCount = 0;
    info->isExtern = false;  /* Extern flag */
    info->isEntry = false;   /* Entry flag */
    info->next = NULL;

    /*Trim the original line after the label and process further */
    commandStart = info->line + strlen(label);
    if (*commandStart == ':') {
        commandStart++;
    }
    commandStart = trimWhitespace(commandStart);

    /* Determine the opcode */
    if (type == LINE_INSTRUCTION && isInstructionLine(commandStart, &opcode)) {
        info->opcode = opcode;
        parseOperands(commandStart, info, opcode);
    } else {
        info->opcode = -1;  /* Non-instruction lines have no opcode */
    }

    /* Set entry or extern flags based on the directive */
    if (type == LINE_DIRECTIVE) {
        if (strncmp(commandStart, ".entry", 6) == 0) {
            info->isEntry = true;
            info->isExtern = false;  /* Ensure extern is not set */
        } else if (strncmp(commandStart, ".extern", 7) == 0) {
            info->isExtern = true;
            info->isEntry = false;  /* Ensure entry is not set*/
        } else if (strncmp(commandStart, ".data", 5) == 0) {
            parseDataOperands(commandStart + 5, info);
        } else if (strncmp(commandStart, ".string", 7) == 0) {
            parseStringOperand(commandStart + 7, info);
        }
    }
    return info;
}

void parseOperands(const char *line, LineInfo *info, int opcode) {
    char buffer[MAX_LINE_LENGTH], *ptr, *token, *trimmedToken;
    int expectedOperands;
    strncpy(buffer, line, MAX_LINE_LENGTH);
    ptr = buffer;

    /* Skip the command */
    token = strtok(ptr, " \t");
    /* Check the expected number of operands */
    expectedOperands = 0;
    if (opcode >= 0 && opcode <= 4) {  /* 2 operands: mov, cmp, add, sub, lea */
        expectedOperands = 2;
    } else if (opcode >= 5 && opcode <= 12) {  /* 1 operand: clr, not, inc, dec, jmp, bne, red, prn, jsr */
        expectedOperands = 1;
    } /* rts and stop expect 0 operands */

    /* Parse operands */
    token = strtok(NULL, ",");
    while (token != NULL && info->operandCount < expectedOperands) {
        trimmedToken = trimWhitespace(token); /* Use trimmedToken to receive the result */
        strncpy(info->operands[info->operandCount], trimmedToken, MAX_OPERAND_LENGTH);
        info->addressingMethods[info->operandCount] = detectAddressingMethod(trimmedToken);
        info->operandCount++;
        token = strtok(NULL, ",");
    }
}

void parseDataOperands(const char *line, LineInfo *info) {
    char buffer[MAX_LINE_LENGTH], *token;
    strncpy(buffer, line, MAX_LINE_LENGTH);
    token = strtok(buffer, ",");
    while (token != NULL && info->operandCount < MAX_OPERANDS) {
        token = trimWhitespace(token);
        strncpy(info->operands[info->operandCount], token, MAX_OPERAND_LENGTH);
        info->operandCount++;
        token = strtok(NULL, ",");
    }
}

void parseStringOperand(const char *line, LineInfo *info) {
    char buffer[MAX_LINE_LENGTH], *start;
    strncpy(buffer, line, MAX_LINE_LENGTH);
    start = strchr(buffer, '"');
    if (start) {
        char *end = strrchr(start + 1, '"');
        if (end) {
            *(end + 1) = '\0';  /* Null-terminate the string */
            strncpy(info->operands[0], start, MAX_OPERAND_LENGTH);
            info->operandCount = 1;
        }
    }
}

AddressingMethod detectAddressingMethod(const char *operand) {
    const char *registerName;
    int i;
    if (operand[0] == '#') {
        return ADDRESSING_IMMEDIATE;
    } else if (operand[0] == '*') {
        registerName = operand + 1;
        for (i = 0; i < numRegisters; i++) {
            if (strcmp(registerName, registers[i]) == 0) {
                return ADDRESSING_INDIRECT_REGISTER;
            }
        }
    } else {
        for (i = 0; i < numRegisters; i++) {
            if (strcmp(operand, registers[i]) == 0) {
                return ADDRESSING_DIRECT_REGISTER;
            }
        }
        return ADDRESSING_DIRECT;  /* Assuming the operand is a label */
    }
    return ADDRESSING_UNKNOWN;
}

/* Adds a new line to the linked list */
void addLineToList(LineInfo **head, LineInfo *newLine) {
    if (!*head) {
        *head = newLine;
    } else {
        LineInfo *current = *head;
        while (current->next) {
            current = current->next;
        }
        current->next = newLine;
    }
}


/* Prints the type of the line */
void printLineType(LineType type) {
    switch (type) {
        case LINE_EMPTY:
            printf("Empty line\n");
            break;
        case LINE_COMMENT:
            printf("Comment line\n");
            break;
        case LINE_DIRECTIVE:
            printf("Directive line\n");
            break;
        case LINE_INSTRUCTION:
            printf("Instruction line\n");
            break;
        default:
            printf("Unknown line type\n");
            break;
    }
}

void printAddressingMethod(AddressingMethod method) {
    switch (method) {
        case ADDRESSING_IMMEDIATE:
            printf("Immediate");
            break;
        case ADDRESSING_DIRECT:
            printf("Direct");
            break;
        case ADDRESSING_INDIRECT_REGISTER:
            printf("Indirect Register");
            break;
        case ADDRESSING_DIRECT_REGISTER:
            printf("Direct Register");
            break;
        default:
            printf("Unknown");
            break;
    }
}

/* Prints all lines in the linked list */
void printLines(const LineInfo *head) {
    const LineInfo *current = head;
    int i;
    while (current) {
        printf("Line: %s\n", current->line);
        printLineType(current->type);
        if (strlen(current->label) > 0) {
            printf("Label: %s\n", current->label);
        }
        if (current->type == LINE_INSTRUCTION) {
            printf("Opcode: %d\n", current->opcode);
        }
        if (current->operandCount > 0) {
            printf("Operands (%d):", current->operandCount);
            for (i = 0; i < current->operandCount; i++) {
                printf(" %s", current->operands[i]);
                if (i < current->operandCount - 1) {
                    printf(",");
                }
            }
            printf("\n");
            printf("Addressing Methods:");
                for (i = 0; i < current->operandCount; i++) {
                    printf(" ");
                    printAddressingMethod(current->addressingMethods[i]);
                    if (i < current->operandCount - 1) {
                        printf(",");
                    }
                }
            printf("\n");
        } else if (current->type == LINE_DIRECTIVE && current->operandCount > 0) {
            printf("Operands (%d):", current->operandCount);
            for (i = 0; i < current->operandCount; i++) {
                printf(" %s", current->operands[i]);
                if (i < current->operandCount - 1) {
                    printf(",");
                }
            }
            printf("\n");

        }
        if (current->isEntry) {
            printf("This line is an entry.\n");
        }
        if (current->isExtern) {
            printf("This line is an extern.\n");
        }
        current = current->next;
    }
}
