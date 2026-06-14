#ifndef RCC_H
#define RCC_H

#include <stm32h747xx.h>

extern volatile uint32_t msTicks;

void delay_ms(uint32_t);
uint32_t get_ms(void);

#endif
