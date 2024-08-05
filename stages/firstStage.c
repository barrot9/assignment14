/*
   This file contains the implementation of the `first` function, which processes an assembly file
   to build and manage the symbol table. The function reads each line of the file, parses it, and
   updates the symbol table with code and data definitions. It also handles labels, entry points,
   and external symbols, ensuring proper addressing and error checking throughout the process.
 */

#include <stdio.h>
#include <string.h>
#include "struct.h" /* Header file for structure definitions */
#include "logic.h"  /* Header file for logic-related functions */
#include "supp.h"   /* Header file for supplementary functions */

/* Function to process the assembly file and handle symbol table entries */
int first(struct prog* prog, FILE * AMFILE, char *AMFILENAME) {
    int err = 0; /* Error flag initialized to 0 */
    char line[81] = {0}; /* Buffer to hold a line from the file */
    extern struct parse p_line; /* External parse structure for parsed line */
    struct symbol *symbol_f; /* Pointer to a symbol structure */
    int line_c = 1; /* Line counter initialized to 1 */
    int ic = 100; /* Instruction counter initialized to address 100 */
    int dc = 0; /* Data counter initialized to 0 */
    int i; /* Loop variable */

    /* Read each line from the assembly file */
    for (; fgets(line, sizeof(line), AMFILE); line_c++) {
        parser(line); /* Parse the current line */

        /* Check if there is a parsing error */
        if (p_line.err[0] != '\0') {
            err = 1; /* Set error flag */
            print_error(AMFILENAME, line_c, "parser error: %s", p_line.err); /* Print error message */
            continue; /* Skip to the next line */
        }

        /* Check if the line contains a label and is either a directive or code */
        if (p_line.label && ((p_line.parse_type == parse_dir && p_line.d_type <= d_data) || p_line.parse_type == parse_code)) {
            symbol_f = sym_search_function(prog, p_line.label); /* Search for the symbol */
            if (symbol_f) { /* If the symbol is found */
                if (symbol_f->sym_type == new_type_entry_temporary) {
                    /* Update symbol details based on whether it is code or data */
                    if (p_line.parse_type == parse_code) {
                        symbol_f->addr = ic; /* Assign instruction counter as address */
                        symbol_f->defined_in_line = line_c; /* Set line of definition */
                        symbol_f->sym_type = new_type_entry_code; /* Set symbol type */
                    } else {
                        symbol_f->addr = dc; /* Assign data counter as address */
                        symbol_f->defined_in_line = line_c; /* Set line of definition */
                        symbol_f->sym_type = new_type_entry_data; /* Set symbol type */
                    }
                } else {
                    err = 0; /* Reset error flag */
                    print_error(AMFILENAME, line_c, "redefinition of symbol:'%s'", symbol_f->name); /* Print redefinition error */
                }
            } else { /* If the symbol is not found */
                /* Add a new symbol based on the parse type */
                if (p_line.parse_type == parse_code) {
                    add_symbol(prog, p_line.label, new_type_code, ic, line_c, 0, 0); /* Add code symbol */
                } else {
                    if (p_line.d_type == d_data) {
                        add_symbol(prog, p_line.label, new_type_data, dc, line_c, 0, p_line.data_size); /* Add data symbol */
                    } else {
                        add_symbol(prog, p_line.label, new_type_data, dc, line_c, 0, strlen(p_line.dir_str)); /* Add string symbol */
                    }
                }
            }
        }

        /* Increment instruction counter for code lines */
        if (p_line.parse_type == parse_code) {
            ic++; /* Increment instruction counter */
            /* Check operand types and adjust instruction counter */
            if ((p_line.inst_operand_type[0] == operand_direct_register || p_line.inst_operand_type[0] == operand_indirect_register) &&
                (p_line.inst_operand_type[1] == operand_direct_register || p_line.inst_operand_type[1] == operand_indirect_register)) {
                ic++; /* Increment for two register operands */
            } else {
                for (i = 0; i < 2; i++) {
                    /* Increment based on operand type */
                    switch (p_line.inst_operand_type[i]) {
                        case operand_immed: /* Immediate operand */
                        case operand_direct_register: /* Direct register operand */
                        case operand_indirect_register: /* Indirect register operand */
                        case operand_label: /* Label operand */
                            ic++;
                            break;
                        default:
                            break; /* No action for other operand types */
                    }
                }
            }
        } 
        /* Increment data counter for data directives */
        else if (p_line.parse_type == parse_dir && p_line.d_type <= d_data) {
            if (p_line.d_type == d_data) {
                dc += p_line.data_size; /* Increment data counter by data size */
            } else {
                dc += strlen(p_line.dir_str) + 1; /* Increment by string length plus null terminator */
            }
        }
        /* Handle entry and external directives */
        else if ((p_line.parse_type == parse_dir && p_line.d_type > d_data)) {
            symbol_f = sym_search_function(prog, p_line.dir_label); /* Search for the directive label */
            if (symbol_f) { /* If the symbol is found */
                if (p_line.d_type == d_entry) {
                    /* Update symbol type if it is an entry */
                    if (symbol_f->sym_type == new_type_code) {
                        symbol_f->sym_type = new_type_entry_code; /* Set entry code type */
                    } else if (symbol_f->sym_type == new_type_data) {
                        symbol_f->sym_type = new_type_entry_data; /* Set entry data type */
                    } else {
                        err = 1; /* Set error flag for redefinition */
                        print_error(AMFILENAME, line_c, "redefinition of symbol:'%s'", symbol_f->name); /* Print error message */
                    }
                } else {
                    err = 1; /* Set error flag for redefinition */
                    print_error(AMFILENAME, line_c, "redefinition of symbol:'%s'", symbol_f->name); /* Print error message */
                }
            } else { /* If the symbol is not found */
                if (p_line.d_type == d_entry) {
                    add_symbol(prog, p_line.dir_label, new_type_entry_temporary, 0, line_c, 0, 0); /* Add temporary entry symbol */
                } else {
                    add_symbol(prog, p_line.dir_label, new_type_external, 0, line_c, 0, 0); /* Add external symbol */
                }
            }
        }
    }

    /* Adjust symbol addresses and handle entries */
    for (i = 0; i < prog->symbols_size; i++) {
        if (prog->symbols[i].sym_type == new_type_entry_temporary) {
            err = 1; /* Set error flag for temporary entries */
        } else {
            /* Adjust addresses for data symbols */
            if (prog->symbols[i].sym_type == new_type_data || prog->symbols[i].sym_type == new_type_entry_data) {
                prog->symbols[i].addr += ic; /* Add instruction counter to address */
            }
            /* Handle entries */
            if (prog->symbols[i].sym_type == new_type_entry_code || prog->symbols[i].sym_type == new_type_entry_data) {
                prog->entries[prog->entries_count] = &prog->symbols[i]; /* Add to entries list */
                prog->entries_count++; /* Increment entries count */
            }
        }
    }

    return err; /* Return the error status */
}
