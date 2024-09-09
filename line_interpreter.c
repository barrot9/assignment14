/*
 * This file implements the line interpreter for our assembly language processor.
 * It provides functionality to analyze and interpret individual lines of assembly code.
 * This module providing detailed analysis of each assembly instruction and preparing 
 * the data for the next stages of the assembly.
 */

/* Included necesarry header file. */
#include "line_interpreter.h"

/* Global variable to store the current line being analyzed */
struct analized_line current_line = {0};

/* Structure to hold tokens from a string split by spaces */
struct StringTokens {
    char *string_tokens[80];
    int token_count;
};

/* Structure to hold information about directives */
struct DirectiveInfo {
    char * directive_name;
    directive_type directive_code;
};

/* Array of supported directives */
struct DirectiveInfo directives_list[NUMBER_OF_DIRECTIVES] = {
    {".string", directive_string},
    {".data", directive_data},
    {".extern", directive_extern},
    {".entry", directive_entry},
};

/* Structure to hold information about opcodes */
struct OpcodeInfo {
    char * opcode_name;
    enum opcode opcode;
};

/* Array of supported opcodes */
struct OpcodeInfo opcode_table[NUMBER_OF_OPCODES] = {
    {"mov",opcode_mov}, {"cmp",opcode_cmp}, {"add",opcode_add}, {"sub",opcode_sub},
    {"lea",opcode_lea}, {"clr",opcode_clr}, {"not",opcode_not}, {"inc",opcode_inc},
    {"dec",opcode_dec}, {"jmp",opcode_jmp}, {"bne",opcode_bne}, {"red",opcode_red},
    {"prn",opcode_prn}, {"jsr",opcode_jsr}, {"rts",opcode_rts}, {"stop",opcode_stop},
};

/* Structure to hold information about operands for each opcode */
struct OperandInfo {
    char * source_operand;
    char * destination_operand;
};

/* Array of operand information for each opcode */
struct OperandInfo operand_table[NUMBER_OF_OPCODES] = {
    {"0123","123"}, {"0123","0123"}, {"0123","123"}, {"0123","123"},
    {"1","123"}, {NULL,"123"}, {NULL,"123"}, {NULL,"123"},
    {NULL,"123"}, {NULL,"12"}, {NULL,"12"}, {NULL,"123"},
    {NULL,"0123"}, {NULL,"12"}, {NULL,NULL}, {NULL,NULL},
};

