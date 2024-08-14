#ifndef MACRO_HANDLING_H
#define MACRO_HANDLING_H

#include <stdio.h>

/* Constants */
#define MAX_LINE_LENGTH 80

/* Line struct definition */
typedef struct Line {
    char content[MAX_LINE_LENGTH + 1];
    struct Line *next;
} Line;

/* Macro struct definition */
typedef struct Macro {
    char name[MAX_LINE_LENGTH];
    Line *lines;
    struct Macro *next;
} Macro;

/* Function to process a file */
void processFile(const char *filename);

/* Expands macros within a file */
void expandMacros(FILE *inputFile, Macro *macros, FILE *outputFile);

/* Reads a line from file */
int readLine(FILE *file, char *buffer, int maxLength);

/* Adds a macro to the list */
void addMacro(const char *macroName, Line *lines, Macro **macros);

/* Adds a line to a macro */
void addLine(Line **lines, const char *content);

/* Frees allocated memory for lines */
void freeLines(Line *lines);

#endif /* MACRO_HANDLING_H */
