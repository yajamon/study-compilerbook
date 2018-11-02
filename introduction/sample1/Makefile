all: c assembly

bin:
	mkdir bin

c: bin
	gcc -o bin/program_c program.c

assembly: bin
	gcc -o bin/program_asm program.s

.FHONY: all c assembly
