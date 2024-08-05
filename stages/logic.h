/*
   This header file defines the structures and enumerations used for parsing
   assembly language instructions. It provides the necessary data types to 
   represent different opcodes, operand types, and directives. This file also 
   includes the prototype for the parser function, which processes a line of 
   assembly code to extract its components.
 */

#ifndef LOGIC_H
#define LOGIC_H

/* Enumeration for different instruction opcodes */
enum inst_opcode {
    inst_mov,   /* Move instruction */
    inst_cmp,   /* Compare instruction */
    inst_add,   /* Add instruction */
    inst_sub,   /* Subtract instruction */
    inst_lea,   /* Load effective address instruction */
    inst_clr,   /* Clear instruction */
    inst_not,   /* Logical NOT instruction */
    inst_inc,   /* Increment instruction */
    inst_dec,   /* Decrement instruction */
    inst_jmp,   /* Jump instruction */
    inst_bne,   /* Branch if not equal instruction */
    inst_red,   /* Read instruction */
    inst_prn,   /* Print instruction */
    inst_jsr,   /* Jump to subroutine instruction */
    inst_rts,   /* Return from subroutine instruction */
    inst_stop   /* Stop instruction */
};

/* Structure to hold parsed information from a line of assembly code */
struct parse {
    char err[300];  /* Error message buffer */
    char *label;    /* Pointer to label string */
    enum {
        parse_code,  /* Code line */
        parse_dir,   /* Directive line */
        parse_define /* Define line */
    } parse_type;   /* Type of parse for the line */

    enum inst_opcode i_op; /* Instruction opcode */

    /* Enumeration for operand types */
    enum {
        operand_nothing = -1,          /* No operand */
        operand_immed = 0,             /* Immediate operand */
        operand_label = 1,             /* Label operand */
        operand_indirect_register = 2, /* Indirect register operand */
        operand_direct_register = 3    /* Direct register operand */
    } inst_operand_type[2]; /* Array of operand types */

    /* Array to hold operand details */
    struct {
        int reg;       /* Register number */
        int immed;     /* Immediate value */
        char *label;   /* Pointer to label string */
    } operands[2];

    char *define_label; /* Pointer to define label string */
    int define_c;       /* Define count or value */

    /* Enumeration for directive types */
    enum {
        d_string, /* String directive */
        d_data,   /* Data directive */
        d_extern, /* External directive */
        d_entry   /* Entry directive */
    } d_type;    /* Type of directive */

    char *dir_label; /* Pointer to directive label string */
    char *dir_str;   /* Pointer to directive string */
    int data[100];   /* Array to hold data values */
    int data_size;   /* Size of data array */
};

/* Function prototype for parsing a line of assembly code */
void parser(char *line);

#endif /* LOGIC_H */
