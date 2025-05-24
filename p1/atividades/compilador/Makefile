CC = gcc
CFLAGS = -Wall

all: compilador assembler executor

compilador: main.c parser.c gerador_asm.c
	$(CC) $(CFLAGS) main.c parser.c gerador_asm.c -o compilador

assembler: assembler.c
	$(CC) $(CFLAGS) assembler.c -o assembler

executor: executor.c neander.c
	$(CC) $(CFLAGS) executor.c neander.c -o executor

# Regra para compilar, montar e executar o programa
run: all
	./compilador programa.lpn
	./assembler programa.asm
	./executor programa.bin

clean:
	del /Q *.exe *.o programa.asm programa.bin
