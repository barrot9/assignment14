/*
 * This file implements the preprocessor for our assembly language.
 * It handles macro definitions and expansions in the input file, 
 * and generating an output file.
 * it prepares the assembly code for the next stages.
 */

#include "pre_processor.h"

/* Function to process the input file and generate a macro-expanded output file */
char* preProcessor(const char* inputFilename) {
    char fileBuffer[MAX_LENGTH] = {0};
    char *commentMarker, *quoteStartPtr, *quoteEndPtr;
    char *sourceFileName, *macroFileName;
    int lineCounter = 1;
    FILE *sourceFile, *macroFile;
    MacroTableDef macroTable = {0};
    MacroDef* activeMacro = NULL;
    int i;

    /* Allocate memory for source and macro filenames */
    sourceFileName = (char *)malloc(strlen(inputFilename) + 4);
    if (sourceFileName == NULL) {
        printf("Memory allocation failed for assembly file name.\n");
        return NULL;
    }
    strcpy(sourceFileName, inputFilename);
    strcat(sourceFileName, INPUT_FILE_EXT);

    macroFileName = (char *)malloc(strlen(inputFilename) + 4); 
    if (macroFileName == NULL) {
        printf("Memory allocation failed for extracted file name.\n");
        free(sourceFileName);
        return NULL;
    }
    strcpy(macroFileName, inputFilename);
    strcat(macroFileName, UNPACKED_FILE_EXT);

    /* Open source and macro files */
    sourceFile = fopen(sourceFileName, MODE_READ);
    macroFile = fopen(macroFileName, MODE_WRITE);
    
    /* Check if file opening was successful */
    if (!sourceFile || !macroFile) {
        printf("Failed to open files: %s or %s.\n", sourceFileName, macroFileName);
        free(macroFileName);
        free(sourceFileName);
        return NULL;
    }
    
    /* Read each line from the source file */
    while (fgets(fileBuffer, sizeof(fileBuffer), sourceFile)) {
        LineCategory lineType = categorize_line(fileBuffer, &macroTable, &activeMacro);
        commentMarker = strchr(fileBuffer, COMMENT_PREFIX);
        if (commentMarker) {
            quoteStartPtr = strchr(fileBuffer, DOUBLE_QUOTE);
            if (quoteStartPtr) {
                quoteEndPtr = strrchr(fileBuffer, DOUBLE_QUOTE);
                /* Check if comment is outside the quoted text */
                if (commentMarker > quoteEndPtr || commentMarker < quoteStartPtr) {
                    *commentMarker = '\0';  /* Null-terminate the line at comment start */
                }
            } else {
                *commentMarker = '\0';  /* Null-terminate the line at comment start */
            }
        }

        if (lineType == DEFINE_MACRO) {
            /* Macro definition start */
        }
        else if (lineType == ERROR_NO_NAME) {
            /* Error: missing macro name */
            printf("Line %d: No macro name specified after 'macr'.\n", lineCounter);
        }
        else if (lineType == ERROR_ALREADY_DEFINED) {
            /* Error: macro already defined */
            printf("Line %d: Macro '%s' already defined.\n", lineCounter, activeMacro->macroName);
            activeMacro = NULL;  /* Reset active macro */
        }
        else if (lineType == END_MACRO) {
            /* End of macro definition */
            activeMacro = NULL;  /* Reset active macro */
        }
        else if (lineType == CALL_MACRO) {
            /* Macro invocation */
            for (i = 0; i < activeMacro->lineTotal; i++) {
                fputs(activeMacro->macroContent[i], macroFile);
            }
            activeMacro = NULL;  /* Reset active macro */
        }
        else if (lineType == NORMAL_LINE) {
            /* Regular line handling */
            if (activeMacro) {
                /* Store line in macro content if within a macro */
                strcpy(activeMacro->macroContent[activeMacro->lineTotal], fileBuffer);
                activeMacro->lineTotal++;
            } else {
                /* Write line directly to output file */
                fputs(fileBuffer, macroFile);
            }
        }
        else if (lineType == BLANK_LINE) {
            /* Blank line handling */
        }
        lineCounter++;  /* Increment line counter */
    }
    
    /* Close files and free allocated memory */
    fclose(macroFile);
    fclose(sourceFile);
    free(sourceFileName);
    return macroFileName;
}

/* Function to locate a macro by name within a macro table */
MacroDef* locate_macro(const MacroTableDef* macroTable, const char* macroName) {
    int index;
    for (index = 0; index < macroTable->macroCount; index++) {
        /* Compare macro names to find a match */
        if (strcmp(macroTable->macroList[index].macroName, macroName) == 0) {
            return (MacroDef*)&macroTable->macroList[index];
        }
    }
    /* Return NULL if no macro is found */
    return NULL;
}

/* Function to categorize line type and process macros */
LineCategory categorize_line(char* inputLine, MacroTableDef* macroTable, MacroDef** foundMacro) {
    char tempLine[MAX_LENGTH] = {0};
    char *spacePtr = NULL, *endPtr = NULL;

    /* Skip leading whitespace */
    while (isspace(*inputLine)) {
        inputLine++;
    }

    /* Copy input line to a temporary buffer for processing */
    strcpy(tempLine, inputLine);
    inputLine = tempLine;
    /* Remove newline characters from the line */
    inputLine[strcspn(inputLine, "\r\n")] = '\0';
    
    if (*inputLine == '\0') {
        return BLANK_LINE;  /* Return if line is empty */
    }
    
    /* Check for macro end definition */
    if (strncmp(inputLine, "endmacr", 7) == 0) {
        return END_MACRO;
    }
    
    /* Check for macro definition start */
    if (strncmp(inputLine, "macr", 4) == 0) {
        inputLine += 4;  /* Move pointer past 'macr' keyword */
        
        /* Skip whitespace after 'macr' */
        while (isspace(*inputLine)) {
            inputLine++;
        }

        if (*inputLine == '\0') {
            return ERROR_NO_NAME;  /* Return error if macro name is missing */
        }
        
        /* Find the first boundary to separate macro name */
        endPtr = strpbrk(inputLine, WHITESPACE);
        if (endPtr) {
            *endPtr = '\0';  /* Null-terminate at the first boundary */
        }
        
        *foundMacro = locate_macro(macroTable, inputLine);
        if (*foundMacro) {
            return ERROR_ALREADY_DEFINED;  /* Return error if macro is already defined */
        } else {
            /* Add new macro to the table */
            strcpy(macroTable->macroList[macroTable->macroCount].macroName, inputLine);
            *foundMacro = &macroTable->macroList[macroTable->macroCount];
            macroTable->macroCount++;
            return DEFINE_MACRO;  /* Return macro define type */
        }
    }
    
    /* Find the first boundary in the line */
    endPtr = spacePtr = strpbrk(inputLine, WHITESPACE);
    if (spacePtr) {
        while (isspace(*spacePtr)) {
            spacePtr++;
        }
        if (*spacePtr == '\0') {
            *endPtr = '\0';
        } else {
            return NORMAL_LINE;
        }
    }
    
    /* Check for macro */
    *foundMacro = locate_macro(macroTable, inputLine);
    if (*foundMacro) {
        return CALL_MACRO;  /* Return if macro is found */
    }
    
    return NORMAL_LINE;  /* Default to normal line if no conditions are met */
}
