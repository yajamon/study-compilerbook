all: c assembly

bin:
	mkdir bin

c: bin program.c
	gcc -o bin/program_c program.c

assembly: bin program.s
	gcc -o bin/program_asm program.s

.FHONY: all c assembly
