#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file_generator.h"

#define MAX_LINE_LENGTH 100 /* Maximum length for a line of assembly code */

/* Function to translate a line of assembly code to a binary representation */
int translateToBinary(const char *assemblyLine) {
    int binary = 0;
    
    /* Translation logic goes here */
    /* For demonstration, let's assume each character translates to its ASCII value */
    /* In a real assembler, you would parse the instruction and operands and generate the machine code */
    for (int i = 0; assemblyLine[i] != '\0'; i++) {
        binary = (binary << 1) ^ assemblyLine[i]; /* Simple transformation for demonstration */
    }

    return binary;
}

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

/* Function to write the memory image to the .ob file */
void writeMemoryImage(FILE *file, const char **assemblyLines, int lineCount) {
    int address = 0; /* Starting address */
    int binaryContent;
    int octalContent;

    for (int i = 0; i < lineCount; ++i) {
        /* Translate the line to binary */
        binaryContent = translateToBinary(assemblyLines[i]);

        /* Convert binary to octal */
        octalContent = convertBinaryToOctal(binaryContent);

        /* Write to the .ob file with address and octal content */
        fprintf(file, "%04d %05o\n", address, octalContent);

        /* Increment address (assume each instruction occupies one word) */
        address++;
    }
}

/* Function to create the .ob file */
void createObFile(const char *filename, int instructionLength, int dataLength, const char **assemblyLines, int lineCount) {
    FILE *file;

    file = fopen(filename, "w");
    if (!file) {
        perror("Failed to open .ob file");
        exit(EXIT_FAILURE);
    }

    /* Write the header */
    writeHeader(file, instructionLength, dataLength);

    /* Write memory image */
    writeMemoryImage(file, assemblyLines, lineCount);

    fclose(file);
    printf("File '%s' has been created successfully.\n", filename);
}

/* Function to write the header */
void writeHeader(FILE *file, int instructionLength, int dataLength) {
    fprintf(file, "%d %d\n", instructionLength, dataLength);
}

/* Function to create the .ent file */
void createEntFile(const char *filename, const Symbol *symbolTable, int symbolCount) {
    FILE *file;

    file = fopen(filename, "w");
    if (!file) {
        perror("Failed to open .ent file");
        exit(EXIT_FAILURE);
    }

    /* Write entry symbols to the file */
    for (int i = 0; i < symbolCount; ++i) {
        if (symbolTable[i].isEntry) {
            fprintf(file, "%s %04d\n", symbolTable[i].name, symbolTable[i].address);
        }
    }

    fclose(file);
    printf("File '%s' has been created successfully.\n", filename);
}

/* Function to create the .ext file */
void createExtFile(const char *filename, const ExternalRef *externalRefs, int refCount) {
    FILE *file;

    file = fopen(filename, "w");
    if (!file) {
        perror("Failed to open .ext file");
        exit(EXIT_FAILURE);
    }

    /* Write external references to the file */
    for (int i = 0; i < refCount; ++i) {
        fprintf(file, "%s %04d\n", externalRefs[i].name, externalRefs[i].address);
    }

    fclose(file);
    printf("File '%s' has been created successfully.\n", filename);
}
