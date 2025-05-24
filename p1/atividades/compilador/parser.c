#include "parser.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

#define TAM_MAX_LINHA 256
#define MAX_INSTRUCOES 256
#define MAX_VARS 100
#define TEMP_ADDR_START 200  //Endereços para variáveis temporárias

//Tabela de símbolos para variáveis
typedef struct {
    char nome[32];
    int valor;
    int endereco;
} Variavel;

//Estrutura para pilha de operadores
typedef struct {
    char operadores[32];
    int topo;
} PilhaOperadores;

//Estrutura para pilha de operandos/endereços
typedef struct {
    int operandos[32];
    int topo;
} PilhaOperandos;

Variavel tabela_variaveis[MAX_VARS];
int num_variaveis = 0;
int prox_endereco = 128; //Começamos a armazenar variáveis a partir do endereço 128
int prox_temp = TEMP_ADDR_START; //Endereço para variáveis temporárias

//Funções auxiliares
void trim(char *str) {
    char *inicio = str;
    char *fim;
    
    //Pula espaços no início
    while(isspace((unsigned char)*inicio)) inicio++;
    
    if(*inicio == 0) { //String só com espaços
        *str = 0;
        return;
    }
    
    //Encontra o final da string e recua para ignorar espaços no fim
    fim = inicio + strlen(inicio) - 1;
    while(fim > inicio && isspace((unsigned char)*fim)) fim--;
    
    //Coloca o terminador após o último caractere não-espaço
    *(fim + 1) = 0;
    
    //Move a string se necessário
    if(inicio != str) {
        memmove(str, inicio, strlen(inicio) + 1);
    }
}

//Encontra uma variável pelo nome
int buscar_variavel(const char *nome) {
    for (int i = 0; i < num_variaveis; i++) {
        if (strcmp(tabela_variaveis[i].nome, nome) == 0) {
            return i;
        }
    }
    return -1;
}

// Adiciona uma nova variável
int adicionar_variavel(const char *nome, int valor) {
    int idx = buscar_variavel(nome);
    if (idx >= 0) {
        tabela_variaveis[idx].valor = valor;
        return idx;
    }
    
    strncpy(tabela_variaveis[num_variaveis].nome, nome, 31);
    tabela_variaveis[num_variaveis].nome[31] = '\0';
    tabela_variaveis[num_variaveis].valor = valor;
    tabela_variaveis[num_variaveis].endereco = prox_endereco++;
    return num_variaveis++;
}

//Aloca um endereço temporário
int alocar_temporario() {
    return prox_temp++;
}

// Adiciona uma instrução a um programa
void adicionar_instrucao(Programa *programa, const char *operacao, int argumento) {
    if (programa->quantidade < MAX_INSTRUCOES) {
        strncpy(programa->instrucoes[programa->quantidade].operacao, operacao, 15);
        programa->instrucoes[programa->quantidade].operacao[15] = '\0';
        programa->instrucoes[programa->quantidade].argumento = argumento;
        programa->quantidade++;
    }
}

//Verifica se uma string contém apenas dígitos (para identificar números)
int eh_numero(const char *str) {
    while (*str) {
        if (!isdigit((unsigned char)*str)) return 0;
        str++;
    }
    return 1;
}

//Obtém o endereço de um termo (variável ou valor literal)
int obter_endereco_ou_valor(const char *termo) {
    if (eh_numero(termo)) {
        return atoi(termo);
    } else {
        int var_idx = buscar_variavel(termo);
        if (var_idx >= 0) {
            return tabela_variaveis[var_idx].endereco;
        } else {
            // Criar variável com valor zero se não existir
            var_idx = adicionar_variavel(termo, 0);
            return tabela_variaveis[var_idx].endereco;
        }
    }
}

//Inicializa pilha de operadores
void inicializar_pilha_op(PilhaOperadores *pilha) {
    pilha->topo = -1;
}

//Empilha operador
void empilhar_operador(PilhaOperadores *pilha, char op) {
    pilha->operadores[++pilha->topo] = op;
}

//Desempilha os operadores
char desempilhar_operador(PilhaOperadores *pilha) {
    if (pilha->topo >= 0)
        return pilha->operadores[pilha->topo--];
    return '\0';
}

//Espia operador no topo sem remover
char topo_operador(PilhaOperadores *pilha) {
    if (pilha->topo >= 0)
        return pilha->operadores[pilha->topo];
    return '\0';
}

