#include <stdio.h>
#include <string.h>
#include "stages/utils/struct.h"            /* Symbol and program structure definitions */
#include "stages/utils/logic.h"             /* Parsing logic and opcode definitions */
#include "stages/utils/supp.h"              /* Supporting utilities */
#include "stages/lineAnalyzer/line_info.h"  /* LineInfo structure and related definitions */

/*
 * Processes the second pass of an assembly file to complete code and data section generation.
 * This function analyzes each line of the assembly code, updates code and data sections, 
 * resolves symbols, and handles external references.
 */
int secondStage(struct SymbolTableManager* symbolManager, LineInfo* head) {
    int err = 0; /* Error flag initialized to 0 */
    LineInfo* current = head; /* Pointer to the current node in the linked list */
    struct symbol* symbol_f; /* Pointer to a symbol structure for symbol table lookup */
    struct external* ext_f;  /* Pointer to an external structure for external references */
    int line_c = 1;          /* Line counter initialized to 1 */
    int i;                   /* General-purpose loop variable */

    /* Iterate through the linked list of parsed lines */
    while (current != NULL) {
        /* Handle instruction lines */
        if (current->type == LINE_INSTRUCTION) {
            /* Set opcode in the code section */
            symbolManager->code[symbolManager->code_size] = current->opcode << 11;

            /* Set bits for operand types */
            for (i = 0; i < current->operandCount; i++) {
                if (current->addressingMethods[i] == ADDRESSING_DIRECT_REGISTER || 
                    current->addressingMethods[i] == ADDRESSING_INDIRECT_REGISTER) {
                    symbolManager->code[symbolManager->code_size] |= 1 << (i == 0 ? 7 : 3);
                }
            }

            symbolManager->code_size++; /* Increment code size */

            /* Process register and other operand types */
            for (i = 0; i < current->operandCount; i++) {
                switch (current->addressingMethods[i]) {
                    case ADDRESSING_IMMEDIATE:
                        symbolManager->code[symbolManager->code_size++] = 
                            (atoi(current->operands[i]) << 3) | 4;
                        break;

                    case ADDRESSING_DIRECT:
                        symbol_f = sym_search_function(symbolManager, current->operands[i]);
                        if (symbol_f) {
                            if (symbol_f->sym_type == new_type_external) {
                                ext_f = sym_search_function_external(symbolManager, symbol_f->name);
                                if (ext_f) {
                                    ext_f->addr[ext_f->addr_c++] = symbolManager->code_size + 100;
                                } else {
                                    ext_f = &symbolManager->externals[symbolManager->externals_size];
                                    ext_f->addr[ext_f->addr_c++] = symbolManager->code_size + 100;
                                    ext_f->ext_name = symbol_f->name;
                                    symbolManager->externals_size++;
                                }
                                symbolManager->code[symbolManager->code_size++] = 1;
                            } else {
                                symbolManager->code[symbolManager->code_size++] = 
                                    (symbol_f->addr << 3) | 2;
                            }
                        } else {
                            err = 1;
                            print_error("Error in line", line_c, "undefined symbol:'%s'", current->operands[i]);
                        }
                        break;

                    case ADDRESSING_DIRECT_REGISTER:
                    case ADDRESSING_INDIRECT_REGISTER:
                        symbolManager->code[symbolManager->code_size++] = 
                            (current->operands[i][1] - '0') << (i == 0 ? 6 : 3) | 4;
                        break;

                    default:
                        break;
                }
            }
        }
        /* Handle data directives */
        else if (current->type == LINE_DIRECTIVE) {
            if (strncmp(current->line, ".data", 5) == 0) {
                for (i = 0; i < current->operandCount; i++) {
                    symbolManager->data[symbolManager->data_size++] = atoi(current->operands[i]);
                }
            } else if (strncmp(current->line, ".string", 7) == 0) {
                for (i = 0; i < strlen(current->operands[0]) + 1; i++) {
                    symbolManager->data[symbolManager->data_size++] = current->operands[0][i];
                }
            }
        }

        line_c++; /* Increment line counter */
        current = current->next; /* Move to the next line in the linked list */
    }

    return err; /* Return the error status */
}


/*---------- temporary function to check for generated code --------------*/

void printGeneratedCode(struct SymbolTableManager* symbolManager) {
    int i;
    
    printf("Generated Code:\n");
    printf("----------------\n");
    
    for (i = 0; i < symbolManager->code_size; i++) {
        printf("0x%04X: %04X\n", i + 100, symbolManager->code[i]);
    }
    
    printf("----------------\n");
    printf("Total instructions: %d\n", symbolManager->code_size);
}

