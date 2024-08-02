#ifndef LINE_RECOGNIZER_H
#define LINE_RECOGNIZER_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>

void decimalToBinary(int decimal, char *binary);
void commandToBinary(const char *command, char *binary);
int isEmptyLine(const char *line);
int isCommentLine(const char *line);
int isDirectiveSentence(const char *line);
int isInstructionLine(const char *line);
void processDirective(const char *line);
void processInstruction(const char *line, char *binary);

#endif /* LINE_RECOGNIZER_H */