/*
 * This header file provides essential utilities, structures, and constants
 * for the assembler implementation. It serves as a central point of definition
 * for components used across various stages of the assembly process.
 * This header is crucial for maintaining consistency across the assembler's modules,
 * providing a shared set of definitions and interfaces for symbol management,
 * file handling, and core assembly operations.
 */

#ifndef UTILS_H
#define UTILS_H

/* Included necessary libraries */
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

/* Global definition used across the entire process */
#define WHITESPACE  " \t\f\r\v"
#define ERROR -1
#define NONE 0
#define INVALID_VALUE -1
#define OUT_OF_RANGE -2
#define IMMEDIATE 1
#define LABEL 2
#define INDIRECT 3
#define DIRECT 4
#define MAX_ERROR_LENGTH 300
#define MAX_DATA_VALUE_LENGTH 100
#define MAX_CODE_SIZE 1000
#define MAX_DATA_SIZE 1000
#define MAX_SYMBOLS 100
#define MAX_EXTERNALS 100
#define MAX_EXTERNAL_ADDRESSES 100
#define NUMBER_OF_DIRECTIVES 4
#define NUMBER_OF_OPCODES 16
#define MIN_NUM_RANGE -2048
#define MAX_NUM_RANGE 2047
#define MIN_DATA_RANGE -8192
#define MAX_DATA_RANGE 8191
#define MAX_SIZE 4096  
#define MAX_LINES 10
#define MAX_LENGTH 82
#define MAX_MAC_FOUND 1000
#define INIT_ADDRESS 100  
#define MACRO_MAX_SIZE 32
#define MODE_WRITE "w"
#define MODE_READ "r"
#define OBJ_FILE_TYPE ".ob"
#define ENT_FILE_TYPE ".ent"
#define EXT_FILE_TYPE ".ext"
#define INPUT_FILE_EXT ".as"
#define UNPACKED_FILE_EXT ".am"
#define COMMENT_PREFIX ';'
#define DOUBLE_QUOTE '"'
#define COMMA ','
#define D_COMMA ","
#define REGISTER_START 'r'
#define INDIRECT_REGISTER '*'
#define IMMEDIATE_VAL '#'
#define OUTPUT_FILE_DIR "output_files"
#define MAX_PATH_LENGTH 256
#define INPUT_FILES_PREFIX_1 "input_files\\"
#define INPUT_FILES_PREFIX_2 "input_files/"

/* Enum to define different types of symbols */
enum Symbol {
    Symbol_code,          
    Symbol_data,          
    temp_entry_symbol,    
    entry_symbol_data,    
    entry_symbol_code,    
    external_symbol       
};

/* Structure representing a symbol in the symbol table */
struct symbols_table  {
    char symbol_name[MAX_LENGTH];    
    enum Symbol symbol_type;       
    int symbol_address;            
    int line_number;                
    int constant_value;             
    int data_size;                 
};

/* Structure representing an external symbol and its addresses */
struct external_symbols_table {
    char * external_symbol_name;               
    int external_symbol_addresses[MAX_SIZE];    
    int address_count;                        
};

/* Structure representing the entire assembly unit, including code, data, symbols, and entries */
struct AssemblyUnit {
    int code[MAX_SIZE];           
    int code_size;                 
    int data[MAX_SIZE];          
    int data_size;               
    struct symbols_table symbols[MAX_SIZE]; 
    int symbols_size;                      
    const struct symbols_table *entries[MAX_SIZE];  
    int entries_count;                              
    struct external_symbols_table externals[MAX_SIZE];  
    int externals_size;                               
};

/* Function prototypes */
char* preProcessor(const char* inputFilename);
int firstStage(struct AssemblyUnit* unit, FILE *AMFILE, char *AMFILENAME);
int secondStage(struct AssemblyUnit* unit, FILE* AMFILE, char *AMFILENAME);
int create_entry_file(const struct symbols_table * const items[], const int size_items, char *name_b);
int create_external_file(const struct external_symbols_table *params, const int size_params, char *name_b);
int create_object_file(const int *code, const int code_size, const int *data, const int data_size, char *origin_name);
void add_symbol(struct AssemblyUnit *unit, char *symbol_name, enum Symbol type, int address, int line_number, int const_value, int data_size);
void update_symbol(struct symbols_table *symbol, int line_counter, int address, enum Symbol type);
void update_entry_symbol(struct symbols_table *symbol, const char *file_name, int line_counter);
void adjust_symbol_address(struct symbols_table *symbol, int instruction_counter);
void add_to_entries(struct AssemblyUnit *Unit, struct symbols_table *symbol);
struct symbols_table * search_symbol(struct AssemblyUnit * unit, char * name);
struct external_symbols_table * search_external_symbol(struct AssemblyUnit * unit, char * name);
const char* stripInputFilesPrefix(const char* filename);
char* getFilePath(const char* dir, const char* filename, const char* extension);
FILE* createFile(const char* filepath);
int writeInstruction(FILE* file, int address, int instruction);

#endif