/* Main function to analyze an assembly line */
int analyze_assembly_line(char *assembly_line) {
    struct OpcodeInfo *instruction;
    struct DirectiveInfo *DirectiveInfo;
    struct StringTokens space_tokens;
    static char line_copy[82] = {0};
    char *label_pos, *equal_pos, *text_pos;
    char **current_token;

    /* Initialize current_line structure and set default operand types to 'none' */
    memset(&current_line, 0, sizeof(current_line));
    current_line.operand_type[0] = none;
    current_line.operand_type[1] = none;

    /* Skip leading whitespace characters */
    while (isspace(*assembly_line)) assembly_line++;

    /* Remove trailing newline characters */
    assembly_line[strcspn(assembly_line, "\r\n")] = 0; 

    /* Make a copy of the assembly line for manipulation */
    strcpy(line_copy, assembly_line);

    /* Split the line by spaces */
    space_tokens = split_by_spaces(assembly_line);
    current_token = &space_tokens.string_tokens[0];

    /* Check if the first token is a label (contains ':') */
    label_pos = strchr(*current_token, ':');

    if (label_pos) {
        /* Validate and process the label */
        *label_pos = '\0'; /* Terminate the string at ':' */
        label_pos++;

        if (*label_pos != '\0') {
            label_pos--;
            *label_pos = ':'; /* Restore ':' if invalid label */
            sprintf(current_line.error, "Error: invalid label:'%s'.", *current_token);
            return ERROR;
        }

        /* Check if the label is valid */
        if (validate_label(*current_token, 0) != 0) {
            label_pos--;
            *label_pos = ':'; /* Restore ':' if invalid label */
            sprintf(current_line.error, "Error: invalid label:'%s'.", *current_token);
            return ERROR;
        }

        /* Assign the label to current_line */
        current_line.label_name = *current_token;
        current_token++;
    }

    /* If no tokens left after processing the label, report empty line */
    if (*current_token == NULL) {
        sprintf(current_line.error, "Empty line");
        return 0;  
    }

    /* Check if the current token is an instruction */
    instruction = find_instruction_by_name(*current_token);
    if (instruction) {
        struct OperandInfo *operand_info = &operand_table[instruction - opcode_table];
        current_line.line_type = code_line;
        current_token++;

        /* Check for missing or unexpected operands */
        if (*current_token == NULL && operand_info->destination_operand != NULL) {
            sprintf(current_line.error, "Error: missing operands");
            return ERROR;
        }
        if (*current_token && operand_info->destination_operand == NULL) {
            sprintf(current_line.error, "Error: extra operands");
            return ERROR;
        }

        /* Parse operands if present */
        if (*current_token) {
            label_pos = &line_copy[0];
            if (pre_analyze_operand(label_pos + ((*current_token) - space_tokens.string_tokens[0]), instruction, operand_info) != 0) {
                return ERROR;  
            }
        }

        /* Set the opcode of the instruction */
        current_line.opcode = instruction->opcode;
    } else {
        /* If not an instruction, check if it's a directive */
        DirectiveInfo = find_directive_by_name(*current_token);
        if (DirectiveInfo) {
            current_token++;

            /* Check for missing operands */
            if (*current_token == NULL) {
                sprintf(current_line.error, "Error: missing operands");
                return ERROR;
            }

            /* Process the directive with its operands */
            label_pos = &line_copy[0];
            if (process_directive(label_pos + ((*current_token) - space_tokens.string_tokens[0]), DirectiveInfo) != 0) {
                return ERROR; 
            }
            current_line.line_type = directive_line;
        } else {
            /* Check for a .define statement */
            if (strcmp(".define", *current_token) == 0) {
                current_token++;
                label_pos = &line_copy[0] + ((*current_token) - space_tokens.string_tokens[0]);
                equal_pos = strchr(label_pos, '=');

                if (equal_pos) {
                    *equal_pos = '\0'; /* Terminate string at '=' */
                    text_pos = strpbrk(label_pos, WHITESPACE);

                    if (text_pos) *text_pos = '\0'; /* Remove trailing spaces from label */

                    equal_pos++;
                    while (isspace(*equal_pos)) equal_pos++; /* Skip leading spaces */

                    text_pos = strpbrk(equal_pos, WHITESPACE);
                    if (text_pos) {
                        *text_pos = '\0'; /* Terminate string at first space */
                        text_pos++;
                        while (isspace(*text_pos)) text_pos++; /* Skip leading spaces */

                        if (*text_pos != '\0') {
                            /* If extra text is found, report an error */
                            sprintf(current_line.error, "Error: extra text:'%s'", text_pos);
                            return ERROR;
                        }
                    }

                    /* Validate label and number for .define */
                    if (validate_label(label_pos, 0) == 0) {
                        int validation_result = validate_number(equal_pos, &current_line.definition_count, MIN_NUM_RANGE, MAX_NUM_RANGE);
                        current_line.line_type = definition_line;
                        current_line.definition_label = label_pos;

                        if (validation_result == 0) {
                            /* Valid number, continue processing */
                        } else if (validation_result == INVALID_VALUE) {
                            sprintf(current_line.error, "Error: invalid number format: '%s'", equal_pos);
                            return ERROR;
                        } else if (validation_result == OUT_OF_RANGE) {
                            sprintf(current_line.error, "Error: number out of range (-2048 to 2047): '%s'", equal_pos);
                            return ERROR;
                        } else {
                            sprintf(current_line.error, "Unexpected error while processing: '%s'", equal_pos);
                            return ERROR;
                        }
                    } else {
                        sprintf(current_line.error, "Error: invalid label or no label found:'%s'", label_pos);
                        return ERROR;
                    }
                } else {
                    /* Report error if '=' is missing */
                    sprintf(current_line.error, "Error: '=' was not found.");
                    return ERROR;
                }
            } else {
                /* Report error if the token is an undefined keyword */
                sprintf(current_line.error, "Error: keyword:'%s' not recognized", *current_token);
                return ERROR;
            }
        }
    }

    return 0;  
}

/* Function to validate a label */
int validate_label(char *label_name, int brackets) {
    int label_length = 0;
    /* Check if the first character is a letter */
    if (!isalpha(*label_name)) {
        return INVALID_VALUE;
    }
    label_name++; 
    label_length++;
    /* Check remaining characters */
    while (*label_name) {
        /* If character is not alphanumeric and brackets are not allowed, return -1 */
        if ((!isalnum(*label_name)) && brackets == 0) {
            return INVALID_VALUE;
        }
        label_length++;
        label_name++;
    }
    /* Check if label exceeds maximum allowed length */
    if (label_length > 31) {
        return OUT_OF_RANGE;
    }
    return 0; /* Label is valid */
}