//Verifica se pilha de operadores está vazia
int pilha_operadores_vazia(PilhaOperadores *pilha) {
    return pilha->topo == -1;
}

//Inicializa pilha dos operandos
void inicializar_pilha_end(PilhaOperandos *pilha) {
    pilha->topo = -1;
}

//Empilha o operando
void empilhar_operando(PilhaOperandos *pilha, int end) {
    pilha->operandos[++pilha->topo] = end;
}

//Desempilha o operando
int desempilhar_operando(PilhaOperandos *pilha) {
    if (pilha->topo >= 0)
        return pilha->operandos[pilha->topo--];
    return -1;
}

//Verifica se pilha dos operandos está vazia
int pilha_operandos_vazia(PilhaOperandos *pilha) {
    return pilha->topo == -1;
}

//Retorna a precedência do operador
int precedencia(char op) {
    if (op == '*' || op == '/') return 2;
    if (op == '+' || op == '-') return 1;
    return 0;
}

//Implementa multiplicação usando repetição de adições
void gerar_codigo_multiplicacao(Programa *programa, int endereco_a, int endereco_b) {
    
    int temp_contador = alocar_temporario();
    int temp_resultado = alocar_temporario();
    int temp_zero = alocar_temporario();
    
    //Inicializa resultado com zero
    adicionar_instrucao(programa, "LDA", 0);
    adicionar_instrucao(programa, "STA", temp_resultado);
    adicionar_instrucao(programa, "STA", temp_zero);
    adicionar_instrucao(programa, "STA", temp_contador + 1); // Registrador extra para comparações
    
    //Carrega o valor do multiplicador em uma variável temporária
    adicionar_instrucao(programa, "LDA", endereco_b);
    adicionar_instrucao(programa, "STA", temp_contador);
    
    //Início do loop
    int inicio_loop = programa->quantidade;
    
    //Verifica se o contador é zero
    adicionar_instrucao(programa, "LDA", temp_contador);
    adicionar_instrucao(programa, "SUB", temp_contador + 1); // Compare com zero
    
    //Salta para o fim do loop se for zero (Z=1)
    int salto_fim = programa->quantidade;
    adicionar_instrucao(programa, "JZ", 0);  // Será corrigido depois
    
    //Adiciona o valor do multiplicando ao resultado
    adicionar_instrucao(programa, "LDA", temp_resultado);
    adicionar_instrucao(programa, "ADD", endereco_a);
    adicionar_instrucao(programa, "STA", temp_resultado);
    
    //Decrementa 
    adicionar_instrucao(programa, "LDA", temp_contador);
    adicionar_instrucao(programa, "SUB", 1);
    adicionar_instrucao(programa, "STA", temp_contador);
    
    //Volta ao início do loop
    adicionar_instrucao(programa, "JMP", inicio_loop);
    
    //Atualiza o endereço do salto para o fim
    int fim_loop = programa->quantidade;
    programa->instrucoes[salto_fim].argumento = fim_loop;
    
    //Carrega resultado final no acumulador
    adicionar_instrucao(programa, "LDA", temp_resultado);
}

//Implementa divisão usando subtrações sucessivas
void gerar_codigo_divisao(Programa *programa, int endereco_a, int endereco_b) {
    // Aloca variáveis temporárias
    int temp_resultado = alocar_temporario();
    int temp_resto = alocar_temporario();
    int temp_divisor = alocar_temporario();
    
    adicionar_instrucao(programa, "LDA", 0);
    adicionar_instrucao(programa, "STA", temp_resultado);
     
    adicionar_instrucao(programa, "LDA", endereco_a);
    adicionar_instrucao(programa, "STA", temp_resto);
    
    adicionar_instrucao(programa, "LDA", endereco_b);
    adicionar_instrucao(programa, "STA", temp_divisor);
    
    int inicio_loop = programa->quantidade;
    
    //Verifica se o resto é menor divisor 
    adicionar_instrucao(programa, "LDA", temp_resto);
    adicionar_instrucao(programa, "SUB", temp_divisor);
    
    //Se for negativo pula para o fim
    int salto_fim = programa->quantidade;
    adicionar_instrucao(programa, "JN", 0);  // Será corrigido depois
    
    //Se não for negativo continua a divisão
    adicionar_instrucao(programa, "STA", temp_resto); // Resto = Resto - Divisor
    adicionar_instrucao(programa, "LDA", temp_resultado);
    adicionar_instrucao(programa, "ADD", 1); // Incrementa o resultado
    adicionar_instrucao(programa, "STA", temp_resultado);
    
    //Volta ao início do loop
    adicionar_instrucao(programa, "JMP", inicio_loop);
    
    //Atualiza o endereço do salto para o fim
    int fim_loop = programa->quantidade;
    programa->instrucoes[salto_fim].argumento = fim_loop;
    
    //Carrega resultado final no acumulador
    adicionar_instrucao(programa, "LDA", temp_resultado);
}

