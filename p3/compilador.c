#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void bf_set(int value) {
    printf("[-]"); // limpa memory[0]
    for (int i = 0; i < value; i++) printf("+");
}

int main() {
    char linha[256];
    if (!fgets(linha, sizeof linha, stdin)) {
        fprintf(stderr, "Erro ao ler a linha\n");
        return 1;
    }

    int a, b, c;
    char var, op1, op2;
    int result = 0;

    // tenta primeiro com 3 números e 2 operadores
    int n = sscanf(linha, " %c = %d %c %d %c %d", &var, &a, &op1, &b, &op2, &c);

    if (n == 6) {
        // avalia a primeira operação
        switch (op1) {
            case '+': result = a + b; break;
            case '-': result = a - b; break;
            case '*': result = a * b; break;
            case '/': result = (b != 0) ? a / b : 0; break;
            default: fprintf(stderr, "Operador invalido: %c\n", op1); return 1;
        }
        // avalia a segunda operação com o resultado anterior
        switch (op2) {
            case '+': result += c; break;
            case '-': result -= c; break;
            case '*': result *= c; break;
            case '/': result = (c != 0) ? result / c : 0; break;
            default: fprintf(stderr, "Operador invalido: %c\n", op2); return 1;
        }
    }
    else if (n == -1 || n == 0) {
        fprintf(stderr, "Erro de sintaxe\n");
        return 1;
    }
    else if (n < 6) {
        // com 2 números e 1 operador
        n = sscanf(linha, " %c = %d %c %d", &var, &a, &op1, &b);
        if (n != 4) {
            fprintf(stderr, "Erro de sintaxe\n");
            return 1;
        }
        switch (op1) {
            case '+': result = a + b; break;
            case '-': result = a - b; break;
            case '*': result = a * b; break;
            case '/': result = (b != 0) ? a / b : 0; break;
            default: fprintf(stderr, "Operador invalido: %c\n", op1); return 1;
        }
    }

    // Gera código Brainfuck
    bf_set(result);
    return 0;
}
