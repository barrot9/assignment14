/*
   This file implements functions to generate the output files (.ob, .ent, .ext) 
   for the assembler. The object file (.ob) contains the machine code and data 
   segments, the entry file (.ent) contains entry symbols and their addresses, 
   and the external file (.ext) contains external symbols and their references.

   The three functions in this file are:
   1. createObjectFile: Generates the .ob file with machine code and data.
   2. createEntryFile: Generates the .ent file listing all entry symbols.
   3. createExternalFile: Generates the .ext file listing all external symbols and their references.
*/

#include <stdio.h>
#include <stdlib.h>
#include "../utils/struct.h"

/*
   Function to create the object file (.ob) containing machine code and data sections.

   Parameters:
   - code: Pointer to the array of machine code instructions.
   - code_size: Number of instructions in the code array.
   - data: Pointer to the array of data values.
   - data_size: Number of data items in the data array.
   - origin_name: The base name of the original file (without extension).

   This function appends ".ob" to the origin_name, opens the file, and writes the 
   code and data segments to it. Each line in the file includes an address (starting 
   from 100) and the corresponding code or data value in chunks of 3 bits.
*/
void createObjectFile(const int *code,
                      const int code_size,
                      const int *data,
                      const int data_size,
                      char *origin_name) {
    
    int index_i;
    FILE *obj_file;
    char obj_file_name[256];  /* Adjust the size as needed */
    int ic = 100;  /* Instruction counter starts at 100 */

    /* Create the object file name by appending ".ob" to the original file name */
    snprintf(obj_file_name, sizeof(obj_file_name), "%s.ob", origin_name);

    /* Open the object file for writing */
    obj_file = fopen(obj_file_name, "w");

    /* Check if the file was successfully opened */
    if (obj_file) {
        /* Write the code and data sizes at the beginning of the file */
        fprintf(obj_file, "  %d %d\n", code_size, data_size);

        /* Write the code segment to the object file */
        for (index_i = 0; index_i < code_size; index_i++, ic++) {
            /* Print the address (instruction counter) */
            fprintf(obj_file, "%04d ", ic);
            /* Print the 15-bit code value in chunks of 3 bits each */
            fprintf(obj_file, "%d%d%d%d%d\n",
                    (code[index_i] >> 12) & 0x7,  /* Bits 14-12 */
                    (code[index_i] >> 9)  & 0x7,  /* Bits 11-9 */
                    (code[index_i] >> 6)  & 0x7,  /* Bits 8-6 */
                    (code[index_i] >> 3)  & 0x7,  /* Bits 5-3 */
                    (code[index_i]      ) & 0x7); /* Bits 2-0 */
        }

        /* Write the data segment to the object file */
        for (index_i = 0; index_i < data_size; index_i++, ic++) {
            /* Print the address (instruction counter) */
            fprintf(obj_file, "%04d ", ic);
            /* Print the 15-bit data value in chunks of 3 bits each */
            fprintf(obj_file, "%d%d%d%d%d\n",
                    (data[index_i] >> 12) & 0x7,  /* Bits 14-12 */
                    (data[index_i] >> 9)  & 0x7,  /* Bits 11-9 */
                    (data[index_i] >> 6)  & 0x7,  /* Bits 8-6 */
                    (data[index_i] >> 3)  & 0x7,  /* Bits 5-3 */
                    (data[index_i]      ) & 0x7); /* Bits 2-0 */
        }

        /* Close the object file after writing */
        fclose(obj_file);
    }
}

/*
   Function to create the entry symbols file (.ent) containing entry labels and their addresses.

   Parameters:
   - items: Array of pointers to symbols (entry symbols).
   - size_items: Number of symbols in the items array.
   - name_b: The base name of the original file (without extension).

   This function appends ".ent" to the name_b, opens the file, and writes each entry 
   symbol's name and address to it. The symbols are listed one per line.
*/
void createEntryFile(const struct symbol * const items[], const int size_items, char *name_b) {
    char file_ent_name[256];
    FILE *file_ent;
    int index_i;

    /* Create the entry file name by appending ".ent" to the original file name */
    snprintf(file_ent_name, sizeof(file_ent_name), "%s.ent", name_b);

    /* Open the entry file for writing */
    file_ent = fopen(file_ent_name, "w");

    /* Check if the file was successfully opened */
    if (file_ent) {
        /* Write each entry symbol and its address to the entry file */
        for (index_i = 0; index_i < size_items; index_i++) {
            fprintf(file_ent, "%s\t%04d\n", items[index_i]->name, items[index_i]->addr);
        }
        /* Close the entry file after writing */
        fclose(file_ent);
    }
}

/*
   Function to create the external references file (.ext) containing external symbols and their references.

   Parameters:
   - params: Array of external symbols.
   - size_params: Number of external symbols in the params array.
   - name_b: The base name of the original file (without extension).

   This function appends ".ext" to the name_b, opens the file, and writes each external 
   symbol's name and its reference addresses to the file. Multiple references are listed 
   for each symbol if applicable.
*/
void createExternalFile(const struct external *params, const int size_params, char *name_b) {
    char file_ext_name[256];
    FILE *file_ext;
    int index_i;
    int index_j;

    /* Create the external file name by appending ".ext" to the original file name */
    snprintf(file_ext_name, sizeof(file_ext_name), "%s.ext", name_b);

    /* Open the external file for writing */
    file_ext = fopen(file_ext_name, "w");

    /* Check if the file was successfully opened */
    if (file_ext) {
        /* Write each external symbol and its references to the external file */
        for (index_i = 0; index_i < size_params; index_i++) {
            for (index_j = 0; index_j < params[index_i].addr_c; index_j++) {
                fprintf(file_ext, "%s\t%d\n", params[index_i].ext_name, params[index_i].addr[index_j]);
            }
        }
        /* Close the external file after writing */
        fclose(file_ext);
    }
}
