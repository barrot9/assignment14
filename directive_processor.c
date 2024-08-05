#include "directive_processor.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/* Processes a directive sentence from the given line */
void processDirective(const char *line) {
    char directive[10];
    const char *parameters;

    /* Extract the directive name */
    sscanf(line, "%s", directive);

    /* Point to parameters after directive */
    parameters = line + strlen(directive);
    
    /* Trim leading whitespace from parameters */
    while (isspace(*parameters)) {
        parameters++;
    }

    /* Determine which directive to process */
    if (strcmp(directive, ".data") == 0) {
        processDataDirective(parameters);
    } else if (strcmp(directive, ".string") == 0) {
        processStringDirective(parameters);
    } else if (strcmp(directive, ".entry") == 0) {
        processEntryDirective(parameters);
    } else if (strcmp(directive, ".extern") == 0) {
        processExternDirective(parameters);
    } else {
        printf("Unknown directive: %s\n", directive);
    }
}

/* Process .data directive */
void processDataDirective(const char *parameters) {
    int value;
    const char *p = parameters;

    /* Parse each integer in the parameter list */
    while (*p) {
        /* Read integer from string */
        if (sscanf(p, "%d", &value) == 1) {
            /* Allocate memory or store value as needed */
            printf("Data value: %d\n", value);
        }

        /* Move to the next integer or delimiter */
        while (*p && *p != ',') {
            p++;
        }

        /* Skip comma delimiter */
        if (*p == ',') {
            p++;
        }

        /* Trim leading whitespace */
        while (isspace(*p)) {
            p++;
        }
    }
}

/* Process .string directive */
void processStringDirective(const char *parameters) {
    const char *start, *end;
    char *string;
    size_t length, i;

    /* Check for starting double quote */
    if (*parameters != '"') {
        printf("Invalid string directive.\n");
        return;
    }

    /* Point to the first character after the opening quote */
    start = parameters + 1;

    /* Find the closing double quote */
    end = strchr(start, '"');
    if (!end) {
        printf("Unterminated string directive.\n");
        return;
    }

    /* Allocate space for string and null terminator */
    length = end - start;
    string = (char *)malloc(length + 1);
    if (!string) {
        printf("Memory allocation failed.\n");
        return;
    }

    /* Copy string content */
    strncpy(string, start, length);
    string[length] = '\0';

    /* Print or store ASCII values and null terminator */
    for (i = 0; i <= length; i++) {
        printf("String char: %d\n", string[i]);
    }

    /* Free allocated memory */
    free(string);
}

/* Process .entry directive */
void processEntryDirective(const char *parameters) {
    char label[50];

    /* Read the label name */
    sscanf(parameters, "%s", label);

    /* Mark label as entry point (implement as needed) */
    printf("Entry label: %s\n", label);
}

/* Process .extern directive */
void processExternDirective(const char *parameters) {
    char label[50];

    /* Read the label name */
    sscanf(parameters, "%s", label);

    /* Mark label as external (implement as needed) */
    printf("Extern label: %s\n", label);
}
