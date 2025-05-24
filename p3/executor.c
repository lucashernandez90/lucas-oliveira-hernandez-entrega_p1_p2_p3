#include <stdio.h>
#include <stdlib.h>

#define MEMORY_SIZE 30000

int main() {
    unsigned char memory[MEMORY_SIZE] = {0};
    int ptr = 0;
    int c;

    char code[10000];
    int pos = 0;

    // le o código Brainfuck
    while ((c = getchar()) != EOF && pos < 9999) {
        if (c == '+' || c == '-' || c == '>' || c == '<' ||
            c == '[' || c == ']' || c == '.' || c == ',') {
            code[pos++] = c;
        }
    }
    code[pos] = '\0';

    // executa o codigo brainfuck
    int loop_stack[1000], loop_ptr = 0;
    for (int i = 0; code[i]; i++) {
        switch (code[i]) {
            case '>': ptr++; break;
            case '<': ptr--; break;
            case '+': memory[ptr]++; break;
            case '-': memory[ptr]--; break;
            case '.': putchar(memory[ptr]); break;
            case ',': memory[ptr] = getchar(); break;
            case '[':
                if (memory[ptr] == 0) {
                    int depth = 1;
                    while (depth > 0 && code[++i]) {
                        if (code[i] == '[') depth++;
                        else if (code[i] == ']') depth--;
                    }
                } else {
                    loop_stack[loop_ptr++] = i;
                }
                break;
            case ']':
                if (memory[ptr] != 0) {
                    i = loop_stack[loop_ptr - 1];
                } else {
                    loop_ptr--;
                }
                break;
        }
    }

    printf("\n[SAIDA NUMERICA FINAL] memory[0] = %d\n", memory[0]);
    return 0;
}
