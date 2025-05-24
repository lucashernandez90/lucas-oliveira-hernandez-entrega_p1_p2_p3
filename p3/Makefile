CC = gcc
CFLAGS = -Wall -Wextra -std=c99

EXPR = a = 4 / 2 * 3 

all: compilador executor

compilador: compilador.c
	$(CC) $(CFLAGS) -o compilador compilador.c

executor: executor.c
	$(CC) $(CFLAGS) -o executor executor.c

bf: all
	echo "$(EXPR)" | ./compilador

run: all
	echo "$(EXPR)" | ./compilador | ./executor

clean:
	rm -f compilador executor
