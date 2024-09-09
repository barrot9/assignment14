/*
 * This header file defines the interface for the first stage of the assembly process.
 */

#ifndef FIRST_STAGE_H
#define FIRST_STAGE_H

/* Included header files */
#include "../line_interpreter.h"
#include "../utils.h"

/* Function prototype */
int firstStage(struct AssemblyUnit* Unit, FILE *assembly_file, char *file_name);

#endif 
