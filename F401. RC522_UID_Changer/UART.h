//===============================================================================
#include <stm32f401xe.h>
#include <string.h>
#include <stdio.h>
//===============================================================================
void UART1_Init(void);
void UART2_Init(void);

void UART1_SendChar(uint8_t);
void UART1_SendString(char*);

void UART2_SendChar(uint8_t);
void UART2_SendString(char*);
//===============================================================================