/* Function to split a string into tokens based on whitespace */
struct StringTokens split_by_spaces(char *input_string) {
    int token_count = 0;
    char *current_pos;
    struct StringTokens tokens = {0};

    /* Skip leading whitespace */
    while (isspace(*input_string)) input_string++;

    /* If the string is empty after removing leading whitespace, return */
    if (*input_string == '\0') {
        return tokens;
    }

    /* Loop to tokenize the string by spaces */
    while (1) {
        tokens.string_tokens[token_count++] = input_string; /* Store the start of the token */
        current_pos = strpbrk(input_string, WHITESPACE); /* Find the next space character */

        if (current_pos) {
            *current_pos = '\0'; /* Terminate the current token */
            current_pos++;

            /* Skip any additional spaces */
            while (isspace(*current_pos)) current_pos++;

            /* If the end of the string is reached, break the loop */
            if (*current_pos == '\0') {
                break;
            }

            /* Move input_string to the start of the next token */
            input_string = current_pos;
        } else {
            /* No more spaces found, break the loop */
            break;
        }
    }

    tokens.token_count = token_count; /* Set the number of tokens found */
    return tokens;
}

/* Function to find an instruction by its name */
struct OpcodeInfo *find_instruction_by_name(char *instruction_name) {
    int i;

    /* Loop through the opcode_table array to find a matching instruction name */
    for (i = 0; i < NUMBER_OF_OPCODES; i++) {
        /* Compare the input instruction name with the current instruction's name */
        if (strcmp(instruction_name, opcode_table[i].opcode_name) == 0) {
            /* Return a pointer to the matching instruction */
            return &opcode_table[i];
        }
    }

    /* If no matching instruction is found, return NULL */
    return NULL;
}

/* Function to find a directive by its name */
struct DirectiveInfo *find_directive_by_name(char *directive_name) {
    int i;
    /* Loop through the directives array to find a matching directive name */
    for (i = 0; i < NUMBER_OF_DIRECTIVES; i++) {
        /* Compare the input directive name with the current directive's name */
        if (strcmp(directives_list[i].directive_name, directive_name) == 0) {
            /* Return a pointer to the matching directive */
            return &directives_list[i];
        }
    }

    /* If no matching directive is found, return NULL */
    return NULL;
}

/* Function to validate a number within a given range */
int validate_number(char *number_str, int *value, int min_value, int max_value) {
    char *end_ptr;
    /* Convert the string to an integer and store the end pointer */
    *value = strtol(number_str, &end_ptr, 10);
    /* Check if the conversion failed because no digits were found */
    if (end_ptr == number_str) {
        return INVALID_VALUE;
    }
    /* Check if there are extra characters after the number */
    if (*end_ptr != '\0') {
        return INVALID_VALUE;
    }
    /* Check if the value is outside the acceptable range */
    if (*value > max_value || *value < min_value) {
        return OUT_OF_RANGE;
    }
    /* The number is valid and within the range */
    return 0;
}

/* Function to analyze an operand */
int analyze_operand(char *operand_text, struct OpcodeInfo *instruction, struct OperandInfo *operand_info, int operand_index) {
    /* Skip leading whitespace */
    while (isspace(*operand_text)) operand_text++;

    if (check_for_extra_text(operand_text) != 0) {
        /* Error already set in check_for_extra_text */
        return ERROR;
    }

    if (is_valid_register(operand_text)) {
        if (analyze_direct_register(operand_text, operand_index) != 0) {
            /* Error already set in analyze_direct_register */
            return ERROR;
        }
    } else if (*operand_text == INDIRECT_REGISTER && is_valid_register(operand_text + 1)) {
        if (analyze_indirect_register(operand_text, operand_index) != 0) {
            /* Error already set in analyze_indirect_register */
            return ERROR;
        }
    } else if (*operand_text == IMMEDIATE_VAL) {
        if (analyze_immediate_value(operand_text, operand_index) != 0) {
            /* Error already set in analyze_immediate_value */
            return ERROR;
        }
    } else {
        if (analyze_label(operand_text, operand_index) != 0) {
            /* Error already set in analyze_label */
            return ERROR;
        }
    }

    /* Successful analysis of operand */
    return 0;
}

/* function to check if the operand is a valid register */
int is_valid_register(const char *operand) {
    if (*operand != REGISTER_START) return 0;
    if (strlen(operand) != 2) return 0;
    if (operand[1] < '0' || operand[1] > '7') return 0;
    return 1;
}

