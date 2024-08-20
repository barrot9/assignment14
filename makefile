all: final_project

final_project: main.o macro_Handling.o macro_validation.o line_recognizer.o utils.o line_validator.o firstStage.o secondStage.o fileGenerator.o
	gcc -g -ansi -pedantic -Wall main.o macro_Handling.o macro_validation.o line_recognizer.o utils.o line_validator.o firstStage.o secondStage.o fileGenerator.o -o final_project

main.o: main.c main.h stages/preProcessor/macro_Handling.h stages/preProcessor/macro_validation.h stages/lineAnalyzer/line_recognizer.h stages/utils/utils.h stages/utils/struct.h stages/lineAnalyzer/line_validator.h 
	gcc -g -ansi -pedantic -Wall -c main.c

macro_Handling.o: stages/preProcessor/macro_Handling.c stages/preProcessor/macro_Handling.h stages/utils/utils.h
	gcc -g -ansi -pedantic -Wall -c stages/preProcessor/macro_Handling.c

macro_validation.o: stages/preProcessor/macro_validation.c stages/preProcessor/macro_validation.h stages/preProcessor/macro_Handling.h stages/utils/utils.h
	gcc -g -ansi -pedantic -Wall -c stages/preProcessor/macro_validation.c

line_recognizer.o: stages/lineAnalyzer/line_recognizer.c stages/lineAnalyzer/line_recognizer.h stages/utils/utils.h
	gcc -g -ansi -pedantic -Wall -c stages/lineAnalyzer/line_recognizer.c

line_validator.o: stages/lineAnalyzer/line_validator.c stages/lineAnalyzer/line_validator.h stages/lineAnalyzer/line_recognizer.h stages/utils/utils.h
	gcc -g -ansi -pedantic -Wall -c stages/lineAnalyzer/line_validator.c

utils.o: stages/utils/utils.c stages/utils/utils.h
	gcc -g -std=c99 -pedantic -Wall -c stages/utils/utils.c

firstStage.o: stages/firstStage/firstStage.c stages/utils/struct.h stages/lineAnalyzer/line_info.h stages/utils/utils.h
	gcc -g -ansi -pedantic -Wall -c stages/firstStage/firstStage.c

secondStage.o: stages/secondStage/secondStage.c stages/utils/struct.h stages/lineAnalyzer/line_info.h stages/utils/utils.h
	gcc -g -ansi -pedantic -Wall -c stages/secondStage/secondStage.c

fileGenerator.o: stages/finalStage/fileGenerator.c stages/utils/struct.h stages/finalStage/fileGenerator.h
	gcc -g -ansi -pedantic -Wall -c stages/finalStage/fileGenerator.c

clean:
	rm -f final_project main.o macro_Handling.o macro_validation.o line_recognizer.o utils.o line_validator.o file_generator.o