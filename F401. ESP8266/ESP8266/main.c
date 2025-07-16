//-------------------------------------------------------------------------------------------//
#include <stm32f401xe.h>
#include <stdio.h>
#include <string.h>
//-------------------------------------------------------------------------------------------//
#include "main.h"
#include "UART.h"
#include "TIM.h"
#include "DHT11.h"
#include "ESP-01.h"
//-------------------------------------------------------------------------------------------//
char str = 0, recieve[590], command = 0;
uint16_t T, H;
uint8_t init_flag = 0;
extern uint8_t buffer_counter;
extern char buffer[200];
//-------------------------------------------------------------------------------------------//

int main(){
	UART_Init();
	TIM_Init();
	//DHT11_Init();
	while(1){
		if(init_flag == 0){
			ESP_Init();
			init_flag = 1;
		}
		else if(init_flag == 1){
			while(init_flag){
				Server_Start();
			}
		}
	}
}
//-------------------------------------------------------------------------------------------//
void USART2_IRQHandler(void){
	str = 0;
	while(USART2->SR & USART_SR_RXNE){
		str = USART2->DR;
	}
	SendString1(&str);
}
//-------------------------------------------------------------------------------------------//
void USART1_IRQHandler(void){
	while(USART1->SR & USART_SR_RXNE){
		str = USART1->DR;
	}
	SendString2(&str);
	buffer[buffer_counter] = str;
	buffer_counter++;
}
//-------------------------------------------------------------------------------------------//
