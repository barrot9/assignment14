/*
 * This header file defines the structures, enumerations, and function prototypes
 * for interpreting and analyzing the lines of the assembly code.
 * This header serves as the interface for the line interpretation phase
 * of the assembly process. It provides the necessary tools for analyzing,
 * categorizing, and validating individual lines of the assembly code.
 */

#ifndef LINE_INTERPRETER_H
#define LINE_INTERPRETER_H

#include "utils.h"

/* Enumeration for opcodes */
typedef enum opcode {
    opcode_mov, opcode_cmp, opcode_add, opcode_sub, opcode_lea, opcode_clr, opcode_not, opcode_inc,
    opcode_dec, opcode_jmp, opcode_bne, opcode_red, opcode_prn, opcode_jsr, opcode_rts, opcode_stop
} opcode;

/* Structure to represent an operand */
typedef struct operand {
    int register_num;
    int immediate_value;
    char *label_name;
} operand;

/* Enumeration for line types */
typedef enum line_type {
    code_line,
    directive_line,
    definition_line
} line_type;

/* Enumeration for directive types */
typedef enum directive_type {
    directive_string,
    directive_data,
    directive_extern,
    directive_entry
} directive_type;

/* Enumeration for operand types */
typedef enum operand_type {
    none = NONE,
    immediate = IMMEDIATE,
    label = LABEL,
    indirect_register = INDIRECT,
    direct_register = DIRECT
} operand_type;

/* Structure to represent an analyzed line of assembly code */
typedef struct analized_line {
    char error[MAX_ERROR_LENGTH];
    char *label_name;
    line_type line_type;  
    opcode opcode;
    operand_type operand_type[2];
    operand operand_list[2];
    char *definition_label;
    int definition_count;
    directive_type directive_type;  
    char *directive_label;
    char *directive_string;
    int data_value[MAX_DATA_VALUE_LENGTH];
    int data_size;
} analized_line;

/* declarations of structures */
typedef struct StringTokens StringTokens; 
typedef struct OpcodeInfo OpcodeInfo;  
typedef struct OperandInfo OperandInfo;  
typedef struct DirectiveInfo DirectiveInfo; 

/* Function Prototypes */
int analyze_assembly_line(char *assembly_line);
int validate_label(char *label_name, int brackets);
struct StringTokens split_by_spaces(char *input_string);
struct OpcodeInfo *find_instruction_by_name(char *instruction_name);
struct DirectiveInfo *find_directive_by_name(char *directive_name);
int validate_number(char *number_str, int *value, int min_value, int max_value);
int analyze_operand(char *operand_text, struct OpcodeInfo *instruction, struct OperandInfo *operand_info, int operand_index);
int is_valid_register(const char *operand);
int check_for_extra_text(char *operand_text);
int analyze_direct_register(char *operand_text, int operand_index);
int analyze_indirect_register(char *operand_text, int operand_index);
int analyze_immediate_value(char *operand_text, int operand_index);
int analyze_label(char *operand_text, int operand_index);
int pre_analyze_operand(char *operands_text, struct OpcodeInfo *instruction, struct OperandInfo *operand_info);
int split_operands(char *operands_text, char **first_operand, char **second_operand);
int validate_operand_count(int operand_count, struct OperandInfo *operand_info);
int process_directive(char *directive_text, struct DirectiveInfo *directive_info);
int process_entry_extern(char *directive_text);
int process_string(char *directive_text);
int process_data(char *directive_text);

#endif 