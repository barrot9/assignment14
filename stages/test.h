#ifndef PROG_H
#define PROG_H
enum symbol_type {
    type_code,
    type_data,
    type_entry_temporary,
    type_entry_data,
    type_entry_code,
    type_external
};
struct symbol  {
    char name[32];
    enum symbol_type sym_type;
    int addr;
    int defined_in_line;
    int c_num;
    int data_size;
};
struct external {
    char * ext_name;
    int addr[4096];
    int addr_c;
};

struct test {
    int code[4096];
    int code_size;
    int data[4096];
    int data_size;

    struct symbol symbols[4096];
    int symbols_size;

    const struct symbol *entries[4096];
    int entries_count;

    struct external externals[4096];
    int externals_size;
};

struct symbol * sym_search_function(struct test * test,char * name);
void add_symbol(struct test * test, char *name, 
                        enum new_symbol_type symbol_type,
                        int address,int line_of_def,int c_number,int data_or_str_size);



struct external * sym_search_function_external(struct test * test,char * name);

#endif
