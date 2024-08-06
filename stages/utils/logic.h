/*
   Header file defining the structures and enumerations for parsing
   assembly language instructions. Includes opcode definitions,
   operand types, and directive types, as well as the prototype for
   the parser function that processes assembly code lines.
*/

#ifndef LOGIC_H
#define LOGIC_H

/* Enum for opcode definitions used in assembly instructions */
enum opcode {
    _mov,   /* Move operation */
    _cmp,   /* Compare operation */
    _add,   /* Add operation */
    _sub,   /* Subtract operation */
    _lea,   /* Load effective address */
    _clr,   /* Clear operation */
    _not,   /* Logical NOT operation */
    _inc,   /* Increment operation */
    _dec,   /* Decrement operation */
    _jmp,   /* Jump operation */
    _bne,   /* Branch if not equal */
    _red,   /* Read operation */
    _prn,   /* Print operation */
    _jsr,   /* Jump to subroutine */
    _rts,   /* Return from subroutine */
    _stop   /* Stop operation */
};

/* Struct for holding parsed details from an assembly line */
struct AssemblyLine {
    char err[300];  /* Error message buffer */
    char *label;    /* Label pointer */
    
    /* Enum for the type of line being parsed */
    enum {
        code_line,      /* Indicates a code line */
        directive_line, /* Indicates a directive line */
        define_line     /* Indicates a define line */
    } line_type;        /* Line type for the parsed assembly */

    enum opcode i_op;   /* Opcode for the current instruction */

    /* Enum for different operand types */
    enum {
        operand_nothing = -1,          /* No operand */
        operand_immed = 0,             /* Immediate value operand */
        operand_label = 1,             /* Label operand */
        operand_indirect_register = 2, /* Indirect register operand */
        operand_direct_register = 3    /* Direct register operand */
    } inst_operand_type[2]; /* Operand types array */

    /* Struct for operand details */
    struct {
        int reg;       /* Register number */
        int immed;     /* Immediate value */
        char *label;   /* Operand label pointer */
    } operands[2];     /* Operand details array */

    char *define_label; /* Label for define directive */
    int define_c;       /* Count or value for define directive */

    /* Enum for directive types */
    enum {
        string,          /* String directive */
        data,            /* Data directive */
        extern_directive,/* External directive */
        entry            /* Entry directive */
    } directive_type;   /* Type of directive parsed */

    char *dir_label;   /* Label pointer for directive */
    char *dir_str;     /* String pointer for directive */
    int data[100];     /* Data values array */
    int data_size;     /* Size of the data array */
};

/* Function prototype for parsing a line of assembly code */
void parser(char *line);

#endif /* LOGIC_H */
