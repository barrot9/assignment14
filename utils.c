/*
 * This file contains utility functions and data structures used throughout
 * the assembly process, including the file process, 
 * symbol table management, file handling, and instruction writing.
 */

#include "utils.h"

/* Global structures for processing data */
static struct AssemblyUnit AssemblyUnit = {0};
extern struct analized_line current_line;

/* Function to process a single assembly file */
int process_file(char *filename) {
    FILE *input_file = NULL;
    char *preprocessed_filename = NULL;
    int result = 0;

    if (filename == NULL) {
        fprintf(stderr, "Error: Null filename provided\n");
        return ERROR;
    }

    preprocessed_filename = preProcessor(filename);
    if (preprocessed_filename == NULL) {
        fprintf(stderr, "Error: Failed to preprocess file %s\n", filename);
        return ERROR;
    }

    input_file = fopen(preprocessed_filename, "r");
    if (input_file == NULL) {
        fprintf(stderr, "Error: Unable to open file %s\n", preprocessed_filename);
        free(preprocessed_filename);
        return ERROR;
    }

    /* Reset the global structure before processing */
    memset(&AssemblyUnit, 0, sizeof(AssemblyUnit));

    /* Run first and second stages processing */
    if (firstStage(&AssemblyUnit, input_file, preprocessed_filename) != 0) {
        fprintf(stderr, "Error: First stage processing failed for %s\n", filename);
        result = ERROR;
    } else if (secondStage(&AssemblyUnit, input_file, filename) != 0) {
        fprintf(stderr, "Error: Second stage processing failed for %s\n", filename);
        result = ERROR;
    } else {
        /* Create output files based on assembly results */
        if (AssemblyUnit.code_size > 0 || AssemblyUnit.data_size > 0) {
            if (create_object_file(AssemblyUnit.code, AssemblyUnit.code_size, AssemblyUnit.data, AssemblyUnit.data_size, filename) != 0) {
                fprintf(stderr, "Error: Failed to create object file for %s\n", filename);
                result = ERROR;
            }
        }

        if (result == 0 && AssemblyUnit.entries_count > 0) {
            if (create_entry_file(AssemblyUnit.entries, AssemblyUnit.entries_count, filename) != 0) {
                fprintf(stderr, "Error: Failed to create entry file for %s\n", filename);
                result = ERROR;
            }
        }

        if (result == 0 && AssemblyUnit.externals_size > 0) {
            if (create_external_file(AssemblyUnit.externals, AssemblyUnit.externals_size, filename) != 0) {
                fprintf(stderr, "Error: Failed to create external file for %s\n", filename);
                result = ERROR;
            }
        }
    }

    fclose(input_file);
    free(preprocessed_filename);

    return result;
}

/* Symbol table management functions */

/* Function to add a symbol to the symbol table */
void add_symbol(struct AssemblyUnit *unit, char *symbol_name, enum Symbol type, 
                int address, int line_number, int const_value, int data_size) {
    struct symbols_table *current_symbol = &unit->symbols[unit->symbols_size];
    strcpy(current_symbol->symbol_name, symbol_name);
    current_symbol->symbol_type = type;
    current_symbol->symbol_address = address;
    current_symbol->line_number = line_number;
    current_symbol->constant_value = const_value;
    current_symbol->data_size = data_size;
    unit->symbols_size++;
}

/* Function to update an existing symbol in the symbol table */
void update_symbol(struct symbols_table *symbol, int line_counter, int address, enum Symbol type) {
    symbol->line_number = line_counter;
    symbol->symbol_address = address;
    symbol->symbol_type = type;
}

/* Function to update a symbol as an entry symbol */
void update_entry_symbol(struct symbols_table *symbol, const char *file_name, int line_counter) {
    if (symbol->symbol_type == Symbol_code) {
        symbol->symbol_type = entry_symbol_code;
    } else if (symbol->symbol_type == Symbol_data) {
        symbol->symbol_type = entry_symbol_data;
    } else { 
        printf("%s:%d: Symbol already exists: '%s'\n", file_name, line_counter, symbol->symbol_name); 
    }
}

/* Function to adjust symbol addresses after code generation */
void adjust_symbol_address(struct symbols_table *symbol, int instruction_counter) {
    if (symbol->symbol_type == Symbol_data || symbol->symbol_type == entry_symbol_data) {
        symbol->symbol_address += instruction_counter;
    }
}

/* Function to add entry symbols to the entries list */
void add_to_entries(struct AssemblyUnit *Unit, struct symbols_table *symbol) {
    if (symbol->symbol_type == entry_symbol_code || symbol->symbol_type == entry_symbol_data) {
        Unit->entries[Unit->entries_count] = symbol;
        Unit->entries_count++;
    }
}

/* Function to search for a symbol in the symbol table */
struct symbols_table *search_symbol(struct AssemblyUnit *unit, char *symbol_name) {
    int i;
    for (i = 0; i < unit->symbols_size; i++) {
        if (strcmp(unit->symbols[i].symbol_name, symbol_name) == 0) {
            return &unit->symbols[i];
        }
    }
    return NULL;
}

/* Function to search for an external symbol in the external symbols table */
struct external_symbols_table *search_external_symbol(struct AssemblyUnit *unit, char *external_name) {
    int i;
    for (i = 0; i < unit->externals_size; i++) {
        if (strcmp(external_name, unit->externals[i].external_symbol_name) == 0) {
            return &unit->externals[i];
        }
    }
    return NULL;
}

/* File handling functions */

/* Function to strip input file prefixes from filename */
const char* stripInputFilesPrefix(const char* filename) {
    const char* stripped = strstr(filename, INPUT_FILES_PREFIX_1);
    if (stripped != NULL) {
        return stripped + strlen(INPUT_FILES_PREFIX_1);
    }
    stripped = strstr(filename, INPUT_FILES_PREFIX_2);
    if (stripped != NULL) {
        return stripped + strlen(INPUT_FILES_PREFIX_2);
    }
    return filename;
}

/* Function to generate a file path for output files */
char* getFilePath(const char* dir, const char* filename, const char* extension) {
    char* filepath;
    
    filepath = malloc(MAX_PATH_LENGTH);
    if (filepath == NULL) {
        fprintf(stderr, "[ERROR] Out of memory, aborting.\n");
        exit(1);
    }
    snprintf(filepath, MAX_PATH_LENGTH, "%s/%s%s", dir, filename, extension);
    return filepath;
}

/* Function to create a new file for writing */
FILE* createFile(const char* filepath) {
    FILE* file;
    
    file = fopen(filepath, "w");
    if (file == NULL) {
        fprintf(stderr, "[ERROR] Unable to create file: %s\n", filepath);
    }
    return file;
}

/* Function to write an instruction to the output file */
int writeInstruction(FILE* file, int address, int instruction) {
    if (!file) {
        fprintf(stderr, "Error: Null file pointer passed to writeInstruction\n");
        return ERROR;
    }

    if (fprintf(file, "%04d %d%d%d%d%d\n",
                address,
                (instruction >> 12) & 0x7,
                (instruction >> 9) & 0x7,
                (instruction >> 6) & 0x7,
                (instruction >> 3) & 0x7,
                instruction & 0x7) < 0) {
        fprintf(stderr, "Error: Failed to write instruction to file\n");
        return ERROR;
    }

    return 0;
}