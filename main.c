#include "main.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h> /* Include stdlib.h for exit */

/*
 * processFiles: Processes each file provided in the command-line arguments.
 */
void processFiles(int argc, char *argv[]) {
    int i;
    char outputFilename[MAX_LINE_LENGTH];
    for (i = 1; i < argc; i++) {
        char *filename = argv[i];
        size_t len = strlen(filename);

        if (len < 3 || strcmp(filename + len - 3, ".as") != 0) {
            fprintf(stderr, "Error: Invalid file type for '%s'. Expected a '.as' file.\n", filename);
            continue;
        }

        strncpy(outputFilename, filename, len - 3);
        outputFilename[len - 3] = '\0';
        strcat(outputFilename, ".am");

        printf("Processing file: %s\n", filename);

        /* Validate macros first */
        if (validateMacros(filename) != 0) {
            fprintf(stderr, "Error validating macros in file '%s'. Skipping file.\n", filename);
            continue;
        }

        /* If validation passes, process the file */
        processFile(filename);

        /* Rename output file to match the input file with .am extension */
        rename("output.asm", outputFilename);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename1.as> <filename2.as> ...\n", argv[0]);
        return 1;
    }

    processFiles(argc, argv);

    return 0;
}
