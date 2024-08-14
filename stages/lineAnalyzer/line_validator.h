#ifndef LINE_VALIDATOR_H
#define LINE_VALIDATOR_H

#include "line_info.h"
#include <stdbool.h>

/* Checks if a command is recognized and retrieves its opcode */
bool isCommandRecognized(const char *command, int *opcode);

/* Checks if a command is in lowercase */
bool isCommandLowercase(const char *command);

bool isReservedWord(const char *word);

/* Checks if a label is valid */
bool isValidLabel(const char *label);

/* Checks if a string is a valid integer */
bool isValidInteger(const char *str);

/* Checks if the operand is a valid register */
bool isValidRegister(const char *operand);

/* Validates the entire line and performs all necessary checks */
bool validateLine(const char *line, char *label, LineType *type, int *opcode, int lineNumber);

/* Expected number of operands for a given opcode */
int getExpectedOperandCount(int opcode);

#endif /* LINE_VALIDATOR_H */
