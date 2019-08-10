compile:
	gcc -Wall -ansi -pedantic -o assembler *.c

debug:
	gcc -Wall -ansi -pedantic -g -o assembler *.c
 
run:
	./assembler __tests__/test