//Executa uma operação aritmética e adiciona as instruções necessárias
void executar_operacao(Programa *programa, char operador, PilhaOperandos *pilha_end) {
    int endereco_b = desempilhar_operando(pilha_end);
    int endereco_a = desempilhar_operando(pilha_end);
    int temp_resultado = alocar_temporario();
    
    //Carrega o primeiro operando no acumulador
    adicionar_instrucao(programa, "LDA", endereco_a);
    
    //Executa a operação correspondente
    if (operador == '+') {
        adicionar_instrucao(programa, "ADD", endereco_b);
    } else if (operador == '-') {
        adicionar_instrucao(programa, "SUB", endereco_b);
    } else if (operador == '*') {
        //Salva o primeiro operando
        adicionar_instrucao(programa, "STA", temp_resultado);
        //Realiza a multiplicação
        gerar_codigo_multiplicacao(programa, temp_resultado, endereco_b);
    } else if (operador == '/') {
        //Salva o primeiro operando
        adicionar_instrucao(programa, "STA", temp_resultado);
        //Realiza a divisão
        gerar_codigo_divisao(programa, temp_resultado, endereco_b);
    }
    
    //Armazena o resultado em uma posição temporária
    adicionar_instrucao(programa, "STA", temp_resultado);
    
    //Empilha o endereço do resultado
    empilhar_operando(pilha_end, temp_resultado);
}

//Processa uma expressão com precedência de operadores e parênteses
void processar_expressao_complexa(Programa *programa, const char *var_dest, const char *expressao) {
    char expr_limpa[TAM_MAX_LINHA] = {0};
    int idx_limpa = 0;
    
    //Remover espaços da expressão
    for (int i = 0; expressao[i]; i++) {
        if (!isspace(expressao[i])) {
            expr_limpa[idx_limpa++] = expressao[i];
        }
    }
    expr_limpa[idx_limpa] = '\0';
    
    //Inicializar pilhas
    PilhaOperadores pilha_op;
    PilhaOperandos pilha_end;
    inicializar_pilha_op(&pilha_op);
    inicializar_pilha_end(&pilha_end);
    
    //Analisar a expressão
    char termo[32];
    int idx_termo = 0;
    
    for (int i = 0; expr_limpa[i]; i++) {
        //Se for dígito ou letra, parte de um operando
        if (isalnum(expr_limpa[i])) {
            termo[idx_termo++] = expr_limpa[i];
        } 
        //Se for operador ou parêntese
        else {
            
            if (idx_termo > 0) {
                termo[idx_termo] = '\0';
                int endereco = obter_endereco_ou_valor(termo);
                empilhar_operando(&pilha_end, endereco);
                idx_termo = 0;
            }
            
            //Trata operadores e parênteses
            if (expr_limpa[i] == '(') {
                empilhar_operador(&pilha_op, expr_limpa[i]);
            } 
            else if (expr_limpa[i] == ')') {
                //Processa operadores até encontrar o (
                while (!pilha_operadores_vazia(&pilha_op) && topo_operador(&pilha_op) != '(') {
                    char op = desempilhar_operador(&pilha_op);
                    executar_operacao(programa, op, &pilha_end);
                }
                // Remove o ( da pilha
                if (!pilha_operadores_vazia(&pilha_op)) {
                    desempilhar_operador(&pilha_op); 
                }
            } 
            else if (expr_limpa[i] == '+' || expr_limpa[i] == '-' || 
                     expr_limpa[i] == '*' || expr_limpa[i] == '/') {

                while (!pilha_operadores_vazia(&pilha_op) && 
                       topo_operador(&pilha_op) != '(' && 
                       precedencia(topo_operador(&pilha_op)) >= precedencia(expr_limpa[i])) {
                    char op = desempilhar_operador(&pilha_op);
                    executar_operacao(programa, op, &pilha_end);
                }
                empilhar_operador(&pilha_op, expr_limpa[i]);
            }
        }
    }
    
    if (idx_termo > 0) {
        termo[idx_termo] = '\0';
        int endereco = obter_endereco_ou_valor(termo);
        empilhar_operando(&pilha_end, endereco);
    }
    
    //Processa operadores restantes na pilha
    while (!pilha_operadores_vazia(&pilha_op)) {
        char op = desempilhar_operador(&pilha_op);
        executar_operacao(programa, op, &pilha_end);
    }

    if (!pilha_operandos_vazia(&pilha_end)) {
        int endereco_resultado = desempilhar_operando(&pilha_end);
        adicionar_instrucao(programa, "LDA", endereco_resultado);
        
        //Armazena o resultado na variável de destino
        int dest_idx = adicionar_variavel(var_dest, 0);
        adicionar_instrucao(programa, "STA", tabela_variaveis[dest_idx].endereco);
    } else {
        //Caso a expressão esteja vazia ou com erro
        adicionar_instrucao(programa, "LDA", 0);
        int dest_idx = adicionar_variavel(var_dest, 0);
        adicionar_instrucao(programa, "STA", tabela_variaveis[dest_idx].endereco);
    }
}

