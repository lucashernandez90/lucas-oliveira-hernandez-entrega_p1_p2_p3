#include "neander.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Uso: %s <arquivo.bin>\n", argv[0]);
        return 1;
    }

    executarNeander(argv[1]);
    return 0;
}