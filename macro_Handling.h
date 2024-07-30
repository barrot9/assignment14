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

/* Function prototypes */
void processFile(const char *filename);
void expandMacros(FILE *inputFile, Macro *macros, FILE *outputFile);
int readLine(FILE *file, char *buffer, int maxLength);
void addMacro(const char *macroName, Line *lines, Macro **macros);
void addLine(Line **lines, const char *content);
void freeLines(Line *lines);
char *trimWhitespace(char *str);

#endif /* MACRO_HANDLING_H */
