all: final_project

final_project: main.o macro_Handling.o macro_validation.o line_recognizer.o file_generator.o utils.o line_validator.o
	gcc -g -ansi -pedantic -Wall main.o macro_Handling.o macro_validation.o line_recognizer.o file_generator.o utils.o line_validator.o -o final_project

main.o: main.c main.h stages/preProcessor/macro_Handling.h stages/preProcessor/macro_validation.h line_recognizer.h stages/finalStage/file_generator.h stages/utils/utils.h line_validator.h
	gcc -g -ansi -pedantic -Wall -c main.c

macro_Handling.o: stages/preProcessor/macro_Handling.c stages/preProcessor/macro_Handling.h stages/utils/utils.h
	gcc -g -ansi -pedantic -Wall -c stages/preProcessor/macro_Handling.c

macro_validation.o: stages/preProcessor/macro_validation.c stages/preProcessor/macro_validation.h stages/preProcessor/macro_Handling.h stages/utils/utils.h
	gcc -g -ansi -pedantic -Wall -c stages/preProcessor/macro_validation.c

line_recognizer.o: line_recognizer.c line_recognizer.h stages/utils/utils.h
	gcc -g -ansi -pedantic -Wall -c line_recognizer.c

line_validator.o: line_validator.c line_validator.h line_recognizer.h stages/utils/utils.h
	gcc -g -ansi -pedantic -Wall -c line_validator.c

file_generator.o: stages/finalStage/file_generator.c stages/finalStage/file_generator.h
	gcc -g -ansi -pedantic -Wall -c stages/finalStage/file_generator.c

utils.o: stages/utils/utils.c stages/utils/utils.h
	gcc -g -std=c99 -pedantic -Wall -c stages/utils/utils.c

clean:
	rm -f final_project main.o macro_Handling.o macro_validation.o line_recognizer.o utils.o line_validator.o file_generator.o