void processar_atribuicao_simples(Programa *programa, const char *variavel, const char *valor) {
    int var_idx = adicionar_variavel(variavel, 0);
    
    if (eh_numero(valor)) {
        //Se o valor é um número literal
        adicionar_instrucao(programa, "LDA", atoi(valor));
    } else {
        //Se o valor é uma variável
        int val_idx = buscar_variavel(valor);
        if (val_idx >= 0) {
            adicionar_instrucao(programa, "LDA", tabela_variaveis[val_idx].endereco);
        } else {
            //Variável não encontrada, assume valor 0
            adicionar_instrucao(programa, "LDA", 0);
        }
    }
    
    adicionar_instrucao(programa, "STA", tabela_variaveis[var_idx].endereco);
}

void processar_expressao_simples(Programa *programa, const char *var_dest, const char *expressao) {
    char expr_copia[TAM_MAX_LINHA];
    strncpy(expr_copia, expressao, TAM_MAX_LINHA - 1);
    expr_copia[TAM_MAX_LINHA - 1] = '\0';
    
    //Remover espaços
    char *p = expr_copia;
    char *q = expr_copia;
    while (*p) {
        if (!isspace((unsigned char)*p)) {
            *q++ = *p;
        }
        p++;
    }
    *q = '\0';
    
    //Dividir a expressão em termos e operadores
    char termo[32];
    int termo_idx = 0;
    int primeiro_termo = 1;
    char operador_anterior = '+';
    
    for (p = expr_copia; *p; p++) {
        if (*p == '+' || *p == '-' || *p == '*' || *p == '/') {
            //Encontrou um operador, processa o termo anterior
            termo[termo_idx] = '\0';
            
            if (primeiro_termo) {
                //Para o primeiro termo, apenas carregamos no acumulador
                int end = obter_endereco_ou_valor(termo);
                adicionar_instrucao(programa, "LDA", end);
                primeiro_termo = 0;
            } else {
                int end = obter_endereco_ou_valor(termo);
                
                if (operador_anterior == '+') {
                    adicionar_instrucao(programa, "ADD", end);
                } else if (operador_anterior == '-') {
                    adicionar_instrucao(programa, "SUB", end);
                } else if (operador_anterior == '*') {
                    int temp = alocar_temporario();
                    adicionar_instrucao(programa, "STA", temp);
                    gerar_codigo_multiplicacao(programa, temp, end);
                } else if (operador_anterior == '/') {
                    int temp = alocar_temporario();
                    adicionar_instrucao(programa, "STA", temp);
                    gerar_codigo_divisao(programa, temp, end);
                }
            }
            
            operador_anterior = *p;
            termo_idx = 0;
        } else {
            if (termo_idx < 31) {
                termo[termo_idx++] = *p;
            }
        }
    }

    if (termo_idx > 0) {
        termo[termo_idx] = '\0';
        int end = obter_endereco_ou_valor(termo);
        
        if (primeiro_termo) {
            // Se só havia um termo
            adicionar_instrucao(programa, "LDA", end);
        } else {
            if (operador_anterior == '+') {
                adicionar_instrucao(programa, "ADD", end);
            } else if (operador_anterior == '-') {
                adicionar_instrucao(programa, "SUB", end);
            } else if (operador_anterior == '*') {
                int temp = alocar_temporario();
                adicionar_instrucao(programa, "STA", temp);
                gerar_codigo_multiplicacao(programa, temp, end);
            } else if (operador_anterior == '/') {
                int temp = alocar_temporario();
                adicionar_instrucao(programa, "STA", temp);
                gerar_codigo_divisao(programa, temp, end);
            }
        }
    }
    
    int dest_idx = adicionar_variavel(var_dest, 0);
    adicionar_instrucao(programa, "STA", tabela_variaveis[dest_idx].endereco);
}

