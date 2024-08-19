#ifndef FILE_GENERATOR_H
#define FILE_GENERATOR_H

#include "../utils/struct.h"  /* Include the necessary structure definitions */

/* Function to create the object file (.ob) */
void createObjectFile(const int *code,
                      const int code_size,
                      const int *data,
                      const int data_size,
                      char *origin_name);

/* Function to create the entry symbols file (.ent) */
void createEntryFile(const struct symbol * const items[], const int size_items, char *name_b);

/* Function to create the external references file (.ext) */
void createExternalFile(const struct external *params, const int size_params, char *name_b);

#endif /* FILE_GENERATOR_H */
