#include "main.h"
#include "stages/preProcessor/macro_Handling.h"
#include "stages/lineAnalyzer/line_recognizer.h"
#include "stages/utils/utils.h"
#include "stages/lineAnalyzer/line_validator.h"
#include "stages/utils/struct.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h> /* Include stdlib.h for exit */

/* function declarations (will be moved later) */
static struct SymbolTableManager symbolManager = {0}; 
int firstStage(struct SymbolTableManager* symbolManager, LineInfo* head); 
int secondStage(struct SymbolTableManager* symbolManager, LineInfo* head);
void printGeneratedCode(struct SymbolTableManager* symbolManager);

/*
 * processFiles: Processes each file provided in the command-line arguments.
 */

void processFiles(int argc, char *argv[]) {
    int i;
    char outputFilename[MAX_LINE_LENGTH];
    char filename[MAX_LINE_LENGTH];
    size_t len;
    for (i = 1; i < argc; i++) {
        
        /* Copy the base filename */
        strcpy(filename, argv[i]);
        
        /* Append the .as extension */
        strcat(filename, ".as");

        len = strlen(filename);

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
    int opcode, lineNumber = 1;
    FILE *file = fopen(filename, "r");
    int result;
    int err;

    if (!file) {
        perror("Could not open expanded file");
        return;
    }

    while (fgets(line, sizeof(line), file)) {
        trimmedLine = trimWhitespace(line);
        /* Detect the line type and extract the label if present */
        type = detectLineType(trimmedLine, label);

        if (type == LINE_DIRECTIVE || type == LINE_INSTRUCTION) {
            /* Validate the line before processing it further */
            if (!validateLine(trimmedLine, label, &type, &opcode, lineNumber)) {
                fprintf(stderr, "Error: Invalid line detected in file '%s'. Skipping line.\n", filename);
                lineNumber++;
                continue;
            }
            /* Create a new LineInfo and add it to the list */
            info = createLineInfo(trimmedLine, type, label);
            addLineToList(&head, info);
            printf("Line added to list: %s\n", trimmedLine);  /* Debugging output */
        }
        lineNumber++;
}

    result = firstStage(&symbolManager, head); /* call for the first pass function */
    /* Check for errors */ 
    if (result) {
        printf("Errors occurred during the first pass.\n");
    } else {
        printf("First pass completed successfully.\n");
    }

    err = secondStage(&symbolManager, head); /* call for the second pass function */
    /* Check for errors */
    if (err) {
        printf("Errors encountered during second stage processing.\n");
    } else {
        printf("Second pass completed successfully.\n");
        
        /* Generate the output files */
        
        /* Generate the .ob file (object file) */
        createObjectFile(symbolManager.code, symbolManager.code_size, 
                             symbolManager.data, symbolManager.data_size, 
                             (char *)filename);

        /* Generate the .ent file (entry symbols) */
        if (symbolManager.entries_count > 0) {
            createEntryFile(symbolManager.entries, symbolManager.entries_count, (char *)filename);
        }

        /* Generate the .ext file (external references) */
        if (symbolManager.externals_size > 0) {
            createExternalFile(symbolManager.externals, symbolManager.externals_size, (char *)filename);
        }

        /* Optionally print the generated code for debugging */
        printGeneratedCode(&symbolManager);  
    }


    fclose(file);

    /* Print all lines and their types */
    /*printLines(head); */

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