void processar_expressao(Programa *programa, const char *linha) {
    char copia[TAM_MAX_LINHA];
    strncpy(copia, linha, TAM_MAX_LINHA - 1);
    copia[TAM_MAX_LINHA - 1] = '\0';
    
    //Divide em variável e expressão
    char *igual = strchr(copia, '=');
    if (!igual) return;
    
    *igual = '\0';
    char *var_dest = copia;
    char *expressao = igual + 1;
    
    trim(var_dest);
    trim(expressao);
    
    //Removendo comentários se existirem
    char *comentario = strchr(expressao, ';');
    if (comentario) *comentario = '\0';
    trim(expressao);
    
    // Verifica se há um operador na expressão
    if (strchr(expressao, '+') || strchr(expressao, '-') || 
        strchr(expressao, '*') || strchr(expressao, '/')) {
        processar_expressao_complexa(programa, var_dest, expressao);
    } else {
        processar_atribuicao_simples(programa, var_dest, expressao);
    }
}

Programa *analisar_arquivo(const char *nome_arquivo) {
    FILE *arquivo = fopen(nome_arquivo, "r");
    if (!arquivo) return NULL;
    
    Programa *programa = malloc(sizeof(Programa));
    programa->instrucoes = malloc(MAX_INSTRUCOES * sizeof(Instrucao));
    programa->quantidade = 0;
    
    char linha[TAM_MAX_LINHA];
    while (fgets(linha, TAM_MAX_LINHA, arquivo)) {
        //Remove quebra de linha
        size_t len = strlen(linha);
        if (len > 0 && (linha[len-1] == '\n' || linha[len-1] == '\r')) {
            linha[--len] = '\0';
        }
        
        //Remove comentários
        char *comentario = strchr(linha, ';');
        if (comentario) *comentario = '\0';
        
        trim(linha);
        
        //Ignora linhas vazias
        if (strlen(linha) == 0) continue;
        
        //Ignora cabeçalhos
        if (strncmp(linha, "PROGRAMA", 8) == 0 || 
            strncmp(linha, "INICIO", 6) == 0 || 
            strncmp(linha, "FIM", 3) == 0) {
            continue;
        }
        
        //Processa atribuições com =
        if (strchr(linha, '=')) {
            processar_expressao(programa, linha);
            continue;
        }
        
        char *token = strtok(linha, " \n");
        if (!token) continue;
        
        Instrucao instrucao;
        strcpy(instrucao.operacao, token);
        
        // Traduz operações da linguagem LPN para assembly
        if (strcmp(instrucao.operacao, "carrega") == 0) {
            strcpy(instrucao.operacao, "LDA");
        } else if (strcmp(instrucao.operacao, "adiciona") == 0) {
            strcpy(instrucao.operacao, "ADD");
        } else if (strcmp(instrucao.operacao, "subtrai") == 0) {
            strcpy(instrucao.operacao, "SUB");
        } else if (strcmp(instrucao.operacao, "armazena") == 0) {
            strcpy(instrucao.operacao, "STA");
        } else if (strcmp(instrucao.operacao, "para") == 0) {
            strcpy(instrucao.operacao, "HLT");
        }
        
        token = strtok(NULL, " \n");
        instrucao.argumento = token ? atoi(token) : 0;
        
        programa->instrucoes[programa->quantidade++] = instrucao;
    }
    
    if (programa->quantidade == 0 || strcmp(programa->instrucoes[programa->quantidade-1].operacao, "HLT") != 0) {
        strcpy(programa->instrucoes[programa->quantidade].operacao, "HLT");
        programa->instrucoes[programa->quantidade].argumento = 0;
        programa->quantidade++;
    }
    
    fclose(arquivo);
    return programa;
}

void liberar_programa(Programa *programa) {
    if (programa) {
        free(programa->instrucoes);
        free(programa);
    }
}