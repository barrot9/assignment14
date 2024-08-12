#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file_generator.h"

/* Function to convert a short (15-bit binary number) to octal */
int convertShortToOctal(short binaryValue) {
    int octal = 0;
    int decimal = 0;
    int base = 1;
    int i;
    /* Convert the short binary value to a decimal */
    for (i = 0; i < 15; ++i) {
        decimal += (binaryValue & (1 << i)) ? base : 0;
        base *= 2;
    }

    /* Reset base for octal conversion */
    base = 1;

    /* Convert decimal to octal */
    while (decimal > 0) {
        octal += (decimal % 8) * base;
        decimal /= 8;
        base *= 10;
    }

    return octal;
}

/* Helper function to construct the output filename */
char *constructOutputFilename(const char *originFilename, const char *extension) {
    size_t len = strlen(originFilename);
    char *outputFilename = malloc(len + strlen(extension) + 1);

    if (!outputFilename) {
        perror("Failed to allocate memory for filename");
        exit(EXIT_FAILURE);
    }

    strcpy(outputFilename, originFilename);
    strcat(outputFilename, extension);

    return outputFilename;
}

/* Function to create the .ob file */
void createObFile(const char *originFilename, const InputData *inputData) {
    FILE *file;
    Node *currentNode;
    int octalContent;

    char *obFilename = constructOutputFilename(originFilename, ".ob");

    file = fopen(obFilename, "w");
    if (!file) {
        perror("Failed to open .ob file");
        free(obFilename);
        exit(EXIT_FAILURE);
    }

    /* Write the header with instruction and data lengths */
    fprintf(file, "%d %d\n", inputData->instructionLength, inputData->dataLength);

    /* Iterate over the linked list and write memory image */
    currentNode = inputData->head;
    while (currentNode != NULL) {
        /* Convert 15-bit short binary value to octal */
        octalContent = convertShortToOctal(currentNode->binaryValue);

        /* Write address and octal content to the .ob file */
        fprintf(file, "%04d %05o\n", currentNode->addressIndex, octalContent);

        /* Move to the next node */
        currentNode = currentNode->next;
    }

    fclose(file);
    free(obFilename);
    printf("File '%s' has been created successfully.\n", obFilename);
}

/* Function to create the .ent file */
void createEntFile(const char *originFilename, const Symbol *symbolTable) {
    FILE *file;
    char *entFilename; 
    int i;
    bool hasEntrySymbols = false;
    const Symbol *currentSymbol;

    entFilename = constructOutputFilename(originFilename, ".ent");
    

    file = fopen(entFilename, "w");
    if (!file) {
        perror("Failed to open .ent file");
        free(entFilename);
        exit(EXIT_FAILURE);
    }

    /* Write entry symbols to the file */
    currentSymbol = symbolTable;
    while (currentSymbol != NULL) {
        if (currentSymbol->isEntry) {
            for (i = 0; i < currentSymbol->usageCount; ++i) {
                fprintf(file, "%s %04d\n", currentSymbol->name, currentSymbol->usageAddresses[i]);
            }
            hasEntrySymbols = true;
        }
        currentSymbol = currentSymbol->next;
    }

    fclose(file);
    if (!hasEntrySymbols) {
        remove(entFilename); /* Remove file if no entry symbols */
    }
    free(entFilename);
    printf("File '%s' has been created successfully.\n", entFilename);
}


/* Function to create the .ext file */
void createExtFile(const char *originFilename, const Symbol *symbolTable) {
    FILE *file;
    int i;
    bool hasExternalSymbols = false;
    const Symbol *currentSymbol;
    char *extFilename = constructOutputFilename(originFilename, ".ext");

    file = fopen(extFilename, "w");
    if (!file) {
        perror("Failed to open .ext file");
        free(extFilename);
        exit(EXIT_FAILURE);
    }

    /* Write external references to the file */
    currentSymbol = symbolTable;
    while (currentSymbol != NULL) {
        if (currentSymbol->isExternal) {
            for (i = 0; i < currentSymbol->usageCount; ++i) {
                fprintf(file, "%s %04d\n", currentSymbol->name, currentSymbol->usageAddresses[i]);
            }
            hasExternalSymbols = true;
        }
        currentSymbol = currentSymbol->next;
    }

    fclose(file);
    if (!hasExternalSymbols) {
        remove(extFilename); /* Remove file if no external symbols */
    }
    free(extFilename);
    printf("File '%s' has been created successfully.\n", extFilename);
}