/* Function to check for extra text after an operand */
int check_for_extra_text(char *operand_text) {
    char *extra_text_pos = strpbrk(operand_text, WHITESPACE);
    if (extra_text_pos) {
        *extra_text_pos = '\0';
        extra_text_pos++;
        while (isspace(*extra_text_pos)) extra_text_pos++;
        if (*extra_text_pos != '\0') {
            sprintf(current_line.error, "Error: extra text:'%s'", extra_text_pos);
            return 1;
        }
    }
    return 0;
}

/* Function to analyze a direct register operand */
int analyze_direct_register(char *operand_text, int operand_index) {
    int reg_number;
    int validation_result = validate_number(operand_text + 1, &reg_number, 0, 7);
    if (validation_result == 0) {
        current_line.operand_type[operand_index] = direct_register;
        current_line.operand_list[operand_index].register_num = reg_number;
        return 0;
    } else if (validation_result == INVALID_VALUE) {
        sprintf(current_line.error, "Error: invalid register format: '%s'", operand_text + 1);
    } else if (validation_result == OUT_OF_RANGE) {
        sprintf(current_line.error, "Error: register number out of range (0 to 7): '%s'", operand_text + 1);
    } else {
        sprintf(current_line.error, "Error: unexpected error while validating register: '%s'", operand_text + 1);
    }
    return 1;
}

/* Function to analyze an indirect register operand */
int analyze_indirect_register(char *operand_text, int operand_index) {
    int reg_number;
    int validation_result = validate_number(operand_text + 2, &reg_number, 0, 7);
    if (validation_result == 0) {
        current_line.operand_type[operand_index] = indirect_register;
        current_line.operand_list[operand_index].register_num = reg_number;
        return 0;
    } else if (validation_result == INVALID_VALUE) {
        sprintf(current_line.error, "Error: invalid indirect register format: '%s'", operand_text + 2);
    } else if (validation_result == OUT_OF_RANGE) {
        sprintf(current_line.error, "Error: indirect register number out of range (0 to 7): '%s'", operand_text + 2);
    } else {
        sprintf(current_line.error, "Unexpected error while processing: '%s'", operand_text + 2);
    }
    return 1;
}

/* Function to analyze an immediate value operand */
int analyze_immediate_value(char *operand_text, int operand_index) {
    int immed_value;
    int validation_result = validate_number(operand_text + 1, &immed_value, MIN_NUM_RANGE, MAX_NUM_RANGE);
    if (validation_result == 0) {
        current_line.operand_type[operand_index] = immediate;
        current_line.operand_list[operand_index].immediate_value = immed_value;
        return 0;
    } else if (validation_result == INVALID_VALUE) {
        sprintf(current_line.error, "Error: invalid immediate value format: '%s'", operand_text + 1);
    } else if (validation_result == OUT_OF_RANGE) {
        sprintf(current_line.error, "Error: immediate value out of range (-2048 to 2047): '%s'", operand_text + 1);
    } else {
        sprintf(current_line.error, "Unexpected error while validating: '%s'", operand_text + 1);
    }
    return 1;
}

/* Function to analyze a label operand */
int analyze_label(char *operand_text, int operand_index) {
    int label_check = validate_label(operand_text, 0);
    if (label_check == 0) {
        current_line.operand_type[operand_index] = label;
        current_line.operand_list[operand_index].label_name = operand_text;
        return 0;
    } else if (label_check == INVALID_VALUE) {
        sprintf(current_line.error, "Error: invalid operand:'%s'", operand_text);
    } else if (label_check == OUT_OF_RANGE) {
        sprintf(current_line.error, "Error: invalid label (too long):'%s'", operand_text);
    }
    return 1;
}

/* Function to pre-analyze operands */
int pre_analyze_operand(char *operands_text, struct OpcodeInfo *instruction, struct OperandInfo *operand_info) {
    char *first_operand, *second_operand;
    int operand_count;

    operand_count = split_operands(operands_text, &first_operand, &second_operand);

    if (!validate_operand_count(operand_count, operand_info)) {
        /* Error: Invalid operand count */
        return ERROR;
    }

    if (operand_count >= 1) {
        if (analyze_operand(first_operand, instruction, operand_info, operand_count == 1 ? 1 : 0) != 0) {
            /* Error occurred in first operand */
            return ERROR;
        }
    }

    if (operand_count == 2) {
        if (analyze_operand(second_operand, instruction, operand_info, 1) != 0) {
            /* Error occurred in second operand */
            return ERROR;
        }
    }

    /* Success: All operands analyzed without errors */
    return 0;
}
/* Function to split operands into separate strings */
int split_operands(char *operands_text, char **first_operand, char **second_operand) {
    char *comma_pos = strchr(operands_text, ',');

    if (comma_pos) {
        *comma_pos = '\0';
        *first_operand = operands_text;
        *second_operand = comma_pos + 1;

        /* Trim leading whitespace from second operand */
        while (isspace(**second_operand)) (*second_operand)++;

        return 2; /* Two operands found */
    } else {
        *first_operand = operands_text;
        *second_operand = NULL;
        return 1; /* One operand found */
    }
}

