/*
   This header file defines the data structures and constants used in the program
   for handling symbols and program sections. It provides the structure for the
   symbol table and program data, including code and data sections. This file
   also includes function prototypes for managing symbols and externals.
 */

#ifndef STRUCT_H
#define STRUCT_H

/* Enumeration for different types of symbols */
enum new_symbol_type {
    new_type_code,               /* Code segment symbol */
    new_type_data,               /* Data segment symbol */
    new_type_entry_temporary,    /* Temporary entry symbol */
    new_type_entry_data,         /* Data entry symbol */
    new_type_entry_code,         /* Code entry symbol */
    new_type_external            /* External symbol */
};

/* Structure to represent a symbol in the symbol table */
struct symbol {
    char name[32];                /* Name of the symbol */
    enum new_symbol_type sym_type;/* Type of the symbol */
    int addr;                     /* Address of the symbol */
    int defined_in_line;          /* Line where the symbol is defined */
    int c_num;                    /* Code number (unused or custom purpose) */
    int data_size;                /* Size of data or string */
};

#define SLOTS 4096                /* Address slots size */                

/* Structure to represent an external reference */
struct external {
    char *ext_name;               /* Name of the external symbol */
    int addr[SLOTS];               /* Addresses where the external is used */
    int addr_c;                   /* Count of addresses */
};

/* Structure to represent a program with its code, data, symbols, and externals */
struct SymbolTableManager {
    int code[SLOTS];                /* Array to hold the code section */
    int code_size;                 /* Size of the code section */
    int data[SLOTS];                /* Array to hold the data section */
    int data_size;                 /* Size of the data section */

    struct symbol symbols[SLOTS];   /* Array to hold the symbols */
    int symbols_size;              /* Number of symbols */

    const struct symbol *entries[SLOTS]; /* Array of pointers to entry symbols */
    int entries_count;             /* Count of entry symbols */

    struct external externals[SLOTS]; /* Array to hold external references */
    int externals_size;            /* Number of external references */
};

/* Function to search for a symbol in the program's symbol table */
struct symbol *sym_search_function(struct SymbolTableManager *symbolManager, char *name);

/* Function to add a symbol to the program's symbol table */
void add_symbol(struct SymbolTableManager *symbolManager, char *name, 
                enum new_symbol_type symbol_type,
                int address, int line_of_def, int c_number, int data_or_str_size);

/* Function to search for an external reference in the program */
struct external *sym_search_function_external(struct SymbolTableManager *symbolManager, char *name);

#endif /* STRUCT_H */
