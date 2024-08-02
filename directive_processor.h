#ifndef DIRECTIVE_PROCESSOR_H
#define DIRECTIVE_PROCESSOR_H

/* Function prototypes */
void processDirective(const char *line);
void processDataDirective(const char *parameters);
void processStringDirective(const char *parameters);
void processEntryDirective(const char *parameters);
void processExternDirective(const char *parameters);

#endif /* DIRECTIVE_PROCESSOR_H */