/* Function to validate the number of operands */
int validate_operand_count(int operand_count, struct OperandInfo *operand_info) {
    if (operand_count == 2 && operand_info->source_operand == NULL) {
        sprintf(current_line.error, "Error: second operand found");
        return 0;
    }
    if (operand_count == 1 && operand_info->source_operand != NULL) {
        sprintf(current_line.error, "Error: second operand not found");
        return 0;
    }
    return 1;
}

/* Function to process a directive */
int process_directive(char *directive_text, struct DirectiveInfo *directive_info) {
    current_line.directive_type = directive_info->directive_code;

    if (directive_info->directive_code == directive_entry || directive_info->directive_code == directive_extern) {
        if (!process_entry_extern(directive_text)) {
            return ERROR; /* Error in processing entry or extern directive */
        }
    } else if (directive_info->directive_code == directive_string) {
        if (!process_string(directive_text)) {
            return ERROR; /* Error in processing string directive */
        }
    } else if (directive_info->directive_code == directive_data) {
        if (!process_data(directive_text)) {
            return ERROR; /* Error in processing data directive */
        }
    } else {
        sprintf(current_line.error, "Error: unknown directive type");
        return ERROR; /* Unknown directive type */
    }

    return 0; /* Success */
}

/* Function to process .entry or .extern directives */
int process_entry_extern(char *directive_text) {
    char *delimiter_pos;

    delimiter_pos = strpbrk(directive_text, WHITESPACE);
    if (delimiter_pos) {
        *delimiter_pos = '\0';
        delimiter_pos++;
        while (isspace(*delimiter_pos)) delimiter_pos++;
        if (*delimiter_pos != '\0') {
            sprintf(current_line.error, "Error: extra text:'%s'", delimiter_pos);
            return 0;
        }
    }
    if (validate_label(directive_text, 0) == 0) {
        current_line.directive_label = directive_text;
        return 1;
    } else {
        sprintf(current_line.error, "Error: invalid label: '%s'", directive_text);
        return 0;
    }
}

/* Function to process a .string directive */
int process_string(char *directive_text) {
    char *quote_pos;

    if (*directive_text != '"') {
        sprintf(current_line.error, "Error: string didn't start correctly");
        return 0;
    }

    quote_pos = strrchr(directive_text + 1, '"');
    if (directive_text == quote_pos) {
        sprintf(current_line.error, "Error: string didn't terminate correctly");
        return 0;
    }

    *quote_pos = '\0';
    quote_pos++;
    while (isspace(*quote_pos)) quote_pos++;
    if (*quote_pos != '\0') {
        sprintf(current_line.error, "Error: extra text");
        return 0;
    }

    current_line.directive_string = directive_text + 1;
    return 1;
}

/* Function to process a .data directive */
int process_data(char *directive_text) {
    char *token, *saveptr;
    char *end;
    int validation_result;
    int is_first_token = 1;

    token = strtok_r(directive_text, D_COMMA, &saveptr);
    while (token != NULL) {
        /* Check for consecutive commas */
        if (!is_first_token && token[0] == '\0') {
            sprintf(current_line.error, "Error: two commas in a row");
            return 0;
        }
        is_first_token = 0;

        /* Trim leading and trailing whitespace */
        while (isspace(*token)) token++;
        end = token + strlen(token) - 1;
        while (end > token && isspace(*end)) end--;
        *(end + 1) = '\0';

        if (*token == '\0') {
            sprintf(current_line.error, "Error: empty data value");
            return 0;
        }

        validation_result = validate_number(token, &current_line.data_value[current_line.data_size], MIN_DATA_RANGE, MAX_DATA_RANGE);
        if (validation_result == 0) {
            current_line.data_size++;
        } else if (validation_result == INVALID_VALUE) {
            sprintf(current_line.error, "Error: invalid data format: '%s'", token);
            return 0;
        } else if (validation_result == OUT_OF_RANGE) {
            sprintf(current_line.error, "Error: data value out of range (-8192 to 8191): '%s'", token);
            return 0;
        } else {
            sprintf(current_line.error, "Error: unexpected error while validating data: '%s'", token);
            return 0;
        }

        token = strtok_r(NULL, D_COMMA, &saveptr);
    }

    return 1;
}