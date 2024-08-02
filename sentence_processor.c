#include "line_recognizer.h"
#include "directive_processor.h"
#include "sentence_processor.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE_LENGTH 256

/* Function Prototypes */
char *trimWhitespace(char *str);

/*
 * processExpandedFile: Processes the expanded file to recognize and handle each sentence type.
 */
void processExpandedFile(const char *filename) {
    char line[MAX_LINE_LENGTH + 2]; /* +2 to handle \n and \0 */
    FILE *file = fopen(filename, "r");

    if (!file) {
        perror("Could not open expanded file");
        return;
    }

    while (fgets(line, sizeof(line), file)) {
        char *trimmedLine = trimWhitespace(line);
        
        /* Recognize the type of the line */
        recognizeLineType(trimmedLine);

        /* Handle each line type accordingly */
        if (isDirectiveSentence(trimmedLine)) {
            processDirective(trimmedLine);
        } else if (isInstructionLine(trimmedLine)) {
            /* Handle instruction lines here */
            printf("Instruction line detected: %s\n", trimmedLine);
            /* Add code to process instructions as needed */
        }
        
        /* Other line types like comments and empty lines are already handled by recognizeLineType */
    }

    fclose(file);
}

/*
 * trimWhitespace: Removes leading and trailing whitespace from a string.
 */
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
