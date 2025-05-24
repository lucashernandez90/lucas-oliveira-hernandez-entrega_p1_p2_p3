#ifndef NEANDER_H
#define NEANDER_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define STA 0x10
#define LDA 0x20
#define ADD 0x30
#define SUB 0x40
#define NOT 0x60
#define JMP 0x80
#define JN 0x90
#define JZ 0xA0
#define HLT 0xF0

void executarNeander(const char *filename);

#endif // NEANDER_H