#include "utils.h"
#include "struct.h"
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
/* Array of supported commands */
const char *commands[] = {
    "mov", "cmp", "add", "sub", "lea", "clr", "not", "inc",
    "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop"
};

/* Number of supported commands */
int numCommands = sizeof(commands) / sizeof(commands[0]);

const char *reservedWords[] = {
    "mov", "cmp", "add", "sub", "lea", 
    "clr", "not", "inc", "dec", "jmp", 
    "bne", "red", "prn", "jsr", "rts", "stop", 
    ".data", ".string", ".entry", ".extern"
};
const int numReservedWords = sizeof(reservedWords) / sizeof(reservedWords[0]);

bool isReservedWord(const char *word) {
    
    for (int i = 0; i < numReservedWords; i++) {
        if (strcmp(word, reservedWords[i]) == 0) {
            return true;
        }
    }
    return false;
}

/* Implementation of trimWhitespace function */
char *trimWhitespace(char *str) {
    char *end;

    /* Trim leading space */
    while (isspace((unsigned char)*str)) str++;

    if (*str == 0)  /* All spaces? */
        return str;

    /* Trim trailing space */
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    /* Write new null terminator */
    end[1] = '\0';

    return str;
}


/* Function to get the number of commands */
int getNumCommands() {
    return numCommands;
}

/* Function to get a command by index */
const char *getCommand(int index) {
    if (index >= 0 && index < numCommands) {
        return commands[index];
    }
    return NULL;
}

/* new implemented functions will add comments later */
struct symbol * sym_search_function(struct SymbolTableManager * prog_ptrs,char * name) {
    int i;
    for(i=0;i<prog_ptrs->symbols_size;i++) {
        if(strcmp(prog_ptrs->symbols[i].name,name) == 0) {
            return &prog_ptrs->symbols[i];
        }
    }
    return NULL;
}
void add_symbol(struct SymbolTableManager * prog_ptrs, char *name, 
                        enum new_symbol_type  symbol_type,
                        int address,int line_of_def,int c_number,int data_or_str_size) {
    strcpy(prog_ptrs->symbols[prog_ptrs->symbols_size].name,name);prog_ptrs->symbols[prog_ptrs->symbols_size].sym_type = symbol_type;prog_ptrs->symbols[prog_ptrs->symbols_size].addr = address;
    prog_ptrs->symbols[prog_ptrs->symbols_size].c_num = c_number;prog_ptrs->symbols[prog_ptrs->symbols_size].defined_in_line =line_of_def;prog_ptrs->symbols[prog_ptrs->symbols_size].data_size = data_or_str_size;
    prog_ptrs->symbols_size++;
}
struct external * sym_search_function_external(struct SymbolTableManager * prog_ptrs,char * name) {
int i;
for(i=0; i < prog_ptrs->externals_size;i++) {
    if(strcmp(name,prog_ptrs->externals[i].ext_name) == 0) {
        return &prog_ptrs->externals[i];
    }
}
return NULL;
}
