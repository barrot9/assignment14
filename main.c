#include "main.h"
#include "macro_Handling.h"
#include "macro_validation.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    /* Validate macros first */
    validateMacros(argv[1]);

    /* If validation passes, process the file */
    processFile(argv[1]);

    return 0;
}
