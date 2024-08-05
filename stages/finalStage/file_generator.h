#ifndef FILE_GENERATOR_H
#define FILE_GENERATOR_H
#include <stdbool.h>

/* Node structure for the linked list */
typedef struct Node {
    short binaryValue;      /* 15-bit binary value represented as a short */
    int addressIndex;        /* Memory address index */
    struct Node *next;       /* Pointer to the next node */
} Node;

/* Symbol structure to store symbol details */
typedef struct Symbol {
    char name[50];          /* Symbol name */
    bool isEntry;           /* Flag to indicate if the symbol is an entry */
    bool isExternal;        /* Flag to indicate if the symbol is external */
    int *usageAddresses;    /* Array of addresses where the symbol is used */
    int usageCount;         /* Number of times the symbol is used */
    struct Symbol *next;    /* Pointer to the next symbol */
} Symbol;

/* Input structure containing the linked list and other relevant data */
typedef struct {
    Node *head;             /* Head of the linked list */
    int instructionLength;  /* Total length of the instruction segment */
    int dataLength;         /* Total length of the data segment */
    Symbol *symbolTable;    /* Linked list of symbols */
} InputData;




/* Function declarations */
void createObFile(const char *originFilename, const InputData *inputData);
void createEntFile(const char *originFilename, const Symbol *symbolTable);
void createExtFile(const char *originFilename, const Symbol *symbolTable);
int convertShortToOctal(short binaryValue);

#endif /* FILE_GENERATOR_H */
