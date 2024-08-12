#ifndef UTILS_H
#define UTILS_H

extern const char *commands[];
extern int numCommands;
/**
 * Trims leading and trailing whitespace from a string.
 * 
 * @param str The string to trim.
 * @return A pointer to the trimmed string.
 */
char *trimWhitespace(char *str);

/* Function to get the number of commands */
extern int getNumCommands();

/* Function to get a command by index */
extern const char *getCommand(int index);

#endif /* UTILS_H */
