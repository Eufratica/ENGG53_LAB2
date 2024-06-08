#include "delay.h"
#include <xc.h>

#define SYSTEMCLOCK 80000000 // Defina o clock do sistema para 80 MHz

void delayUs(unsigned int usec) {
    unsigned int cycles = (SYSTEMCLOCK / 2000000) * usec; // 2 ciclos por instru��o
    while (cycles--) {
        __asm__ volatile ("nop"); // Nop() � uma instru��o que n�o faz nada, usada para criar delays
    }
}

void delayMs(unsigned int msec) {
    while (msec--) {
        delayUs(1000); // 1000 microssegundos = 1 milissegundo
    }
}