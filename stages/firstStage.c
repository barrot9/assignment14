#include <stdio.h>
#include <string.h>
#include "logic.h"
#include "assist.h"
#include "test.h"

int first(struct test* test, FILE * AMFILE, char *AMFILENAME ) {
    int  err = 0;
    char line[81] = {0};
    extern struct parse p_line;
    struct symbol *symbol_f;
    int line_c = 1;
    int ic = 100;
    int dc = 0;
    int i;

    for(;fgets(line,sizeof(line),AMFILE);line_c++) {
        parser(line);

        if(p_line.err[0] !='\0') {
            err =1;
            print_error(AMFILENAME,line_c,"parser error: %s",p_line.err);
            continue;
        }

        if(p_line.label && ((p_line.parse_type == parse_dir && p_line.d_type <= d_data )  || p_line.parse_type == parse_code)) {
            symbol_f = sym_search_function(prog,p_line.label);
            if(symbol_f) {
                if(symbol_f->sym_type == new_type_entry_temporary) {
                    if(p_line.parse_type == parse_code) {
                        symbol_f->addr = ic;
                        symbol_f->defined_in_line = line_c;
                        symbol_f->sym_type = new_type_entry_code;
                    }else {
                        symbol_f->addr = dc;
                        symbol_f->defined_in_line = line_c;
                        symbol_f->sym_type = new_type_entry_data;
                    }
                }else {
                    err = 0;
                    print_error(AMFILENAME,line_c,"redefinition of symbol:'%s'",symbol_f->name);
                }
            }else {
                if(p_line.parse_type == parse_code) {
                    add_symbol(prog,p_line.label,new_type_code,ic,line_c,0,0);
                }else {
                    if(p_line.d_type == d_data) {
                        add_symbol(prog,p_line.label,new_type_data,dc,line_c,0,p_line.data_size);
                    }else {
                        add_symbol(prog,p_line.label,new_type_data,dc,line_c,0,strlen(p_line.dir_str));
                    }
                }
            }
        }

        if(p_line.parse_type == parse_code) {
            ic++;
            if((p_line.inst_operand_type[0] == operand_direct_register  || p_line.inst_operand_type[0] == operand_indirect_register ) && (p_line.inst_operand_type[1] == operand_direct_register  || p_line.inst_operand_type[1] == operand_indirect_register )) {
                ic++;
            }else {
                for(i=0;i<2;i++) {
                    switch(p_line.inst_operand_type[i]) {
                        case operand_immed:   case operand_direct_register: case operand_indirect_register: case operand_label:
                            ic++;
                        break;
                        default:
                        break;
                    }
                }
            }
        }else if(p_line.parse_type == parse_dir && p_line.d_type <= d_data) {
            if(p_line.d_type == d_data) {
                dc += p_line.data_size;
            }else {
                dc += strlen(p_line.dir_str) +1;
            }
        }
        else if((p_line.parse_type == parse_dir && p_line.d_type > d_data ) ) {
            symbol_f = sym_search_function(prog,p_line.dir_label);
            if(symbol_f) {
                if(p_line.d_type == d_entry) {
                    if(symbol_f->sym_type == new_type_code) {
                        symbol_f->sym_type = new_type_entry_code;
                    }else if(symbol_f->sym_type == new_type_data) {
                        symbol_f->sym_type = new_type_entry_data;
                    }else {
                        err = 1;
                        print_error(AMFILENAME,line_c,"redefinition of symbol:'%s'",symbol_f->name);
                    }
                }else {
                    err = 1;
                    print_error(AMFILENAME,line_c,"redefinition of symbol:'%s'",symbol_f->name);
                }
            }else {
                if(p_line.d_type == d_entry) {
                    add_symbol(prog,p_line.dir_label,new_type_entry_temporary,0,line_c,0,0);
                }else {
                    add_symbol(prog,p_line.dir_label,new_type_external,0,line_c,0,0);
                }
            }
        }
        
    }

    for(i=0;i<prog->symbols_size;i++) {
        if(prog->symbols[i].sym_type == new_type_entry_temporary) {
            err = 1;
        }else {
            if(prog->symbols[i].sym_type == new_type_data || prog->symbols[i].sym_type == new_type_entry_data) {
                prog->symbols[i].addr += ic;
            }
            if(prog->symbols[i].sym_type == new_type_entry_code || prog->symbols[i].sym_type == new_type_entry_data) {
                prog->entries[prog->entries_count] = &prog->symbols[i];
                prog->entries_count++;
            }
        }
    }

    return err;
}
