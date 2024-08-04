#ifndef ASSIST_H
#define ASSIST_H
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>


#define malloc_strcat(str,catstr) strcat(strcpy(malloc(strlen(str) + strlen(catstr) + 1),str),catstr);

void print_error(char * file_name,const int line_num,char *format,...);
#endif


