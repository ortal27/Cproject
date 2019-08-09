compile:
	gcc -Wall -ansi -pedantic -o assembler *.c

compile_old:
	gcc -o assembler *.c

debug:
	gcc -Wall -ansi -pedantic -g -o assembler *.c
 
run:
	./assembler test