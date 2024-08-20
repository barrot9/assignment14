#ifndef MAIN_H
#define MAIN_H

#include "stages/preProcessor/macro_Handling.h"
#include "stages/preProcessor/macro_validation.h"
#include "stages/lineAnalyzer/line_recognizer.h"
#include "stages/utils/utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


/* Function Prototypes */
static struct SymbolTableManager symbolManager = {0}; 
int firstStage(struct SymbolTableManager* symbolManager, LineInfo* head); 
int secondStage(struct SymbolTableManager* symbolManager, LineInfo* head);
void printGeneratedCode(struct SymbolTableManager* symbolManager);
void processFiles(int argc, char *argv[]); /* Processes each file provided in the command-line arguments.*/
void processExpandedFile(const char *filename); /*Processes the expanded file to recognize and handle each sentence type.*/

#endif /* MAIN_H */
