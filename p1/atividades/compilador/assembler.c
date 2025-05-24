#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

#define MAX_LINES 256
#define HEADER 0x52444E03

typedef struct {
    char label[32];
    int address;
} Label;

typedef struct {
    char instr[16];
    char arg[16];
} Line;

Label labelTable[MAX_LINES];
int labelCount = 0;
uint8_t memory[256] = {0};
Line program[MAX_LINES];
int programSize = 0;

int getOpcode(const char *instr) {
    if (strcmp(instr, "STA") == 0) return 0x10;
    if (strcmp(instr, "LDA") == 0) return 0x20;
    if (strcmp(instr, "ADD") == 0) return 0x30;
    if (strcmp(instr, "SUB") == 0) return 0x40;
    if (strcmp(instr, "NOT") == 0) return 0x60;
    if (strcmp(instr, "JMP") == 0) return 0x80;
    if (strcmp(instr, "JN")  == 0) return 0x90;
    if (strcmp(instr, "JZ")  == 0) return 0xA0;
    if (strcmp(instr, "HLT") == 0) return 0xF0;
    return -1;
}

void removeComment(char *line) {
    char *semicolon = strchr(line, ';');
    if (semicolon) *semicolon = '\0';
}

void trim(char *str) {
    while (isspace(*str)) str++;
    char *end = str + strlen(str) - 1;
    while (end > str && isspace(*end)) *end-- = '\0';
}

int findLabelAddress(const char *label) {
    for (int i = 0; i < labelCount; i++) {
        if (strcmp(labelTable[i].label, label) == 0)
            return labelTable[i].address;
    }
    fprintf(stderr, "Erro: rotulo não encontrado: %s\n", label);
    exit(1);
}

int parseHexOrLabel(char *arg) {
    if (isxdigit(arg[0])) {
        return (int)strtol(arg, NULL, 16);
    } else {
        return findLabelAddress(arg);
    }
}

void firstPass(FILE *file) {
    char line[128];
    int addr = 0;

    while (fgets(line, sizeof(line), file)) {
        removeComment(line);
        trim(line);

        if (strlen(line) == 0) continue;

        char *label = strtok(line, ":");
        char *rest = strtok(NULL, "");

        if (rest != NULL) {
            //linha rótulo e instrucao
            trim(rest);
            strcpy(labelTable[labelCount].label, label);
            labelTable[labelCount++].address = addr;
            sscanf(rest, "%s %s", program[programSize].instr, program[programSize].arg);
            addr += 2;
            programSize++;
        } else {
            //ou é instrucao, ou rotulo puro
            if (strchr(label, ' ') == NULL) {
                //rótulo puro
                strcpy(labelTable[labelCount].label, label);
                labelTable[labelCount++].address = addr;
            } else {
                //so instrucao
                sscanf(label, "%s %s", program[programSize].instr, program[programSize].arg);
                addr += 2;
                programSize++;
            }
        }
    }

    rewind(file);
}

void secondPass() {
    int addr = 0;
    for (int i = 0; i < programSize; i++) {
        int opcode = getOpcode(program[i].instr);
        if (opcode == -1) {
            fprintf(stderr, "Instrucao desconhecida: %s\n", program[i].instr);
            exit(1);
        }

        memory[addr++] = opcode;

        if (opcode != 0xF0) { //nao é HLT, espera argumento
            int value = parseHexOrLabel(program[i].arg);
            memory[addr++] = (uint8_t)value;
        } else {
            memory[addr++] = 0x00; //HLT é seguido de um byte nulo
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: %s arquivo.asm\n", argv[0]);
        return 1;
    }

    FILE *input = fopen(argv[1], "r");
    if (!input) {
        perror("Erro ao abrir arquivo");
        return 1;
    }

    firstPass(input);
    secondPass();
    fclose(input);

    //Salva o arquivo .bin
    char outname[256];
    strcpy(outname, argv[1]);
    char *dot = strrchr(outname, '.');
    if (dot) strcpy(dot, ".bin");
    else strcat(outname, ".bin");

    FILE *output = fopen(outname, "wb");
    if (!output) {
        perror("Erro ao criar arquivo de saída");
        return 1;
    }

    uint32_t header = HEADER;
    fwrite(&header, sizeof(uint32_t), 1, output);

    for (int i = 0; i < 256; i++) {
        fputc(i, output);
        fputc(memory[i], output);
    }

    fclose(output);
    printf("Arquivo binário gerado com sucesso: %s\n", outname);
    return 0;
}