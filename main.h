/*
 * This header file provides the main interface for the assembler program.
 */

#ifndef MAIN_H
#define MAIN_H

<<<<<<< HEAD
/* Included necessary library */
=======
#include "stages/preProcessor/macro_Handling.h"
#include "stages/preProcessor/macro_validation.h"
#include "stages/lineAnalyzer/line_recognizer.h"
#include "stages/utils/utils.h"
#include "stages/utils/struct.h"
>>>>>>> 389b960335faa1dd26beee952d443d9d0b53411c
#include <stdio.h>

/* Function prototype */
int process_file(char *filename);

<<<<<<< HEAD
#endif 
=======
static struct SymbolTableManager symbolManager = {0}; 

/* Function Prototypes */

int firstStage(struct SymbolTableManager* symbolManager, LineInfo* head); 
int secondStage(struct SymbolTableManager* symbolManager, LineInfo* head);
void printGeneratedCode(struct SymbolTableManager* symbolManager);
void processFiles(int argc, char *argv[]); /* Processes each file provided in the command-line arguments.*/
void processExpandedFile(const char *filename); /*Processes the expanded file to recognize and handle each sentence type.*/

#endif /* MAIN_H */
>>>>>>> 389b960335faa1dd26beee952d443d9d0b53411c
