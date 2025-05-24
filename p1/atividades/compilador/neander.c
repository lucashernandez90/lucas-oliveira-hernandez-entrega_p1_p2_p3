#include "neander.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

void executarNeander(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        printf("Erro ao abrir arquivo.\n");
        return;
    }

    uint8_t bytes[256] = {0};
    uint8_t byte = 0;
    int getByte = 0;
    uint32_t memID = 0;
    uint8_t ac = 0, pc = 0;
    bool neg = false, zer = false;

    if (fread(&memID, 1, 4, file) != 4) {
        printf("Erro ao ler o cabeçalho do arquivo.\n");
        fclose(file);
        return;
    }

    if (memID != 0x52444E03) {
        printf("O arquivo fornecido não é um formato válido do Neander.\n");
        fclose(file);
        return;
    }

    for (int i = 0; i < 512; i++) {
        if (i % 2 == 1) {
            if (fread(&byte, 1, 1, file) != 1) {
                printf("Erro ao ler o arquivo.\n");
                fclose(file);
                return;
            }
        } else {
            if (fread(&byte, 1, 1, file) != 1) {
                printf("Erro ao ler o arquivo.\n");
                fclose(file);
                return;
            }
            bytes[getByte++] = byte;
        }
    }
    fclose(file);

    while (bytes[pc] != HLT) {
        neg = (ac >= 0x80);
        zer = (ac == 0);

        switch (bytes[pc]) {
        case STA:
            pc++;
            bytes[bytes[pc]] = ac;
            break;
        case LDA:
            pc++;
            ac = bytes[bytes[pc]];
            break;
        case ADD:
            pc++;
            ac += bytes[bytes[pc]];
            break;
        case SUB:
            pc++;
            ac -= bytes[bytes[pc]];
            break;
        case NOT:
            ac = ~ac;
            break;
        case JMP:
            pc++;
            pc = bytes[pc] - 1;
            break;
        case JN:
            pc++;
            if (neg)
                pc = bytes[pc] - 1;
            break;
        case JZ:
            pc++;
            if (zer)
                pc = bytes[pc] - 1;
            break;
        }
        pc++;
    }

    printf("\nAC: %02x - PC: %02x\nflags: N(%c) - Z(%c)\n", ac, pc,
           neg ? '*' : ' ', zer ? '*' : ' ');

    printf("\nMemoria final =========================================");
    for (int i = 0; i < 256; i++) {
        if (i % 16 == 0)
            printf("\n%07x ", i);
        printf("%02x ", bytes[i]);
    }
    printf("\n=======================================================\n");
}