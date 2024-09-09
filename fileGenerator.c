/*
 * This file contains functions for generating output files in the assembly process.
 * It creates object files, entry files, and external files based on the assembled code.
 * The functions in this file handle the final stage of the assembly process, creating the
 * necessary output files.
 */

#include "fileGenerator.h"

/* Function to create the object file containing the assembled machine code */
int create_object_file(const int *assembly_code, const int assembly_code_size, const int *assembly_data, const int assembly_data_size, char *filename) {
    char* obj_path;
    FILE* object_file;
    int instruction_counter;
    int i;
    const char* stripped_filename;

    /* Strip input file prefix and create the output file path */
    stripped_filename = stripInputFilesPrefix(filename);
    if (!stripped_filename) {
        fprintf(stderr, "Error: Failed to strip input file prefix.\n");
        return ERROR;
    }

    obj_path = getFilePath(OUTPUT_FILE_DIR, stripped_filename, OBJ_FILE_TYPE);
    if (!obj_path) {
        fprintf(stderr, "Error: Failed to create output file path.\n");
        return ERROR;
    }

    object_file = createFile(obj_path);
    if (!object_file) {
        fprintf(stderr, "Error: Failed to create object file.\n");
        free(obj_path);
        return ERROR;
    }

    /* Write the sizes of code and data sections */
    if (fprintf(object_file, "  %d %d\n", assembly_code_size, assembly_data_size) < 0) {
        fprintf(stderr, "Error: Failed to write sizes to object file.\n");
        fclose(object_file);
        free(obj_path);
        return ERROR;
    }

    instruction_counter = INIT_ADDRESS;

    /* Write the code section */
    for (i = 0; i < assembly_code_size; i++, instruction_counter++) {
        if (writeInstruction(object_file, instruction_counter, assembly_code[i]) != 0) {
            fprintf(stderr, "Error: Failed to write instruction to object file.\n");
            fclose(object_file);
            free(obj_path);
            return ERROR;
        }
    }

    /* Write the data section */
    for (i = 0; i < assembly_data_size; i++, instruction_counter++) {
        if (writeInstruction(object_file, instruction_counter, assembly_data[i]) != 0) {
            fprintf(stderr, "Error: Failed to write data to object file.\n");
            fclose(object_file);
            free(obj_path);
            return ERROR;
        }
    }

    fclose(object_file);
    free(obj_path);
    return 0; /* Success */
}

/* Function to create the entry file listing entry symbols and their addresses */
int create_entry_file(const struct symbols_table *const entries[], const int entries_count, char *filename) {
    char* ent_path;
    FILE* entry_file;
    int i;
    const char* stripped_filename;

    /* Strip input file prefix and create the output file path */
    stripped_filename = stripInputFilesPrefix(filename);
    if (!stripped_filename) {
        fprintf(stderr, "Error: Failed to strip input file prefix.\n");
        return ERROR;
    }

    ent_path = getFilePath(OUTPUT_FILE_DIR, stripped_filename, ENT_FILE_TYPE);
    if (!ent_path) {
        fprintf(stderr, "Error: Failed to create entry file path.\n");
        return ERROR;
    }

    entry_file = createFile(ent_path);
    if (!entry_file) {
        fprintf(stderr, "Error: Failed to create entry file.\n");
        free(ent_path);
        return ERROR;
    }

    /* Write each entry symbol and its address */
    for (i = entries_count - 1; i >= 0; i--) {
        if (fprintf(entry_file, "%s:\t%d\n", entries[i]->symbol_name, entries[i]->symbol_address) < 0) {
            fprintf(stderr, "Error: Failed to write entry to file.\n");
            fclose(entry_file);
            free(ent_path);
            return ERROR;
        }
    }

    fclose(entry_file);
    free(ent_path);
    return 0; /* Success */
}

/* Function to create the external file listing external symbols and their references */
int create_external_file(const struct external_symbols_table *externals, const int externals_count, char *filename) {
    char* ext_path;
    FILE* external_file;
    int i;
    int j;
    const char* stripped_filename;

    /* Strip input file prefix and create the output file path */
    stripped_filename = stripInputFilesPrefix(filename);
    if (!stripped_filename) {
        fprintf(stderr, "Error: Failed to strip input file prefix.\n");
        return ERROR;
    }

    ext_path = getFilePath(OUTPUT_FILE_DIR, stripped_filename, EXT_FILE_TYPE);
    if (!ext_path) {
        fprintf(stderr, "Error: Failed to create external file path.\n");
        return ERROR;
    }

    external_file = createFile(ext_path);
    if (!external_file) {
        fprintf(stderr, "Error: Failed to create external file.\n");
        free(ext_path);
        return ERROR;
    }

    /* Write each external symbol and its references */
    for (i = 0; i < externals_count; i++) {
        for (j = 0; j < externals[i].address_count; j++) {
            if (fprintf(external_file, "%s\t%d\n", externals[i].external_symbol_name, externals[i].external_symbol_addresses[j]) < 0) {
                fprintf(stderr, "Error: Failed to write external symbol to file.\n");
                fclose(external_file);
                free(ext_path);
                return ERROR;
            }
        }
    }

    fclose(external_file);
    free(ext_path);
    return 0; /* Success with the right treatment */
}