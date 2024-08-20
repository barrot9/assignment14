#ifndef FILE_GENERATOR_H
#define FILE_GENERATOR_H

#include "../utils/struct.h"  

/* fileGenerator.c function prototypes. */
void createObjectFile(const int *code, const int code_size, const int *data, const int data_size, char *origin_name);
void createEntryFile(const struct symbol * const items[], const int size_items, char *name_b);
void createExternalFile(const struct external *params, const int size_params, char *name_b);

#endif 
