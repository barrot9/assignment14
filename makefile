all: final_project

final_project: main.o macro_Handling.o macro_validation.o line_recognizer.o directive_processor.o file_generator.o utils.o
	gcc -g -ansi -pedantic -Wall main.o macro_Handling.o macro_validation.o line_recognizer.o directive_processor.o file_generator.o utils.o -o final_project

main.o: main.c main.h macro_Handling.h macro_validation.h line_recognizer.h directive_processor.h file_generator.h utils.h
	gcc -g -ansi -pedantic -Wall -c main.c

macro_Handling.o: macro_Handling.c macro_Handling.h utils.h
	gcc -g -ansi -pedantic -Wall -c macro_Handling.c

macro_validation.o: macro_validation.c macro_validation.h macro_Handling.h utils.h
	gcc -g -ansi -pedantic -Wall -c macro_validation.c

line_recognizer.o: line_recognizer.c line_recognizer.h 
	gcc -g -ansi -pedantic -Wall -c line_recognizer.c

directive_processor.o: directive_processor.c directive_processor.h
	gcc -g -ansi -pedantic -Wall -c directive_processor.c

file_generator.o: file_generator.c file_generator.h
	gcc -g -ansi -pedantic -Wall -c file_generator.c

utils.o: utils.c utils.h
	gcc -g -std=c99 -pedantic -Wall -c utils.c

clean:
	rm -f final_project main.o macro_Handling.o macro_validation.o line_recognizer.o directive_processor.o utils.o file_generator.o
