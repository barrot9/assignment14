#ifndef FILE_GENERATOR_H
#define FILE_GENERATOR_H

/* Symbol structure to store symbol details */
typedef struct {
    char name[50];   /* Symbol name */
    int address;     /* Address in memory */
    int isEntry;     /* Flag to indicate if the symbol is an entry */
} Symbol;

/* External reference structure for .ext file */
typedef struct {
    char name[50];   /* Symbol name */
    int address;     /* Address where the symbol is used */
} ExternalRef;

/* Function declarations */
void writeHeader(FILE *file, int instructionLength, int dataLength);
void writeMemoryImage(FILE *file, const char **assemblyLines, int lineCount);
int translateToBinary(const char *assemblyLine);
int convertBinaryToOctal(int binary);
void createObFile(const char *filename, int instructionLength, int dataLength, const char **assemblyLines, int lineCount);
void createEntFile(const char *filename, const Symbol *symbolTable, int symbolCount);
void createExtFile(const char *filename, const ExternalRef *externalRefs, int refCount);

#endif /* FILE_GENERATOR_H */
