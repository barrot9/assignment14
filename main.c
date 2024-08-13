#include "main.h"
#include "stages/preProcessor/macro_Handling.h"
#include "line_recognizer.h"
#include "stages/utils/utils.h"
#include "line_validator.h"
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

        /* Process the expanded file to recognize and handle sentence types */
        processExpandedFile(outputFilename);
    }
}

/*
 * processExpandedFile: Processes the expanded file to recognize and handle each sentence type.
 */
void processExpandedFile(const char *filename) {
    char line[MAX_LINE_LENGTH + 2]; /* +2 to handle \n and \0 */
    char label[MAX_LABEL_LENGTH + 1];
    char *trimmedLine;
    LineInfo *head = NULL, *info, *current;
    LineType type;
    int opcode;
    FILE *file = fopen(filename, "r");

    if (!file) {
        perror("Could not open expanded file");
        return;
    }

    while (fgets(line, sizeof(line), file)) {

    trimmedLine = trimWhitespace(line);

    /* Validate the line before processing it further */
    if (!validateLine(trimmedLine, label, &type, &opcode)) {
        fprintf(stderr, "Error: Invalid line detected in file '%s'. Skipping line.\n", filename);
        continue;
    }
    printf("Line passed validation: %s\n", trimmedLine);  /* Debugging output */

    /* Detect the line type and extract the label if present */
    type = detectLineType(trimmedLine, label);

    /* Create a new LineInfo and add it to the list */
    info = createLineInfo(trimmedLine, type, label);
    
    addLineToList(&head, info);
    printf("Line added to list: %s\n", trimmedLine);  /* Debugging output */

    printf("Looping back to read next line.\n");  /* Debugging output */
}


    fclose(file);

    /* Print all lines and their types */
    printLines(head);

    /* Free the linked list */
    current = head;
    while (current) {
        LineInfo *next = current->next;
        free(current);
        current = next;
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
