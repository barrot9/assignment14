/*
   This file implements the `secondStage` function, which conducts the second pass
   of an assembly language file to complete the generation of machine code and data sections.
   The function reads through the assembly file, processes each line to resolve symbols,
   update the code and data sections, and handle external references. 
*/

#include <stdio.h>
#include "stages/utils/struct.h" /* Symbol and program structure definitions */
#include "stages/utils/logic.h"  /* Parsing logic and opcode definitions */
#include "stages/utils/supp.h"   /* Supporting utilities */

/*
 * Function: secondStage
 * ---------------------
 * Processes the second pass of an assembly file to complete code and data section generation.
 * This function analyzes each line of the assembly code, updates code and data sections, 
 * resolves symbols, and handles external references.
 */
int secondStage(struct SymbolTableManager* symbolManager, FILE* AMFILE, char *AMFILENAME) {
    int err = 0; /* Error flag initialized to 0 */
    char line[101] = {0}; /* Buffer to hold a line from the file, now set to 101 characters */
    extern struct AssemblyLine p_line; /* External structure holding parsed line information */
    struct symbol *symbol_f; /* Pointer to a symbol structure for symbol table lookup */
    struct external *ext_f; /* Pointer to an external structure for external references */
    int line_c = 1; /* Line counter initialized to 1 */
    int i; /* General-purpose loop variable */
    
    rewind(AMFILE); /* Reset the file position to the beginning for second pass processing */
    
    /* Read and process each line from the assembly file */
    while (fgets(line, sizeof(line), AMFILE)) {
        parser(line); /* Parse the current line using the parser function */
        
        /* Handle code lines */
        if (p_line.line_type == code_line) {
            symbolManager->code[symbolManager->code_size] = p_line.i_op << 11; /* Set opcode */

            /* Set bits for operand types */
            if (p_line.inst_operand_type[0] != operand_nothing) {
                symbolManager->code[symbolManager->code_size] |= 1 << (p_line.inst_operand_type[0] + 7);
            }
            if (p_line.inst_operand_type[1] != operand_nothing) {
                symbolManager->code[symbolManager->code_size] |= 1 << (p_line.inst_operand_type[1] + 3);
            }
            symbolManager->code[symbolManager->code_size] |= 4; /* Set function bits */
            symbolManager->code_size++; /* Increment code size */

            /* Process register operands */
            if ((p_line.inst_operand_type[0] == operand_direct_register  || p_line.inst_operand_type[0] == operand_indirect_register) &&
                (p_line.inst_operand_type[1] == operand_direct_register  || p_line.inst_operand_type[1] == operand_indirect_register)) {
                symbolManager->code[symbolManager->code_size] = p_line.operands[0].reg << 6;
                symbolManager->code[symbolManager->code_size] |= p_line.operands[1].reg << 3;
                symbolManager->code[symbolManager->code_size++] |= 4;
            } else {
                for (i = 0; i < 2; i++) {
                    switch (p_line.inst_operand_type[i]) {
                        case operand_nothing:
                            break;
                        case operand_immed:
                            symbolManager->code[symbolManager->code_size++] = (p_line.operands[i].immed << 3) | 4;
                            break;
                        case operand_label:
                            symbol_f = sym_search_function(symbolManager, p_line.operands[i].label);
                            if (symbol_f) {
                                if (symbol_f->sym_type == new_type_external) {
                                    ext_f = sym_search_function_external(symbolManager, symbol_f->name);
                                    if (ext_f) {
                                        ext_f->addr[ext_f->addr_c++]  = symbolManager->code_size + 100;
                                    } else {
                                        ext_f = &symbolManager->externals[symbolManager->externals_size];
                                        ext_f->addr[ext_f->addr_c++] = symbolManager->code_size + 100;
                                        ext_f->ext_name = symbol_f->name;
                                        symbolManager->externals_size++;
                                    }
                                    symbolManager->code[symbolManager->code_size++] = 1;
                                } else {
                                    symbolManager->code[symbolManager->code_size++] = ((symbol_f->addr << 3) | 2);
                                }
                            } else {
                                err = 1;
                                print_error(AMFILENAME, line_c, "undefined symbol:'%s'", p_line.operands[i].label);
                            }
                            break;
                        case operand_direct_register: 
                        case operand_indirect_register:
                            symbolManager->code[symbolManager->code_size++] = (p_line.operands[i].reg <<  (6 - (i * 3))) | 4;
                            break;
                        default:
                            break;
                    }
                }
            }
        } else if (p_line.line_type == directive_line && p_line.directive_type <= data) {
            /* Handle data directives */
            if (p_line.directive_type == data) {
                for (i = 0; i < p_line.data_size; i++) {
                    symbolManager->data[symbolManager->data_size++] = p_line.data[i];
                }
            } else {
                for (i = 0; i < strlen(p_line.dir_str) + 1; i++) {
                    symbolManager->data[symbolManager->data_size++] = p_line.dir_str[i];
                }
            }
        }
        line_c++; /* Increment line counter */
    }
    
    return err; /* Return the error status */
}
