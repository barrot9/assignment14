#ifndef LINE_INFO_H
#define LINE_INFO_H

#include <stdbool.h>  /* For bool type */

#define MAX_LINE_LENGTH 80
#define MAX_LABEL_LENGTH 31
#define MAX_OPERANDS 10
#define MAX_OPERAND_LENGTH 30

typedef enum {
    LINE_UNKNOWN,
    LINE_EMPTY,
    LINE_COMMENT,
    LINE_DIRECTIVE,
    LINE_INSTRUCTION
} LineType;

/* Enumeration for addressing methods */
typedef enum {
    ADDRESSING_IMMEDIATE,
    ADDRESSING_DIRECT,
    ADDRESSING_INDIRECT_REGISTER,
    ADDRESSING_DIRECT_REGISTER,
    ADDRESSING_UNKNOWN
} AddressingMethod;

typedef struct LineInfo {
    char line[MAX_LINE_LENGTH];
    LineType type;
    char label[MAX_LABEL_LENGTH + 1];  /* To store the label if present */
    int opcode; /* Opcode for instruction lines */
    char operands[MAX_OPERANDS][MAX_OPERAND_LENGTH]; /* Operands */
    int operandCount;            /* Number of operands */
    AddressingMethod addressingMethods[MAX_OPERANDS]; /* Addressing methods */
    bool isExtern;
    bool isEntry;
    struct LineInfo *next;
} LineInfo;

/* Array of valid registers */
extern const char *registers[];
extern const int numRegisters;

#endif /* LINE_INFO_H */
