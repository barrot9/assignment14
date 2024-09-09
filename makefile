# Build command
all: assembler

# Program link
assembler: main.o firstStage.o secondStage.o line_interpreter.o fileGenerator.o utils.o pre_processor.o 
	gcc -ansi -g  -Wall -pedantic  main.o pre_processor.o firstStage.o secondStage.o line_interpreter.o fileGenerator.o utils.o -o assembler

<<<<<<< HEAD
# Main rule
main.o: main.c main.h
	gcc -ansi -g  -pedantic -Wall -c  main.c -o main.o
=======
main.o: main.c main.h stages/preProcessor/macro_Handling.h stages/preProcessor/macro_validation.h stages/lineAnalyzer/line_recognizer.h stages/utils/utils.h stages/utils/struct.h stages/lineAnalyzer/line_validator.h 
	gcc -g -ansi -pedantic -Wall -c main.c
>>>>>>> 389b960335faa1dd26beee952d443d9d0b53411c

# Utility rules
pre_processor.o: pre_processor/pre_processor.c pre_processor/pre_processor.h
	gcc -ansi -g  -pedantic  -Wall -c  pre_processor/pre_processor.c -o pre_processor.o

firstStage.o: first_stage/firstStage.c first_stage/firstStage.h 
	gcc -ansi -g  -pedantic -Wall -c  first_stage/firstStage.c -o firstStage.o

secondStage.o: second_stage/secondStage.c second_stage/secondStage.h 
	gcc -ansi -g  -pedantic -Wall -c  second_stage/secondStage.c -o secondStage.o

line_interpreter.o: line_interpreter.c line_interpreter.h
	gcc -ansi -g  -pedantic -Wall -c  line_interpreter.c -o line_interpreter.o

fileGenerator.o: fileGenerator.c  fileGenerator.h
	gcc -ansi -g  -pedantic -Wall -c  fileGenerator.c -o fileGenerator.o

utils.o: utils.c utils.h
	gcc -ansi -g  -pedantic -Wall -c  utils.c -o utils.o

# Extra commands
clean:
	rm -f *.o assembler 

test:
	./assembler input_files/good1 input_files/good2 input_files/good3 input_files/faulty1 input_files/faulty2 