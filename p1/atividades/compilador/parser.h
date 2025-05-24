#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>

typedef struct {
    char operacao[16];
    int argumento;
} Instrucao;

typedef struct {
    Instrucao *instrucoes;
    int quantidade;
} Programa;

Programa *analisar_arquivo(const char *nome_arquivo);
void liberar_programa(Programa *programa);

#endif