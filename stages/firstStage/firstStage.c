/*
   This file implements the first pass (firstStage) of an assembler.
   It processes each line of the assembly code, identifies and manages symbols, 
   updates instruction and data counters.
*/

#include <stdio.h>
#include <string.h>
#include "../utils/struct.h" 
#include "../lineAnalyzer/line_info.h" 
#include "../utils/utils.h"

int firstStage(struct SymbolTableManager* symbolManager, LineInfo* head) {
    int err = 0;               /* Error flag initialized to 0 */
    LineInfo* current = head;  /* Start with the head of the linked list */
    struct symbol* symbol_f;   /* Pointer for symbol lookup */
    int line_c = 1;            /* Line counter starts from 1 */
    int ic = 100;              /* Instruction counter starts from 100 */
    int dc = 0;                /* Data counter starts from 0 */
    int i;                     /* General-purpose loop variable */

    /* Iterate through each line in the linked list */
    while (current != NULL) {
       
        /* Handle unknown line types */
        if (current->type == LINE_UNKNOWN) {
            err = 1;
            printf("Error in line %d: Unknown line type or unrecognized instruction\n", line_c);
            line_c++;
            current = current->next;
            continue;  /* Move to the next line */
        }

        /* Handle lines with labels that are either directives or instructions */
        if (strlen(current->label) > 0 &&
            (current->type == LINE_DIRECTIVE || current->type == LINE_INSTRUCTION)) {
            
            /* Search for the symbol in the symbol table */
            symbol_f = sym_search_function(symbolManager, current->label); 

            if (symbol_f) { /* If the symbol already exists */
                if (symbol_f->sym_type == new_type_entry_temporary) {
                    /* Update symbol information based on the line type */
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
                /* Add a new symbol based on the line type */
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
            /* Increase counter for each operand based on its addressing method */
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
        /* Update data counter for data directives */
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
        line_c++; 
        current = current->next; 
    }

    /* Final adjustments to symbol addresses and handling of unresolved entries */
    for (i = 0; i < symbolManager->symbols_size; i++) {
        if (symbolManager->symbols[i].sym_type == new_type_entry_temporary) {
            err = 1; 
            printf("Error: Unresolved entry: '%s'\n", symbolManager->symbols[i].name);
        } else {
            /* Adjust addresses for data symbols */
            if (symbolManager->symbols[i].sym_type == new_type_data || symbolManager->symbols[i].sym_type == new_type_entry_data) {
                symbolManager->symbols[i].addr += ic; 
            }
            /* Track entry symbols for later use */
            if (symbolManager->symbols[i].sym_type == new_type_entry_code || symbolManager->symbols[i].sym_type == new_type_entry_data) {
                symbolManager->entries[symbolManager->entries_count] = &symbolManager->symbols[i];
                symbolManager->entries_count++;
            }
        }
    }

    return err; /* Return the error status */
}
