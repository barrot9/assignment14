/*
 * This file implements the second stage of an assembly process for a custom assembly language.
 * It continues the process of the assembly file, generating machine code for instructions
 * and handling data directives.
 * The second stage focuses on converting the processed assembly code into machine code.
 * It reads the assembly file line by line, generates appropriate binary representations
 * for instructions and data, and handles symbol references.
 */

#include "secondStage.h"

/* This function implements the second stage of assembly processing */
int secondStage(struct AssemblyUnit* Unit, FILE* assembly_file, char *file_name) {
    char line[MAX_LENGTH] = {0};
    extern struct analized_line current_line;
    struct symbols_table *current_symbol;
    struct external_symbols_table *current_external_symbol;
    int line_counter = 1;
    int i;

    /* Validate input parameters */
    if (Unit == NULL || assembly_file == NULL || file_name == NULL) {
        fprintf(stderr, "Error: Null pointer passed to secondStage\n");
        return ERROR;
    }

    rewind(assembly_file);

    /* Process each line in the assembly file */
    while (fgets(line, sizeof(line), assembly_file) != NULL) {
        if (analyze_assembly_line(line) != 0) {
            fprintf(stderr, "Error in file %s, line %d: Failed to analyze line\n", file_name, line_counter);
            return ERROR;
        }

        /* Handle code lines */
        if (current_line.line_type == code_line) {
            /* Check if code size limit is exceeded */
            if (Unit->code_size >= MAX_CODE_SIZE) {
                fprintf(stderr, "Error in file %s, line %d: Code size exceeded maximum limit\n", file_name, line_counter);
                return ERROR;
            }

            /* Generate machine code for the instruction */
            Unit->code[Unit->code_size] = current_line.opcode << 11;

            if (current_line.operand_type[0] != none)
                Unit->code[Unit->code_size] |= 1 << (current_line.operand_type[0] + 7);
            if (current_line.operand_type[1] != none)
                Unit->code[Unit->code_size] |= 1 << (current_line.operand_type[1] + 3);
            Unit->code[Unit->code_size] |= 4;
            Unit->code_size++;

            /* Handle different operand combinations */
            if ((current_line.operand_type[0] == direct_register || current_line.operand_type[0] == indirect_register) &&
                (current_line.operand_type[1] == direct_register || current_line.operand_type[1] == indirect_register)) {
                /* Handle register-to-register operations */
                if (Unit->code_size >= MAX_CODE_SIZE) {
                    fprintf(stderr, "Error in file %s, line %d: Code size exceeded maximum limit\n", file_name, line_counter);
                    return ERROR;
                }
                Unit->code[Unit->code_size] = current_line.operand_list[0].register_num << 6;
                Unit->code[Unit->code_size] |= current_line.operand_list[1].register_num << 3;
                Unit->code[Unit->code_size++] |= 4;
            } else {
                /* Handle other operand types */
                for (i = 0; i < 2; i++) {
                    if (current_line.operand_type[i] == none) {
                        /* Do nothing for empty operands */
                    } else if (current_line.operand_type[i] == immediate) {
                        /* Handle immediate values */
                        if (Unit->code_size >= MAX_CODE_SIZE) {
                            fprintf(stderr, "Error in file %s, line %d: Code size exceeded maximum limit\n", file_name, line_counter);
                            return ERROR;
                        }
                        Unit->code[Unit->code_size++] = (current_line.operand_list[i].immediate_value << 3) | 4;
                    } else if (current_line.operand_type[i] == label) {
                        /* Handle labels and symbols */
                        current_symbol = search_symbol(Unit, current_line.operand_list[i].label_name);
                        if (current_symbol) {
                            if (current_symbol->symbol_type == external_symbol) {
                                /* Process external symbols */
                                current_external_symbol = search_external_symbol(Unit, current_symbol->symbol_name);
                                if (current_external_symbol) {
                                    if (current_external_symbol->address_count >= MAX_EXTERNAL_ADDRESSES) {
                                        fprintf(stderr, "Error in file %s, line %d: Too many references to external symbol '%s'\n", 
                                                file_name, line_counter, current_symbol->symbol_name);
                                        return ERROR;
                                    }
                                    current_external_symbol->external_symbol_addresses[current_external_symbol->address_count++] = Unit->code_size + INIT_ADDRESS;
                                } else {
                                    /* Add new external symbol */
                                    if (Unit->externals_size >= MAX_EXTERNALS) {
                                        fprintf(stderr, "Error in file %s, line %d: Too many external symbols\n", file_name, line_counter);
                                        return ERROR;
                                    }
                                    current_external_symbol = &Unit->externals[Unit->externals_size];
                                    current_external_symbol->external_symbol_addresses[0] = Unit->code_size + INIT_ADDRESS;
                                    current_external_symbol->address_count = 1;
                                    current_external_symbol->external_symbol_name = current_symbol->symbol_name;
                                    Unit->externals_size++;
                                }
                                if (Unit->code_size >= MAX_CODE_SIZE) {
                                    fprintf(stderr, "Error in file %s, line %d: Code size exceeded maximum limit\n", file_name, line_counter);
                                    return ERROR;
                                }
                                Unit->code[Unit->code_size++] = 1;
                            } else {
                                /* Handle internal symbols */
                                if (Unit->code_size >= MAX_CODE_SIZE) {
                                    fprintf(stderr, "Error in file %s, line %d: Code size exceeded maximum limit\n", file_name, line_counter);
                                    return ERROR;
                                }
                                Unit->code[Unit->code_size++] = ((current_symbol->symbol_address << 3) | 2);
                            }
                        } else {
                            fprintf(stderr, "Error in file %s, line %d: Unrecognized symbol '%s'\n", 
                                    file_name, line_counter, current_line.operand_list[i].label_name);
                            return ERROR;
                        }
                    } else if (current_line.operand_type[i] == direct_register || current_line.operand_type[i] == indirect_register) {
                        /* Handle register operands */
                        if (Unit->code_size >= MAX_CODE_SIZE) {
                            fprintf(stderr, "Error in file %s, line %d: Code size exceeded maximum limit\n", file_name, line_counter);
                            return ERROR;
                        }
                        Unit->code[Unit->code_size++] = (current_line.operand_list[i].register_num << (6 - (i * 3))) | 4;
                    } else {
                        fprintf(stderr, "Error in file %s, line %d: Invalid operand type\n", file_name, line_counter);
                        return ERROR;
                    }
                }
            }
        } else if (current_line.line_type == directive_line && current_line.directive_type <= directive_data) {
            /* Handle directive lines */
            if (current_line.directive_type == directive_data) {
                /* Process data directive */
                for (i = 0; i < current_line.data_size; i++) {
                    if (Unit->data_size >= MAX_DATA_SIZE) {
                        fprintf(stderr, "Error in file %s, line %d: Data size exceeded maximum limit\n", file_name, line_counter);
                        return ERROR;
                    }
                    Unit->data[Unit->data_size++] = current_line.data_value[i];
                }
            } else {
                /* Process string directive */
                for (i = 0; i < strlen(current_line.directive_string) + 1; i++) {
                    if (Unit->data_size >= MAX_DATA_SIZE) {
                        fprintf(stderr, "Error in file %s, line %d: Data size exceeded maximum limit\n", file_name, line_counter);
                        return ERROR;
                    }
                    Unit->data[Unit->data_size++] = current_line.directive_string[i];
                }
            }
        }

        line_counter++;
    }

    /* Check for file read errors */
    if (ferror(assembly_file)) {
        fprintf(stderr, "Error: Failed to read from assembly file %s\n", file_name);
        return ERROR;
    }

    return 0;  /* If treated nicely, returns a success flag */
}
