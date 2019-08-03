compile:
	gcc -o assembler *.c

debug:
	gcc -g -o assembler *.c

run:
	./assembler test.as