#include "macro_validation.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h> /* Include ctype.h for isspace function */
#include <stdlib.h> /* Include stdlib.h for exit */

/* List of invalid macro names */
const char *invalidNames[] = {
    "mov", "cmp", "add", "sub", "lea", "clr", "not", "inc", "dec", 
    "jmp", "bne", "red", "prn", "jsr", "rts", "stop", 
    ".data", ".string", ".entry", ".extern"
};
const int numInvalidNames = 20;

/*
 * isValidMacroName: Checks if the macro name is valid.
 */
int isValidMacroName(const char *name) {
    int i;
    if (name[0] == '.') {
        return 0; /* Invalid if the name starts with a dot */
    }
    for (i = 0; i < numInvalidNames; i++) {
        if (strcmp(name, invalidNames[i]) == 0) {
            return 0;
        }
    }
    return 1;
}

/*
 * hasAdditionalCharacters: Checks if a line has additional characters beyond expected.
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
