#include "macro_Handling.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h> /* Include ctype.h for isspace function */

/* Function Prototypes */
void processFile(const char *filename);
void expandMacros(FILE *inputFile, Macro *macros, FILE *outputFile);
int readLine(FILE *file, char *buffer, int maxLength);
void addMacro(const char *macroName, Line *lines, Macro **macros);
void addLine(Line **lines, const char *content);
void freeLines(Line *lines);

/*
 * processFile: Reads the input file line by line, detects macro definitions,
 * stores them, and calls expandMacros to process and expand macros in the file content,
 * writing the result to a new file.
 */
void processFile(const char *filename) {
    char line[MAX_LINE_LENGTH + 2]; /* +2 to handle \n and \0 */
    Macro *macros = NULL;
    FILE *file = fopen(filename, "r");
    FILE *outputFile = fopen("output.asm", "w");

    if (!file) {
        perror("Could not open file");
        return;
    }

    if (!outputFile) {
        perror("Could not open output file");
        fclose(file);
        return;
    }

    while (readLine(file, line, MAX_LINE_LENGTH + 2)) {
        char *trimmedLine = trimWhitespace(line);
        if (strncmp(trimmedLine, "macr", 4) == 0) {
            char macroName[MAX_LINE_LENGTH];
            Line *lines = NULL;
            sscanf(trimmedLine, "macr %s", macroName);

            while (readLine(file, line, MAX_LINE_LENGTH + 2)) {
                trimmedLine = trimWhitespace(line);
                if (strcmp(trimmedLine, "endmacr") == 0) {
                    break;
                }
                addLine(&lines, trimmedLine);
            }
            addMacro(macroName, lines, &macros);
        }
    }

    rewind(file);
    expandMacros(file, macros, outputFile);

    fclose(file);
    fclose(outputFile);

    /* Free the linked list */
    while (macros != NULL) {
        Macro *next = macros->next;
        freeLines(macros->lines);
        free(macros);
        macros = next;
    }
}

/*
 * addMacro: Adds a macro definition to the linked list of macros.
 */
void addMacro(const char *macroName, Line *lines, Macro **macros) {
    Macro *newMacro = (Macro *)malloc(sizeof(Macro));

    if (newMacro == NULL) {
        perror("Failed to allocate memory for macro");
        exit(1);
    }
    strcpy(newMacro->name, macroName);
    newMacro->lines = lines;
    newMacro->next = *macros;
    *macros = newMacro;
}

/*
 * addLine: Adds a line to the linked list of lines.
 */
void addLine(Line **lines, const char *content) {
    Line *newLine = (Line *)malloc(sizeof(Line));
    Line *temp;

    if (newLine == NULL) {
        perror("Failed to allocate memory for line");
        exit(1);
    }
    strcpy(newLine->content, content);
    newLine->next = NULL;

    if (*lines == NULL) {
        *lines = newLine;
    } else {
        temp = *lines;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newLine;
    }
}

/*
 * freeLines: Frees the linked list of lines.
 */
void freeLines(Line *lines) {
    while (lines != NULL) {
        Line *next = lines->next;
        free(lines);
        lines = next;
    }
}

/*
 * expandMacros: Expands the macros found in the file content and writes the expanded content
 * to the output file.
 */
void expandMacros(FILE *inputFile, Macro *macros, FILE *outputFile) {
    char line[MAX_LINE_LENGTH + 2]; /* +2 to handle \n and \0 */
    int isMacro;
    Macro *current;

    while (readLine(inputFile, line, MAX_LINE_LENGTH + 2)) {
        char *trimmedLine = trimWhitespace(line);

        /* Skip macro definitions in the output */
        if (strncmp(trimmedLine, "macr", 4) == 0) {
            while (readLine(inputFile, line, MAX_LINE_LENGTH + 2)) {
                trimmedLine = trimWhitespace(line);
                if (strcmp(trimmedLine, "endmacr") == 0) {
                    break;
                }
            }
            continue;
        }

        isMacro = 0;
        current = macros;

        while (current != NULL) {
            if (strcmp(trimmedLine, current->name) == 0) {
                Line *macroLine;
                macroLine = current->lines;
                while (macroLine != NULL) {
                    fprintf(outputFile, "%s\n", macroLine->content);
                    macroLine = macroLine->next;
                }
                isMacro = 1;
                break;
            }
            current = current->next;
        }

        if (!isMacro) {
            fprintf(outputFile, "%s\n", trimmedLine);
        }
    }
}

/*
 * readLine: Reads a line from the file, ensuring it handles the newline character properly.
 */
int readLine(FILE *file, char *buffer, int maxLength) {
    size_t length;
    if (fgets(buffer, maxLength, file) == NULL) {
        return 0;
    }

    /* Remove newline character if present */
    length = strlen(buffer);
    if (buffer[length - 1] == '\n') {
        buffer[length - 1] = '\0';
    }

    return 1;
}

