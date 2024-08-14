#ifndef MACRO_VALIDATION_H
#define MACRO_VALIDATION_H

#include "macro_Handling.h"
#include <stdbool.h>

/* Function Prototypes */
int isValidMacroName(const char *name);
int hasAdditionalCharacters(const char *line);
int validateMacros(const char *filename);
bool isMacroName(const char *name);
void addMacroName(const char *name);

#endif /* MACRO_VALIDATION_H */
