//-------------------------------------------------------------------------------------------//
#include "UART.h"
#include "DHT11.h"
#include "TIM.h"
//-------------------------------------------------------------------------------------------//
void UART_Init(void){
	// 1. Enable the UART CLOCK and GPIO CLOCK
	RCC->APB1ENR |= (1<<17);  // Enable UART2 CLOCK
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	RCC->AHB1ENR |= (1<<0); // Enable GPIOA CLOCK
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
	
	// 2. Configure the UART PINs for ALternate Functions
	GPIOA->MODER |= (2<<4);  // Bits (5:4)= 1:0 --> Alternate Function for Pin PA2
	GPIOA->MODER |= (2<<6);  // Bits (7:6)= 1:0 --> Alternate Function for Pin PA3
	GPIOB->MODER |= GPIO_MODER_MODE6_1;
	GPIOB->MODER |= GPIO_MODER_MODE7_1;
	
	GPIOA->OSPEEDR |= (3<<4) | (3<<6);  // Bits (5:4)= 1:1 and Bits (7:6)= 1:1 --> High Speed for PIN PA2 and PA3
	GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR6;
	GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR7;
	
	GPIOA->AFR[0] |= (7<<8);  // Bytes (11:10:9:8) = 0:1:1:1  --> AF7 Alternate function for USART2 at Pin PA2
	GPIOA->AFR[0] |= (7<<12); // Bytes (15:14:13:12) = 0:1:1:1  --> AF7 Alternate function for USART2 at Pin PA3
	GPIOB->AFR[0] |= (7<<24);
	GPIOB->AFR[0] |= (7<<28);
	
	// 3. Enable the USART by writing the UE bit in USART_CR1 register to 1.
	USART2->CR1 = 0x00;  // clear all
	USART2->CR1 |= (1<<13);  // UE = 1... Enable USART
	
	// 4. Program the M bit in USART_CR1 to define the word length.
	USART2->CR1 &= ~(1<<12);  // M =0; 8 bit word length
	
	// 5. Select the desired baud rate using the USART_BRR register.
	USART2->BRR = 0x08A;   // Baud rate of 9600, PCLK1 at 16MHz
	
	// 6. Enable the Transmitter/Receiver by Setting the TE and RE bits in USART_CR1 Register
	USART2->CR1 |= (1<<2); // RE=1.. Enable the Receiver
	USART2->CR1 |= (1<<3);  // TE=1.. Enable Transmitter
	USART2->CR1 |= USART_CR1_RXNEIE;
	NVIC_EnableIRQ(USART2_IRQn);
	
	// 3. Enable the USART by writing the UE bit in USART_CR1 register to 1.
	USART1->CR1 &= ~(0xFF);  // clear all
	USART1->CR1 |= (1<<13);  // UE = 1... Enable USART
	
	// 4. Program the M bit in USART_CR1 to define the word length.
	USART1->CR1 &= ~(1<<12);  // M =0; 8 bit word length
	
	// 5. Select the desired baud rate using the USART_BRR register.
	USART1->BRR = 0x08A;   // Baud rate of 9600, PCLK1 at 16MHz
	
	// 6. Enable the Transmitter/Receiver by Setting the TE and RE bits in USART_CR1 Register
	USART1->CR1 |= (1<<2); // RE=1.. Enable the Receiver
	USART1->CR1 |= (1<<3);  // TE=1.. Enable Transmitter
	USART1->CR1 |= USART_CR1_RXNEIE;
	//NVIC_SetPriority(USART2_IRQn, 2);
	NVIC_EnableIRQ(USART1_IRQn);
}
//-------------------------------------------------------------------------------------------//
void SendChar1(uint8_t c){
	while(!( USART1->SR & 0x80)); // wait until we are able to transmit
	USART1->DR = c; // transmit the character
	
}
//-------------------------------------------------------------------------------------------//
void SendString1(char* s){
	while(*s)
		SendChar1(*s++);
}
//-------------------------------------------------------------------------------------------//
void SendChar2(uint8_t c){
	while(!( USART2->SR & 0x80)); // wait until we are able to transmit
  USART2->DR = c; // transmit the character
	
}
//-------------------------------------------------------------------------------------------//
void SendString2(char* s){
	while(*s)
		SendChar2(*s++);
}

void SendTemp(uint16_t temp){
		SendString1("Temperature: ");
		SendChar1((char)(((temp >> 8)/10) + 0x30));
		SendChar1((char)(((temp >> 8)%10) + 0x30));
		SendString1(".");
		SendChar1((char)(((temp & 0x0F)%10) + 0x30));
		SendString1(" C");
}

void SendHumi(uint16_t humi){
		SendString1("Humidity: ");
		SendChar1((char)(((humi >> 8)/10) + 0x30));
		SendChar1((char)(((humi >> 8)%10) + 0x30));
	 	SendString1(".");
	 	SendChar1((char)(((humi & 0x0F)%10) + 0x30));
		SendString1(" %");
}
//-------------------------------------------------------------------------------------------//
