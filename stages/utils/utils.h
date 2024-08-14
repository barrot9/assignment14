#ifndef UTILS_H
#define UTILS_H
#include <stdbool.h>

extern const char *commands[];
extern int numCommands;

bool isReservedWord(const char *word);

/* Trims leading and trailing whitespace from a string. */
char *trimWhitespace(char *str);

/* Function to get the number of commands */
extern int getNumCommands();

/* Function to get a command by index */
extern const char *getCommand(int index);

#endif /* UTILS_H */
