/*
   This module implements the 'first' function to process an assembly source file.
   It parses each line to build and manage the symbol table, handling labels,
   entry points, and external symbols, and ensures proper addressing and error detection.
*/

#include <stdio.h>
#include <string.h>
#include "../utils/struct.h" /* Symbol and program structure definitions */
#include "../lineAnalyzer/line_info.h" /* LineInfo structure and related definitions */
#include "../utils/utils.h"

/*
 * The 'first' function processes an assembly file and updates the symbol table.
 * It parses lines to determine whether they define data, code, or directives
 * and updates the instruction and data counters accordingly.
 */
int first(struct SymbolTableManager* symbolManager, LineInfo* head) {
    int err = 0;               /* Error indicator */
    LineInfo* current = head;  /* Pointer to the current node in the linked list */
    struct symbol* symbol_f;   /* Pointer for symbol lookup */
    int line_c = 1;            /* Line count starts from 1 */
    int ic = 100;              /* Instruction counter starts from 100 */
    int dc = 0;                /* Data counter starts from 0 */
    int i;                     /* General-purpose loop variable */

    /* Iterate through the linked list of parsed lines */
    while (current != NULL) {
        /* Check for errors in the current line */
        if (current->type == LINE_UNKNOWN) {
            err = 1;
            printf("Error in line %d: Unknown line type or unrecognized instruction\n", line_c);
            line_c++;
            current = current->next;
            continue;
        }

        /* Handle lines with labels that are either directives or code */
        if (strlen(current->label) > 0 &&
            (current->type == LINE_DIRECTIVE || current->type == LINE_INSTRUCTION)) {
            
            symbol_f = sym_search_function(symbolManager, current->label); /* Search for the symbol */

            if (symbol_f) { /* If symbol is found */
                if (symbol_f->sym_type == new_type_entry_temporary) {
                    /* Update symbol information based on line type */
                    if (current->type == LINE_INSTRUCTION) {
                        symbol_f->addr = ic;
                        symbol_f->defined_in_line = line_c;
                        symbol_f->sym_type = new_type_entry_code;
                    } else {
                        symbol_f->addr = dc;
                        symbol_f->defined_in_line = line_c;
                        symbol_f->sym_type = new_type_entry_data;
                    }
                } else {
                    err = 1;
                    printf("Error in line %d: Redefinition of symbol: '%s'\n", line_c, symbol_f->name);
                }
            } else {
                /* Add new symbol based on line type */
                if (current->type == LINE_INSTRUCTION) {
                    add_symbol(symbolManager, current->label, new_type_code, ic, line_c, 0, 0);
                } else {
                    if (strncmp(current->line, ".data", 5) == 0) {
                        add_symbol(symbolManager, current->label, new_type_data, dc, line_c, 0, current->operandCount);
                    } else if (strncmp(current->line, ".string", 7) == 0) {
                        add_symbol(symbolManager, current->label, new_type_data, dc, line_c, 0, strlen(current->operands[0]));
                    } else {
                        add_symbol(symbolManager, current->label, new_type_data, dc, line_c, 0, 0);
                    }
                }
            }
        }

        /* Update instruction counter for instruction lines */
        if (current->type == LINE_INSTRUCTION) {
            ic++;
            /* Check and handle operand types */
            for (i = 0; i < current->operandCount; i++) {
                switch (current->addressingMethods[i]) {
                    case ADDRESSING_IMMEDIATE:
                    case ADDRESSING_DIRECT:
                    case ADDRESSING_INDIRECT_REGISTER:
                    case ADDRESSING_DIRECT_REGISTER:
                        ic++;
                        break;
                    default:
                        break;
                }
            }
        }
        /* Update data counter for .data and .string directives */
        else if (current->type == LINE_DIRECTIVE) {
            if (strncmp(current->line, ".data", 5) == 0) {
                dc += current->operandCount;
            } else if (strncmp(current->line, ".string", 7) == 0) {
                dc += strlen(current->operands[0]) + 1;
            }
        }
        /* Handle entry and external directives */
        else if (current->isEntry || current->isExtern) {
            symbol_f = sym_search_function(symbolManager, current->label);
            if (symbol_f) {
                if (current->isEntry) {
                    if (symbol_f->sym_type == new_type_code) {
                        symbol_f->sym_type = new_type_entry_code;
                    } else if (symbol_f->sym_type == new_type_data) {
                        symbol_f->sym_type = new_type_entry_data;
                    } else {
                        err = 1;
                        printf("Error in line %d: Redefinition of symbol: '%s'\n", line_c, symbol_f->name);
                    }
                } else {
                    err = 1;
                    printf("Error in line %d: Redefinition of symbol: '%s'\n", line_c, symbol_f->name);
                }
            } else {
                if (current->isEntry) {
                    add_symbol(symbolManager, current->label, new_type_entry_temporary, 0, line_c, 0, 0);
                } else {
                    add_symbol(symbolManager, current->label, new_type_external, 0, line_c, 0, 0);
                }
            }
        }
        line_c++; /* Increment line count */
        current = current->next; /* Move to the next node in the linked list */
    }

    /* Final adjustments to symbol addresses and handling entries */
    for (i = 0; i < symbolManager->symbols_size; i++) {
        if (symbolManager->symbols[i].sym_type == new_type_entry_temporary) {
            err = 1; /* Flag error for temporary entries */
            printf("Error: Unresolved entry: '%s'\n", symbolManager->symbols[i].name);
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