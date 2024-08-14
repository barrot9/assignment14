#ifndef MACRO_VALIDATION_H
#define MACRO_VALIDATION_H

#include "macro_Handling.h"
#include <stdbool.h>

/* Checks if macro name is valid */
int isValidMacroName(const char *name);

/* Detects extra characters after macro definition */
int hasAdditionalCharacters(const char *line);

/* Validates macros in a given file */
int validateMacros(const char *filename);

/* Determines if string is a macro name */
bool isMacroName(const char *name);

/* Adds a macro name to the list */
void addMacroName(const char *name);

#endif /* MACRO_VALIDATION_H */

