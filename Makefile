compile:
	gcc -Wall -ansi -pedantic -o assembler *.c

debug:
	gcc -Wall -ansi -pedantic -g -o assembler *.c

run:
	./assembler __tests__/simple/test
 
run-all:
	./assembler __tests__/simple/test
	@echo
	./assembler __tests__/complex/test
	@echo
	./assembler __tests__/print_errors/test
