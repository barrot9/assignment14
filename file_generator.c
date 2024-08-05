#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file_generator.h"

/* Function to convert binary to octal */
int convertBinaryToOctal(int binary) {
    int octal = 0;
    int decimal = 0;
    int base = 1;

    /* Convert binary to decimal */
    while (binary > 0) {
        decimal += (binary % 10) * base;
        binary /= 10;
        base *= 2;
    }

    /* Convert decimal to octal */
    base = 1;
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
        /* Convert binary line to octal */
        octalContent = convertBinaryToOctal(currentNode->binaryLine);

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
void createEntFile(const char *originFilename, const Symbol *symbolTable, int symbolCount) {
    FILE *file;
    int i;

    char *entFilename = constructOutputFilename(originFilename, ".ent");

    file = fopen(entFilename, "w");
    if (!file) {
        perror("Failed to open .ent file");
        free(entFilename);
        exit(EXIT_FAILURE);
    }

    /* Write entry symbols to the file */
    for (i = 0; i < symbolCount; ++i) {
        if (symbolTable[i].isEntry) {
            fprintf(file, "%s %04d\n", symbolTable[i].name, symbolTable[i].address);
        }
    }

    fclose(file);
    free(entFilename);
    printf("File '%s' has been created successfully.\n", entFilename);
}

/* Function to create the .ext file */
void createExtFile(const char *originFilename, const ExternalRef *externalRefs) {
    FILE *file;
    const ExternalRef *currentRef = externalRefs; /* Move declaration to the beginning */

    char *extFilename = constructOutputFilename(originFilename, ".ext");

    file = fopen(extFilename, "w");
    if (!file) {
        perror("Failed to open .ext file");
        free(extFilename);
        exit(EXIT_FAILURE);
    }

    /* Write external references to the file */
    while (currentRef != NULL) {
        fprintf(file, "%s %04d\n", currentRef->name, currentRef->address);
        currentRef = currentRef->next;
    }

    fclose(file);
    free(extFilename);
    printf("File '%s' has been created successfully.\n", extFilename);
}



/*
int main() {
    const char *originFilename = "program";

     Example linked list of binary lines 
    Node node3 = {000010001001010, 0102, NULL};
    Node node2 = {000000011000100, 0101, &node3};
    Node node1 = {001010000010100, 0100, &node2};
    */
    /* InputData inputData = {&node1, 10, 20};  Replace with actual instruction and data lengths 
    */
    /* Example symbol table - replace with actual symbols 
    Symbol symbolTable[] = {
        {"MAIN", 100, 1},
        {"LIST", 137, 1}
    };
    int symbolCount = sizeof(symbolTable) / sizeof(symbolTable[0]); */

    /* Example linked list of external references 
    ExternalRef ref3 = {"fn1", 0104, NULL};
    ExternalRef ref2 = {"L3", 0114, &ref3};
    ExternalRef ref1 = {"L3", 0127, &ref2}; */
    /* Create the .ob file 
    createObFile(originFilename, &inputData); */

    /* Create the .ent file 
    createEntFile(originFilename, symbolTable, symbolCount);

     Create the .ext file 
    createExtFile(originFilename, &ref1);

    return 0; */