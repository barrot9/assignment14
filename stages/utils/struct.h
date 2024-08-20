/*
   This header defines the data structures and constants for managing symbols,
   sections, and external references in the assembler. It includes structures 
   for symbols, program data, and external references, along with function 
   prototypes for symbol management.
*/

#ifndef STRUCT_H
#define STRUCT_H

#define SLOTS 4096               

/* Enumeration for symbol types in the assembly program */
enum new_symbol_type {
    new_type_code,              
    new_type_data,               
    new_type_entry_temporary,    
    new_type_entry_data,         
    new_type_entry_code,         
    new_type_external            
};

/* Represents a symbol in the symbol table */
struct symbol {
    char name[32];               
    enum new_symbol_type sym_type; 
    int addr;                    
    int defined_in_line;         
    int c_num;                   
    int data_size;              
};

/* Represents an external reference in the assembly code */
struct external {
    char *ext_name;              
    int addr[SLOTS];             
    int addr_c;                  
};

/* Structure to manage code, data, symbols, and externals */
struct SymbolTableManager {
    int code[SLOTS];             
    int code_size;               
    int data[SLOTS];             
    int data_size;               

    struct symbol symbols[SLOTS]; 
    int symbols_size;            

    const struct symbol *entries[SLOTS]; 
    int entries_count;           

    struct external externals[SLOTS]; 
    int externals_size;         
};

/* Function to search for a symbol in the program's symbol table */
struct symbol *sym_search_function(struct SymbolTableManager *prog_ptrs, char *name);

/* Function to add a new symbol to the program's symbol table */
void add_symbol(struct SymbolTableManager *prog_ptrs, char *name, 
                enum new_symbol_type symbol_type, int address, int line_of_def,
                int c_number, int data_or_str_size);

/* Function to search for an external symbol by name */
struct external *sym_search_function_external(struct SymbolTableManager *prog_ptrs, char *name);

#endif 
