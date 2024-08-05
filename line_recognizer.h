#ifndef LINE_RECOGNIZER_H
#define LINE_RECOGNIZER_H

#include "line_info.h"

/* Function to detect the type of a line and extract the label */
LineType detectLineType(const char *line, char *label);

/* Function to check if a given line is an instruction line and update the opcode */
bool isInstructionLine(const char *line, int *opcode);

/* Function to create a LineInfo struct for a given line */
LineInfo *createLineInfo(const char *line, LineType type, const char *label);

/* Function to add a new line to the linked list */
void addLineToList(LineInfo **head, LineInfo *newLine);

/* Function to print the type of a line */
void printLineType(LineType type);

/* Function to print all lines in the linked list */
void printLines(const LineInfo *head);

/* Function to check if a given line is empty */
bool isEmptyLine(const char *line);

/* Function to check if a given line is a comment line */
bool isCommentLine(const char *line);

/* Function to check if a given line is a directive sentence */
bool isDirectiveSentence(const char *line);

/* Function to parse operands for instructions */
void parseOperands(const char *line, LineInfo *info, int opcode);

/* Function to parse operands for .data directive */
void parseDataOperands(const char *line, LineInfo *info);

/* Function to parse operand for .string directive */
void parseStringOperand(const char *line, LineInfo *info);

/* Function to detect the addressing method of an operand */
AddressingMethod detectAddressingMethod(const char *operand);

/* Function to print the addressing method */
void printAddressingMethod(AddressingMethod method);

#endif /* LINE_RECOGNIZER_H */
