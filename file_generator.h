#ifndef FILE_GENERATOR_H
#define FILE_GENERATOR_H

/* Node structure for the linked list */
typedef struct Node {
    int binaryLine;          /* Binary representation of the assembly line */
    int addressIndex;        /* Memory address index */
    struct Node *next;       /* Pointer to the next node */
} Node;

/* Input structure containing the linked list and other relevant data */
typedef struct {
    Node *head;              /* Head of the linked list */
    int instructionLength;   /* Total length of the instruction segment */
    int dataLength;          /* Total length of the data segment */
} InputData;

/* Symbol structure to store symbol details */
typedef struct {
    char name[50];           /* Symbol name */
    int address;             /* Address in memory */
    int isEntry;             /* Flag to indicate if the symbol is an entry */
} Symbol;

/* External reference structure for .ext file */
typedef struct ExternalRef {
    char name[50];           /* Symbol name */
    int address;             /* Address where the symbol is used */
    struct ExternalRef *next; /* Pointer to the next external reference */
} ExternalRef;

/* Function declarations */
void createObFile(const char *originFilename, const InputData *inputData);
void createEntFile(const char *originFilename, const Symbol *symbolTable, int symbolCount);
void createExtFile(const char *originFilename, const ExternalRef *externalRefs);
int convertBinaryToOctal(int binary);

#endif /* FILE_GENERATOR_H */
