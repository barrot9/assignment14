/*
 * This file contains the main function for the assembler program.
 */

#include "main.h"

/* Main function to iterate over command-line arguments and process each file */
int main(int argc, char **argv) {
    int i;
    for (i = 1; i < argc; i++) {
        process_file(argv[i]);
    }
    return 0;
}
