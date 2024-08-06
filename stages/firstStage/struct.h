/*
   This header defines the data structures and constants for managing symbols
   and sections in the assembler. It includes structures for symbols, program data,
   and external references, along with function prototypes for symbol management.
*/

#ifndef STRUCT_H
#define STRUCT_H

/* Enumeration for symbol types in the assembly program */
enum new_symbol_type {
    new_type_code,               /* Symbol in the code section */
    new_type_data,               /* Symbol in the data section */
    new_type_entry_temporary,    /* Temporary entry symbol */
    new_type_entry_data,         /* Data entry symbol */
    new_type_entry_code,         /* Code entry symbol */
    new_type_external            /* External symbol */
};

/* Represents a symbol in the symbol table */
struct symbol {
    char name[32];               /* Symbol name */
    enum new_symbol_type sym_type; /* Symbol type */
    int addr;                    /* Symbol address */
    int defined_in_line;         /* Line where symbol is defined */
    int c_num;                   /* Custom code number */
    int data_size;               /* Size of data or string associated with symbol */
};

#define SLOTS 4096                /* Maximum size for various arrays */

/* Represents an external reference in the assembly code */
struct external {
    char *ext_name;              /* Name of the external symbol */
    int addr[SLOTS];             /* Addresses where the external is referenced */
    int addr_c;                  /* Count of address references */
};

/* Structure to manage code, data, symbols, and externals */
struct SymbolTableManager {
    int code[SLOTS];             /* Code section */
    int code_size;               /* Size of the code section */
    int data[SLOTS];             /* Data section */
    int data_size;               /* Size of the data section */

    struct symbol symbols[SLOTS]; /* Symbol table */
    int symbols_size;            /* Number of symbols in the table */

    const struct symbol *entries[SLOTS]; /* Entry symbols array */
    int entries_count;           /* Number of entry symbols */

    struct external externals[SLOTS]; /* External references array */
    int externals_size;          /* Number of external references */
};

/* Searches for a symbol in the program's symbol table */
struct symbol *sym_search_function(struct SymbolTableManager *symbolManager, char *name);

/* Adds a symbol to the program's symbol table */
void add_symbol(struct SymbolTableManager *symbolManager, char *name, 
                enum new_symbol_type symbol_type,
                int address, int line_of_def, int c_number, int data_or_str_size);

/* Searches for an external reference in the program */
struct external *sym_search_function_external(struct SymbolTableManager *symbolManager, char *name);

#endif /* STRUCT_H */
