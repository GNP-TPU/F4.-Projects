//===============================================================================
#include "SPI.h"
#include "UART.h"
//===============================================================================
void SPI1_Init(void){
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	
	GPIOA->MODER &= ~(GPIO_MODER_MODE5 | GPIO_MODER_MODE6 | GPIO_MODER_MODE7);
	GPIOA->MODER |= (GPIO_MODER_MODE5_1 | GPIO_MODER_MODE6_1 | GPIO_MODER_MODE7_1);
	GPIOA->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR5 | GPIO_OSPEEDER_OSPEEDR6 | GPIO_OSPEEDER_OSPEEDR7);
	GPIOA->AFR[0] |= GPIO_AFRL_AFRL5_2 | GPIO_AFRL_AFRL5_0 |
									 GPIO_AFRL_AFRL6_2 | GPIO_AFRL_AFRL6_0 |
									 GPIO_AFRL_AFRL7_2 | GPIO_AFRL_AFRL7_0;
	
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
	SPI1->CR1 = 0x0000;
	SPI1->CR1 &= ~(SPI_CR1_DFF | SPI_CR1_LSBFIRST | SPI_CR1_CPOL | SPI_CR1_CPHA);
	SPI1->CR1 &= ~(SPI_CR1_BR);
	SPI1->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_SPE | SPI_CR1_MSTR;
}

void SPI2_Init(void){
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
	
	GPIOB->MODER &= ~(GPIO_MODER_MODE13 | GPIO_MODER_MODE14 | GPIO_MODER_MODE15);
	GPIOB->MODER |= (GPIO_MODER_MODE13_1 | GPIO_MODER_MODE14_1 | GPIO_MODER_MODE15_1);
	GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR13 | GPIO_OSPEEDER_OSPEEDR14 | GPIO_OSPEEDER_OSPEEDR15;
	GPIOB->AFR[1] |= GPIO_AFRH_AFRH5_2 | GPIO_AFRH_AFRH5_0 |
									 GPIO_AFRH_AFRH6_2 | GPIO_AFRH_AFRH6_0 |
	                 GPIO_AFRH_AFRH7_2 | GPIO_AFRH_AFRH7_0;

	RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
	SPI2->CR1 = 0x0000;
	SPI2->CR1 &= ~(SPI_CR1_DFF | SPI_CR1_LSBFIRST | SPI_CR1_CPOL | SPI_CR1_CPHA);
	SPI2->CR1 &= ~(SPI_CR1_BR);
	SPI2->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_SPE | SPI_CR1_MSTR; 	
}
//===============================================================================
void SPI_Transmit(uint8_t* data, uint32_t size){
	uint32_t i = 0;
	uint8_t temp = 0;
	char str[100];
	temp = SPI1->DR;
	temp = SPI1->SR;
	
	while(i < size){
		while(!(SPI1->SR & SPI_SR_TXE));
		SPI1->DR = data[i];
		/*sprintf(str, "SPI->DR: %02X\r\n", SPI1->DR);
		UART_SendString(str);*/
		while((SPI1->SR & SPI_SR_BSY));
		i++;
	}
	while(!(SPI1->SR & SPI_SR_TXE));
	while(SPI1->SR & SPI_SR_BSY);
	temp = SPI1->DR;
	temp = SPI1->SR;
}
//===============================================================================
void SPI_Receive(uint8_t* data, uint32_t size){
	
	while(size){
		SPI1->DR = 0;
		while(!(SPI1->SR & SPI_SR_RXNE));
		*data++ = SPI1->DR;
		size--;
	}
}
//===============================================================================
void SPI_TransmitReceive(uint8_t* wData, uint8_t* rData, uint32_t size){
	uint32_t i = 0, y = 0;
	uint8_t temp = 0;
	temp = SPI1->DR;
	temp = SPI1->SR;
	while(i < size){
		while(!(SPI1->SR & SPI_SR_TXE));
		SPI1->DR = wData[i];
		while((SPI1->SR & SPI_SR_BSY));
		i++;
	}
	//while(!(SPI1->SR & SPI_SR_TXE));
	//while(SPI1->SR & SPI_SR_BSY);
	while((SPI1->SR & SPI_SR_BSY));
	temp = SPI1->DR;
	//temp = SPI1->SR;
	//i = 0;
	while(y < size){
		
		SPI1->DR = 0;
		while(!(SPI1->SR & SPI_SR_RXNE));
		rData[y] = SPI1->DR;
		while((SPI1->SR & SPI_SR_BSY));
		y++;
	}
}
//===============================================================================
