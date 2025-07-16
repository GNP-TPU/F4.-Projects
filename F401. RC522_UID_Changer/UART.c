//===============================================================================
#include "UART.h"
//===============================================================================
void UART1_Init(void){
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
	
	GPIOB->MODER |= GPIO_MODER_MODE6_1;
	GPIOB->MODER |= GPIO_MODER_MODE7_1;
	
	GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR6;
	GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR7;
	
	GPIOB->AFR[0] |= (7 << 24);
	GPIOB->AFR[0] |= (7 << 28);
	
	USART1->CR1 &= ~(0xFF);
	USART1->CR1 |= (1 << 13);
	USART1->CR1 &= ~(1 << 12);
	USART1->BRR = 0x08B; // 115200 baudrate
	//0x683; // 9600 baudrate
	USART1->CR1 |= (1 << 2);
	USART1->CR1 |= (1 << 3);
	
	USART1->CR1 |= USART_CR1_RXNEIE;
	NVIC_EnableIRQ(USART1_IRQn);
}

void UART2_Init(void){
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	
	GPIOA->MODER |= (2 << 4) | (2 << 6);
	GPIOA->OSPEEDR |= (3 << 4) | (3 << 6);
	GPIOA->AFR[0] |= (7 << 8) | (7 << 12);
	
	USART2->CR1 = 0x00;
	USART2->CR1 |= (1 << 13);
	USART2->CR1 &= ~(1 << 12);
	USART2->BRR = 0x08B; // 115200 baudrate
	USART2->CR1 |= (1 << 2);
	USART2->CR1 |= (1 << 3);
	
	USART2->CR1 |= USART_CR1_RXNEIE;
	NVIC_EnableIRQ(USART2_IRQn);
}

//===============================================================================
void UART1_SendChar(uint8_t ch){
	while(!(USART1->SR & 0x80));
	USART1->DR = ch;
}

void UART1_SendString(char* str){
	while(*str)
		UART1_SendChar(*str++);
}
//===============================================================================
void UART2_SendChar(uint8_t ch){
	while(!(USART2->SR & 0x80));
	USART2->DR = ch;
}

void UART2_SendString(char* str){
	while(*str)
		UART2_SendChar(*str++);
}
//===============================================================================
