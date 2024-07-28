all: final_project

final_project: main.o macro_Handling.o
	gcc -g -ansi -pedantic -Wall main.o macro_Handling.o -o final_project

main.o: main.c main.h macro_Handling.h
	gcc -g -ansi -pedantic -Wall -c main.c

macro_Handling.o: macro_Handling.c macro_Handling.h
	gcc -g -ansi -pedantic -Wall -c macro_Handling.c

clean:
	rm -f final_project main.o macro_Handling.o
