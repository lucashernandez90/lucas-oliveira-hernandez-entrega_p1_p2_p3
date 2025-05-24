# Compilador LPN para Neander

**Nome: Lucas de Oliveira Hernandez**

## Descrição do Sistema

Este projeto implementa um sistema completo de compilação e execução para a arquitetura Neander. O sistema é composto por três componentes principais:

1. **Compilador LPN → Assembly**: Traduz programas escritos na linguagem LPN (Linguagem de Programação Neander) para assembly Neander.
2. **Assembler**: Converte o código assembly Neander para código de máquina binário.
3. **Executor**: Simula a execução do código de máquina na arquitetura Neander.

## Compilando o Sistema

Para compilar todos os componentes manualmente, execute:

- Primeiro execute ```main.c``` , ```parser.c ```, ```executor.c``` e ```assembler.c``` para gerar os executaveis do programa

```bash
gcc main.c parser.c gerador_asm.c -o compilador
gcc assembler.c -o assembler
gcc executor.c neander.c -o executor
```

- Depois de executar esses comandos gere os arquivos ```.asm``` , ```.bin``` e depos vai executar o VM

```bash
./compilador exemplo.lpn      
./assembler exemplo.asm       
./executor exemplo.bin   
```

## Rodar com o MakeFile

1. No terminal navague até o terminal:

 ```bash
D:\maquina-virtual-lucashernandez90-main\atividades\compilador
```

2. No terminal apenas tem que digitar make, para poder rodar o código:

```bash 
make
```

3. Execute o programa:

```bash
make run
```

4. Se quiser limpar os arquivos:

```bash
make clean
```

## Caso de Uso Completo

### 1. Escreva um programa em LPN

Crie um arquivo chamado `programa.lpn`:

```
PROGRAMA "programa":
INICIO
A = 5
B = 10
C = A + B
RES = C * 2
FIM
```

Isso gerará o arquivo `programa.asm` contendo código assembly Neander.

### 3. Assemble o código assembly para binário

```bash
./assembler programa.asm
```

Isso gerará o arquivo `programa.bin` contendo o código de máquina.


O programa executará e mostrará o estado final do acumulador, PC e memória.

## Linguagem LPN

A linguagem LPN é uma linguagem de alto nível simples com as seguintes características:

- Declarações de atribuição (A = 5)
- Operações aritméticas (+, -, *, /)
- Variável especial RES para armazenar o resultado final

## Limitações Conhecidas

1. **Operações de Multiplicação e Divisão**: Implementadas usando laços de adição e subtração, o que pode ser ineficiente para números grandes.

2. **Tamanho da Memória**: A arquitetura Neander é limitada a 256 bytes de memória.

3. **Variáveis**: O sistema suporta até 100 variáveis diferentes.

4. **Números Negativos**: O tratamento de números negativos em divisões pode apresentar comportamento inconsistente.

## Estrutura de Diretórios

  - `main.c` - Ponto de entrada do compilador
  - `parser.c/h` - Analisador sintático e semântico
  - `gerador_asm.c/h` - Gerador de código assembly
  - `assembler.c` - Implementação do assembler
  - `executor.c` - Ponto de entrada do executor
  - `neander.c/h` - Implementação da máquina virtual Neander

## Arquitetura Neander

O Neander é uma arquitetura simplificada com as seguintes características:
- Memória de 256 bytes endereçáveis
- Acumulador único (AC)
- Registrador de programa (PC)
- Flags de estado (N - negativo, Z - zero)
- Conjunto de instruções simples:
  - STA (0x10): Armazena o valor do acumulador na memória
  - LDA (0x20): Carrega um valor da memória para o acumulador
  - ADD (0x30): Adiciona um valor da memória ao acumulador
  - SUB (0x40): Subtrai um valor da memória do acumulador
  - NOT (0x60): Inverte os bits do acumulador
  - JMP (0x80): Salta para um endereço especificado
  - JN (0x90): Salta se o flag N estiver ativo
  - JZ (0xA0): Salta se o flag Z estiver ativo
  - HLT (0xF0): Para a execução