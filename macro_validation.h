#ifndef MACRO_VALIDATION_H
#define MACRO_VALIDATION_H

#include "macro_Handling.h"

/* Function Prototypes */
int isValidMacroName(const char *name);
int hasAdditionalCharacters(const char *line);
void validateMacros(const char *filename);

#endif /* MACRO_VALIDATION_H */
