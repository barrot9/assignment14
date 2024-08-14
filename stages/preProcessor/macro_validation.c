#include "macro_validation.h"
#include "../utils/utils.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h> /* Include ctype.h for isspace function */
#include <stdlib.h> /* Include stdlib.h for exit */

#define MAX_MACROS 100
#define MAX_MACRO_NAME_LENGTH 31


static char macroNames[MAX_MACROS][MAX_MACRO_NAME_LENGTH + 1]; /* Array to store defined macro names */
static int macroCount = 0; /* Counter for the number of macros */

/*
 * isValidMacroName: Checks if the macro name is valid.
 * 
 * Parameters:
 * - name: The name of the macro to be validated.
 * 
 * Returns:
 * - 1 if the name is valid, 0 otherwise.
 */
int isValidMacroName(const char *name) {
    int i;
    if (name[0] == '.') {
        return 0; /* Invalid if the name starts with a dot */
    }
    for (i = 0; i < numReservedWords; i++) {
        if (strcmp(name, reservedWords[i]) == 0) {
            return 0;
        }
    }
    return 1;
}

/*
 * isMacroName: Checks if the name is a macro.
 * 
 * Parameters:
 * - name: The name to check against the list of defined macros.
 * 
 * Returns:
 * - true if the name is a macro, false otherwise.
 */
bool isMacroName(const char *name) {
    int i;
    for (i = 0; i < macroCount; i++) {
        if (strcmp(macroNames[i], name) == 0) {
            return true;
        }
    }
    return false;
}


/*
 * addMacroName: Adds a macro name to the list of macro names.
 * 
 * Parameters:
 * - name: The name of the macro to be added.
 */
void addMacroName(const char *name) {
    if (macroCount < MAX_MACROS) {
        strncpy(macroNames[macroCount], name, MAX_MACRO_NAME_LENGTH);
        macroNames[macroCount][MAX_MACRO_NAME_LENGTH] = '\0';
        macroCount++;
    } else {
        fprintf(stderr, "Error: Maximum number of macros exceeded.\n");
    }
}

/*
 * hasAdditionalCharacters: Checks if a line has additional characters beyond expected.
 * 
 * Parameters:
 * - line: The line to be checked for additional characters.
 * 
 * Returns:
 * - 0 if the line is valid, 1 if there are additional characters.
 */
int hasAdditionalCharacters(const char *line) {
    const char *trimmedLine = trimWhitespace((char *)line);
    if (strncmp(trimmedLine, "macr ", 5) == 0) {
        char macroName[MAX_LINE_LENGTH];
        sscanf(trimmedLine, "macr %s", macroName);
        if (strchr(macroName, ' ') != NULL) {
            return 1; /* Invalid if there are spaces in the macro name */
        }
        return 0;
    }
    if (strcmp(trimmedLine, "endmacr") == 0) {
        return 0;
    }
    return 1;
}


/*
 * validateMacros: Validates macro names and ensures no additional characters.
 * 
 * Parameters:
 * - filename: The name of the file containing macros to be validated.
 * 
 * Returns:
 * - 0 on success, 1 on failure.
 */
int validateMacros(const char *filename) {
    char line[MAX_LINE_LENGTH + 2]; /* +2 to handle \n and \0 */
    FILE *file = fopen(filename, "r");

    if (!file) {
        perror("Could not open file");
        return 1; /* Error code */
    }

    while (readLine(file, line, MAX_LINE_LENGTH + 2)) {
        char *trimmedLine = trimWhitespace(line);
        if (strncmp(trimmedLine, "macr", 4) == 0) {
            char macroName[MAX_LINE_LENGTH];
            sscanf(trimmedLine, "macr %s", macroName);

            if (!isValidMacroName(macroName)) {
                fprintf(stderr, "Error: Invalid macro name '%s'\n", macroName);
                fclose(file);
                return 1; /* Error code */
            }
            addMacroName(macroName);
            if (hasAdditionalCharacters(trimmedLine)) {
                fprintf(stderr, "Error: Macro definition line has additional characters: '%s'\n", trimmedLine);
                fclose(file);
                return 1; /* Error code */
            }

            while (readLine(file, line, MAX_LINE_LENGTH + 2)) {
                trimmedLine = trimWhitespace(line);
                if (strncmp(trimmedLine, "endmacr", 7) == 0) {
                    if (hasAdditionalCharacters(trimmedLine)) {
                        fprintf(stderr, "Error: Macro end line has additional characters: '%s'\n", trimmedLine);
                        fclose(file);
                        return 1; /* Error code */
                    }
                    break; /* Correctly break out of the loop */
                }
            }

            if (strcmp(trimmedLine, "endmacr") != 0) {
                fprintf(stderr, "Error: Macro end line has additional characters: '%s'\n", trimmedLine);
                fclose(file);
                return 1; /* Error code */
            }
        }
    }

    fclose(file);
    return 0; /* Success code */
}
