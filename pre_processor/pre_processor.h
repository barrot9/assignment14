/*
 * This header file defines the structures and function prototypes
 * for the preprocessor of our assembly language.
 */

#ifndef PRE_PROCESSOR_H
#define PRE_PROCESSOR_H

/* include of necessary header file */
#include "../utils.h"

/* Enumeration for categorizing different types of lines in input */
typedef enum {
    BLANK_LINE,             
    NORMAL_LINE,            
    DEFINE_MACRO,           
    END_MACRO,              
    CALL_MACRO,           
    ERROR_NO_NAME,          
    ERROR_ALREADY_DEFINED   
} LineCategory;

/* Structure representing a single macro definition */
typedef struct {
    char macroName[MACRO_MAX_SIZE];         
    char macroContent[MAX_LINES][MAX_LENGTH];  
    int lineTotal;              
} MacroDef;

/* Structure for a table of macros, storing all macro definitions */
typedef struct {
    MacroDef macroList[MAX_MAC_FOUND];   
    int macroCount;             
} MacroTableDef;

/* Function declarations */
MacroDef* locate_macro(const MacroTableDef* macroTable, const char* macroName);
LineCategory categorize_line(char* inputLine, MacroTableDef* macroTable, MacroDef** foundMacro);

#endif 
