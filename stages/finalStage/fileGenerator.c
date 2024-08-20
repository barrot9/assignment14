/*
   This module is responsible for generating the output files (.ob, .ent, .ext) 
   created by the assembler. The object file (.ob) contains the machine code and 
   data sections, the entry file (.ent) lists entry symbols and their addresses, 
   and the external file (.ext) tracks external symbols and their references.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fileGenerator.h"

/*
   Generates the object file (.ob) containing machine code and data segments.
 */
void createObjectFile(const int *instructions,
                      const int num_instructions,
                      const int *data_section,
                      const int num_data_items,
                      char *base_filename) {

    FILE *object_file;
    char *object_filename;
    int address_counter = 100;  /* Address starts from 100 */
    int i;

    /* Allocate memory for the object file name */
    object_filename = (char *)malloc(strlen("output_files/") + strlen(base_filename) + 4); /* +4 for ".ob" and null terminator */
    if (!object_filename) {
        perror("Failed to allocate memory for object file name");
        return;
    }

    /* Construct the object file name */
    strcpy(object_filename, "output_files/");
    strcat(object_filename, base_filename);
    strcat(object_filename, ".ob");

    /* Open the object file */
    object_file = fopen(object_filename, "w");
    
    if (object_file) {
        /* Write header with sizes */
        fprintf(object_file, "  %d %d\n", num_instructions, num_data_items);

        /* Write the instruction segment */
        for (i = 0; i < num_instructions; i++, address_counter++) {
            fprintf(object_file, "%04d ", address_counter);
            fprintf(object_file, "%d%d%d%d%d\n",
                    (instructions[i] >> 12) & 0x7,
                    (instructions[i] >> 9)  & 0x7,
                    (instructions[i] >> 6)  & 0x7,
                    (instructions[i] >> 3)  & 0x7,
                    (instructions[i]      ) & 0x7);
        }

        /* Write the data segment */
        for (i = 0; i < num_data_items; i++, address_counter++) {
            fprintf(object_file, "%04d ", address_counter);
            fprintf(object_file, "%d%d%d%d%d\n",
                    (data_section[i] >> 12) & 0x7,
                    (data_section[i] >> 9)  & 0x7,
                    (data_section[i] >> 6)  & 0x7,
                    (data_section[i] >> 3)  & 0x7,
                    (data_section[i]      ) & 0x7);
        }

        /* Close the object file */
        fclose(object_file);
    }

    /* Free the allocated memory */
    free(object_filename);
}

/*
   Creates the entry symbols file (.ent), listing entry symbols and their addresses.
 */
void createEntryFile(const struct symbol * const entry_symbols[], 
                     const int num_entries, 
                     char *base_filename) {
    FILE *entry_file;
    char *entry_filename;
    int i;

    /* Allocate memory for the entry file name */
    entry_filename = (char *)malloc(strlen("output_files/") + strlen(base_filename) + 5); /* +5 for ".ent" and null terminator */
    if (!entry_filename) {
        perror("Failed to allocate memory for entry file name");
        return;
    }

    /* Construct the entry file name */
    strcpy(entry_filename, "output_files/");
    strcat(entry_filename, base_filename);
    strcat(entry_filename, ".ent");

    /* Open the entry file */
    entry_file = fopen(entry_filename, "w");

    if (entry_file) {
        /* Write each entry symbol and its address */
        for (i = 0; i < num_entries; i++) {
            fprintf(entry_file, "%s\t%04d\n", entry_symbols[i]->name, entry_symbols[i]->addr);
        }

        /* Close the entry file */
        fclose(entry_file);
    }

    /* Free the allocated memory */
    free(entry_filename);
}

/*
   Generates the external references file (.ext), listing external symbols and their references.
 */
void createExternalFile(const struct external *externals, 
                        const int num_externals, 
                        char *base_filename) {
    FILE *external_file;
    char *external_filename;
    int i, j;

    /* Allocate memory for the external file name */
    external_filename = (char *)malloc(strlen("output_files/") + strlen(base_filename) + 5); /* +5 for ".ext" and null terminator */
    if (!external_filename) {
        perror("Failed to allocate memory for external file name");
        return;
    }

    /* Construct the external file name */
    strcpy(external_filename, "output_files/");
    strcat(external_filename, base_filename);
    strcat(external_filename, ".ext");

    /* Open the external file */
    external_file = fopen(external_filename, "w");

    if (external_file) {
        /* Write each external symbol and its references */
        for (i = 0; i < num_externals; i++) {
            for (j = 0; j < externals[i].addr_c; j++) {
                fprintf(external_file, "%s\t%d\n", externals[i].ext_name, externals[i].addr[j]);
            }
        }

        /* Close the external file */
        fclose(external_file);
    }

    /* Free the allocated memory */
    free(external_filename);
}
