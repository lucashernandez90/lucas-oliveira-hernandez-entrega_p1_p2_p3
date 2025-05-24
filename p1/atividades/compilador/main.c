#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "gerador_asm.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <arquivo.lpn>\n", argv[0]);
        return 1;
    }

    const char *arquivo_entrada = argv[1];
    Programa *programa = analisar_arquivo(arquivo_entrada);
    if (!programa) {
        fprintf(stderr, "Erro ao analisar o arquivo %s\n", arquivo_entrada);
        return 1;
    }

    char arquivo_saida[256];
    strcpy(arquivo_saida, arquivo_entrada);
    char *ponto = strrchr(arquivo_saida, '.');
    if (ponto) *ponto = '\0';
    strcat(arquivo_saida, ".asm");

    gerar_codigo_asm(programa, arquivo_saida);
    liberar_programa(programa);

    printf("Arquivo %s gerado com sucesso.\n", arquivo_saida);
    return 0;
}