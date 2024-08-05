#ifndef MAIN_H
#define MAIN_H

#include "macro_Handling.h"
#include "macro_validation.h"
#include "line_recognizer.h"
#include "directive_processor.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


/* Function Prototypes */
void processFiles(int argc, char *argv[]); /* Processes each file provided in the command-line arguments.*/
void processExpandedFile(const char *filename); /*Processes the expanded file to recognize and handle each sentence type.*/

#endif /* MAIN_H */
