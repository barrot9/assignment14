/*
 * This header file declares the interface for the second stage of the assembly process.
 * It provides the function prototype for the secondStage function, which is responsible
 * for processing assembly code after the initial preprocessing stage and the first stage.
 */

#ifndef SECOND_STAGE_H
#define SECOND_STAGE_H

/* Include necessery header files. */
#include "../utils.h"
#include "../line_interpreter.h"

/* SecondStage function prototype. */
int secondStage(struct AssemblyUnit* Unit, FILE* assembly_file, char *file_name );

#endif 
