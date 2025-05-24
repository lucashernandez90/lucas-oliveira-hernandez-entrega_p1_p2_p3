# Compilador para Expressões Aritméticas em Brainfuck

Este projeto consiste em dois programas escritos em C:

- `compilador.c`: converte expressões aritméticas simples (como `a = 4 * 3 + 2`) em código Brainfuck.
- `executor.c`: interpreta o código Brainfuck gerado e mostra o resultado numérico da expressão.

---

## Estrutura

```text
.
├── compilador.c      # Converte expressões em código Brainfuck
├── executor.c        # Executa o código Brainfuck gerado
├── Makefile          # Automatiza a compilação e execução
└── README.md         # Este arquivo
```
---

## Requisitos

- GCC (ou outro compilador C compatível com C99)
- Make

---

## Compilação

Compile os arquivos com:

```bash
make
```

Isso irá gerar os binários `compilador` e `executor`

## Execução

Para testar uma expressão diretamente, edite a variável EXPR no `Makefile`. Exemplo:

```bash
EXPR = a = 4 * 3 + 2
```

Depois Execute com:

```bash
make run
```