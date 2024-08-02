all: final_project

final_project: main.o macro_Handling.o macro_validation.o line_recognizer.o directive_processor.o sentence_processor.o
	gcc -g -ansi -pedantic -Wall main.o macro_Handling.o macro_validation.o line_recognizer.o directive_processor.o sentence_processor.o -o final_project

main.o: main.c main.h macro_Handling.h macro_validation.h line_recognizer.h directive_processor.h sentence_processor.h
	gcc -g -ansi -pedantic -Wall -c main.c

macro_Handling.o: macro_Handling.c macro_Handling.h
	gcc -g -ansi -pedantic -Wall -c macro_Handling.c

macro_validation.o: macro_validation.c macro_validation.h macro_Handling.h
	gcc -g -ansi -pedantic -Wall -c macro_validation.c

line_recognizer.o: line_recognizer.c line_recognizer.h
	gcc -g -ansi -pedantic -Wall -c line_recognizer.c

directive_processor.o: directive_processor.c directive_processor.h
	gcc -g -ansi -pedantic -Wall -c directive_processor.c

sentence_processor.o: sentence_processor.c sentence_processor.h
	gcc -g -ansi -pedantic -Wall -c sentence_processor.c

clean:
	rm -f final_project main.o macro_Handling.o macro_validation.o line_recognizer.o directive_processor.o sentence_processor.o
