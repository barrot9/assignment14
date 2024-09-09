/*
 * This file implements the first stage of the assembly process for our assembly language.
 * This first stage focuses on building the symbol table, identifying and categorizing
 * symbols, and preparing for the second stage of the assembly.
 */

/* Include necessary header file */
#include "firstStage.h"

/* Function to perform the first stage of processing on an assembly file */
int firstStage(struct AssemblyUnit* Unit, FILE *assembly_file, char *file_name) {
    /* pointers and variables declarations */
    extern struct analized_line current_line; 
    struct symbols_table *current_symbol; 
    char line[MAX_LENGTH] = {0}; 
    int line_counter = 1; 
    int instruction_counter = INIT_ADDRESS; 
    int data_counter = 0; 
    int error = 0; 
    int i; 

    /* Read each line from the assembly file */
    for (; fgets(line, sizeof(line), assembly_file); line_counter++) {
        analyze_assembly_line(line); /* Analyze the current line */
        
        /* Check if there was an error analyzing the line */
        if (current_line.error[0] != '\0') {
            error = 1;
            printf("At file:%s in line %d, Analyze interupted by: %s\n", file_name, line_counter, current_line.error);
            continue; /* Skip to the next line if an error occurred */
        }

        /* Process label definitions for code or data lines */
        if (current_line.label_name && ((current_line.line_type == directive_line && current_line.directive_type <= directive_data) || current_line.line_type == code_line)) {
            current_symbol = search_symbol(Unit, current_line.label_name); /* Search for the symbol in the symbol table */
            
            if (current_symbol) {
                /* If the symbol is a temporary entry, update its details */
                if (current_symbol->symbol_type == temp_entry_symbol) {
                    if (current_line.line_type == code_line) {
                        update_symbol(current_symbol, line_counter, instruction_counter, entry_symbol_code);
                    } else {
                        update_symbol(current_symbol, line_counter, data_counter, entry_symbol_data);
                    }
                    } else {
                        error = 1;  
                        printf("%s:%d: Symbol already exists: '%s'\n", file_name, line_counter, current_symbol->symbol_name);
                    }
            } else {
                /* Add new symbol to the symbol table */
                if (current_line.line_type == code_line) {
                    add_symbol(Unit, current_line.label_name, Symbol_code, instruction_counter, line_counter, 0, 0);
                } else {
                    /* Check if the directive is for data and handle accordingly */
                    if (current_line.directive_type == directive_data) {
                        add_symbol(Unit, current_line.label_name, Symbol_data, data_counter, line_counter, 0, current_line.data_size);
                    } else {
                        add_symbol(Unit, current_line.label_name, Symbol_data, data_counter, line_counter, 0, strlen(current_line.directive_string));
                    }
                }
            }
        }

        /* Process code lines and update instruction counter */
        if (current_line.line_type == code_line) {
            instruction_counter++; /* Increment instruction counter for the operation code */
            
            /* Check if both operands are registers, if so increment counter */
            if ((current_line.operand_type[0] == direct_register || current_line.operand_type[0] == indirect_register) &&
                (current_line.operand_type[1] == direct_register || current_line.operand_type[1] == indirect_register)) {
                instruction_counter++;
            } else {
                /* Increment instruction counter for each operand */
                for (i = 0; i < 2; i++) {
                    if (current_line.operand_type[i] == immediate || 
                        current_line.operand_type[i] == direct_register || 
                        current_line.operand_type[i] == indirect_register || 
                        current_line.operand_type[i] == label) {
                        instruction_counter++;
                    }
                }
            }
        } else if (current_line.line_type == directive_line && current_line.directive_type <= directive_data) {
            /* Process data directives and update data counter */
            if (current_line.directive_type == directive_data) {
                data_counter += current_line.data_size;
            } else {
                data_counter += strlen(current_line.directive_string) + 1;
            }
        } else if (current_line.line_type == directive_line && current_line.directive_type > directive_data) {
            /* Process entry and external directives */
            current_symbol = search_symbol(Unit, current_line.directive_label); /* Search for the symbol in the symbol table */
            
            if (current_symbol) {
                /* Handle entry directive for existing symbols */
                if (current_line.directive_type == directive_entry) {
                    update_entry_symbol(current_symbol, file_name, line_counter); /* Update symbol type using helper function */
                } else {
                    error = 1;
                    printf("%s:%d: Symbol already exists: '%s'\n", file_name, line_counter, current_symbol->symbol_name);
                }
            } else {
                /* Add new entry or external symbol */
                if (current_line.directive_type == directive_entry) {
                    add_symbol(Unit, current_line.directive_label, temp_entry_symbol, 0, line_counter, 0, 0);
                } else {
                    add_symbol(Unit, current_line.directive_label, external_symbol, 0, line_counter, 0, 0);
                }
            }
        }
    }

    /* Post-processing: Update symbol addresses and add entries to the entries list */
    for (i = 0; i < Unit->symbols_size; i++) {
    if (Unit->symbols[i].symbol_type == temp_entry_symbol) {
        error = 1;
        continue;
    }
    adjust_symbol_address(&Unit->symbols[i], instruction_counter);
    add_to_entries(Unit, &Unit->symbols[i]);
}

    return error; /* If treated nicely, returns no errors. */
}
