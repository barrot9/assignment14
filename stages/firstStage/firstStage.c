/*
   This module implements the 'first' function to process an assembly source file.
   It parses each line to build and manage the symbol table, handling labels,
   entry points, and external symbols, and ensures proper addressing and error detection.
*/

#include <stdio.h>
#include <string.h>
#include "stages/utils/struct.h" /* Symbol and program structure definitions */
#include "stages/utils/logic.h"  /* Parsing logic and opcode definitions */
#include "stages/utils/supp.h"   /* Supporting utilities */

/*
 * The 'first' function processes an assembly file and updates the symbol table.
 * It parses lines to determine whether they define data, code, or directives
 * and updates the instruction and data counters accordingly.
 */
int first(struct SymbolTableManager* symbolManager, FILE *AMFILE, char *AMFILENAME) {
    int err = 0;               /* Error indicator */
    char line[81] = {0};       /* Line buffer for reading the file */
    extern struct AssemblyLine p_line; /* Parsed line structure */
    struct symbol *symbol_f;   /* Pointer for symbol lookup */
    int line_c = 1;            /* Line count starts from 1 */
    int ic = 100;              /* Instruction counter starts from 100 */
    int dc = 0;                /* Data counter starts from 0 */
    int i;                     /* General-purpose loop variable */

    /* Process each line of the assembly file */
    while (fgets(line, sizeof(line), AMFILE)) {
        parser(line); /* Invoke parser on the current line */

        /* Check for parsing errors */
        if (p_line.err[0] != '\0') {
            err = 1; /* Mark error */
            print_error(AMFILENAME, line_c, "parser error: %s", p_line.err);
            line_c++;
            continue; /* Proceed to the next line */
        }

        /* Handle lines with labels that are either directives or code */
        if (p_line.label && ((p_line.line_type == directive_line && p_line.directive_type <= data) || p_line.line_type == code_line)) {
            symbol_f = sym_search_function(symbolManager, p_line.label); /* Search for the symbol */

            if (symbol_f) { /* If symbol is found */
                if (symbol_f->sym_type == new_type_entry_temporary) {
                    /* Update symbol information based on line type */
                    if (p_line.line_type == code_line) {
                        symbol_f->addr = ic;
                        symbol_f->defined_in_line = line_c;
                        symbol_f->sym_type = new_type_entry_code;
                    } else {
                        symbol_f->addr = dc;
                        symbol_f->defined_in_line = line_c;
                        symbol_f->sym_type = new_type_entry_data;
                    }
                } else {
                    err = 0; /* Reset error flag */
                    print_error(AMFILENAME, line_c, "redefinition of symbol:'%s'", symbol_f->name);
                }
            } else {
                /* Add new symbol based on line type */
                if (p_line.line_type == code_line) {
                    add_symbol(symbolManager, p_line.label, new_type_code, ic, line_c, 0, 0);
                } else {
                    if (p_line.directive_type == data) {
                        add_symbol(symbolManager, p_line.label, new_type_data, dc, line_c, 0, p_line.data_size);
                    } else {
                        add_symbol(symbolManager, p_line.label, new_type_data, dc, line_c, 0, strlen(p_line.dir_str));
                    }
                }
            }
        }

        /* Update instruction counter for code lines */
        if (p_line.line_type == code_line) {
            ic++;
            /* Check and handle operand types */
            if ((p_line.inst_operand_type[0] == operand_direct_register || p_line.inst_operand_type[0] == operand_indirect_register) &&
                (p_line.inst_operand_type[1] == operand_direct_register || p_line.inst_operand_type[1] == operand_indirect_register)) {
                ic++;
            } else {
                for (i = 0; i < 2; i++) {
                    switch (p_line.inst_operand_type[i]) {
                        case operand_immed:
                        case operand_direct_register:
                        case operand_indirect_register:
                        case operand_label:
                            ic++;
                            break;
                        default:
                            break;
                    }
                }
            }
        }
        /* Update data counter for data directives */
        else if (p_line.line_type == directive_line && p_line.directive_type <= data) {
            if (p_line.directive_type == data) {
                dc += p_line.data_size;
            } else {
                dc += strlen(p_line.dir_str) + 1;
            }
        }
        /* Handle entry and external directives */
        else if ((p_line.line_type == directive_line && p_line.directive_type > data)) {
            symbol_f = sym_search_function(symbolManager, p_line.dir_label);
            if (symbol_f) {
                if (p_line.directive_type == entry) {
                    if (symbol_f->sym_type == new_type_code) {
                        symbol_f->sym_type = new_type_entry_code;
                    } else if (symbol_f->sym_type == new_type_data) {
                        symbol_f->sym_type = new_type_entry_data;
                    } else {
                        err = 1;
                        print_error(AMFILENAME, line_c, "redefinition of symbol:'%s'", symbol_f->name);
                    }
                } else {
                    err = 1;
                    print_error(AMFILENAME, line_c, "redefinition of symbol:'%s'", symbol_f->name);
                }
            } else {
                if (p_line.directive_type == entry) {
                    add_symbol(symbolManager, p_line.dir_label, new_type_entry_temporary, 0, line_c, 0, 0);
                } else {
                    add_symbol(symbolManager, p_line.dir_label, new_type_external, 0, line_c, 0, 0);
                }
            }
        }
        line_c++; /* Increment line count */
    }

    /* Final adjustments to symbol addresses and handling entries */
    for (i = 0; i < symbolManager->symbols_size; i++) {
        if (symbolManager->symbols[i].sym_type == new_type_entry_temporary) {
            err = 1; /* Flag error for temporary entries */
        } else {
            if (symbolManager->symbols[i].sym_type == new_type_data || symbolManager->symbols[i].sym_type == new_type_entry_data) {
                symbolManager->symbols[i].addr += ic; /* Adjust data symbol address */
            }
            if (symbolManager->symbols[i].sym_type == new_type_entry_code || symbolManager->symbols[i].sym_type == new_type_entry_data) {
                symbolManager->entries[symbolManager->entries_count] = &symbolManager->symbols[i];
                symbolManager->entries_count++;
            }
        }
    }

    return err; /* Return the error status */
}
