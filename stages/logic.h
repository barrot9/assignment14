#ifndef LOGIC_H
#define LOGIC_H
    enum opcode{
        mov,
        cmp,
        add,
        sub,
        lea,
        clr,
        not,
        inc,
        dec,
        jmp,
        bne,
        red,
        prn,
        jsr,
        rts,
        stop
    };



struct parse {
    char err[300];
    char * label;
    enum {
        parse_code,
        parse_dir,
        parse_define
    }parse_type;
    enum opcode i_op;
    enum {
        operand_nothing = -1,
        operand_immed = 0,
        operand_label = 1,
        operand_indirect_register = 2,
        operand_direct_register = 3
    }operand_type[2];
    struct {
        int reg;
        int immed;
        char * label;
    }operands[2];
    char * define_label;
    int  define_c;

    enum {
        d_string,
        d_data,
        d_extern,
        d_entry
    }d_type;
    char * dir_label;
    char * dir_str;
    int data[100];
    int data_size;
};


void parser(char * line);

#endif