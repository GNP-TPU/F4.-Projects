//-------------------------------------------------------------------------------------------//
#include "DHT11.h"
#include "TIM.h"
//-------------------------------------------------------------------------------------------//
int i = 0, cycle = 0;

uint8_t counter, humidity_0, temperature_0, humidity_1, temperature_1, parity_bit, hbyte, tbyte, array_index, hhh, sum;
uint16_t flag = 0, prev_flag, error = 0, temperature, humidity;
uint8_t HT [40] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0
};

void DHT11_Init(void){
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	
	GPIOA->MODER &= ~GPIO_MODER_MODE8;
	GPIOA->MODER |= GPIO_MODER_MODE8_0;
	
	GPIOA->OTYPER |= GPIO_OTYPER_OT8;
	
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR8;
	
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD8;
	
	GPIOA->ODR |= GPIO_ODR_OD8;
}

uint16_t DHT11_ReadTemp1(void){
	  temperature = 0;
		humidity_0 = 0;
		humidity_1 = 0;
		temperature_0 = 0;
		temperature_1 = 0;
		parity_bit = 0;
		counter = 0;
		sum = 0;
		
		GPIOA->ODR &= ~GPIO_ODR_OD8;
		Delay_us(20000);
		GPIOA->ODR |= GPIO_ODR_OD8;
		//respond beginning
		Delay_us(140);
		
		while (counter != 40){
			hhh = 0;
		  while((GPIOA->IDR & GPIO_IDR_ID8) && (hhh < 100)){
				hhh++;
			}
			while(!(GPIOA->IDR & (1 << 8)));
			Delay_us(40);
			if (!(GPIOA->IDR & (1 << 8))){
				HT[counter] = 0;
			}
			else{
				HT[counter] = 1;
			}
			counter++;
		}
		
		array_index = 0;
		for(hbyte = 8; hbyte >= 1; hbyte--){
			humidity_0 |= HT[array_index] << (hbyte - 1);
			array_index++;
		}
		array_index = 8;
		for(hbyte = 8; hbyte >= 1; hbyte--){
			humidity_1 |= HT[array_index] << (hbyte - 1);
			array_index++;
		}
			array_index = 16;
		for(tbyte = 8; tbyte >= 1; tbyte--){
			temperature_0 |= HT[array_index] << (tbyte - 1);
			array_index++;
		}
		array_index = 24;
		for(tbyte = 8; tbyte >= 1; tbyte--){
			temperature_1 |= HT[array_index] << (tbyte - 1);
			array_index++;
		}
		array_index = 32;
		for(tbyte = 8; tbyte >= 1; tbyte--){
			parity_bit |= HT[array_index] << (tbyte - 1);
			array_index++;
		}
		sum = humidity_0 + humidity_1 + temperature_0 + temperature_1;
		if(sum == parity_bit){
			Delay_us(50000);
			Delay_us(50000);
			Delay_us(50000);
			Delay_us(50000);
			Delay_us(50000);
			Delay_us(50000);
			Delay_us(50000);
			Delay_us(50000);
			Delay_us(50000);
			Delay_us(50000);
		}
		temperature = (temperature_0 << 8)+(temperature_1);
		return temperature;
}


uint16_t DHT11_ReadHumi1(void){
	  temperature = 0;
	  humidity = 0;
		humidity_0 = 0;
		humidity_1 = 0;
		temperature_0 = 0;
		temperature_1 = 0;
		parity_bit = 0;
		counter = 0;
		sum = 0;
		
		GPIOA->ODR &= ~GPIO_ODR_OD8;
		Delay_us(20000);
		GPIOA->ODR |= GPIO_ODR_OD8;
		//respond beginning
		Delay_us(140);
		
		while (counter != 40){
			hhh = 0;
		  while((GPIOA->IDR & GPIO_IDR_ID8) && (hhh < 100)){
				hhh++;
			}
			while(!(GPIOA->IDR & (1 << 8)));
			Delay_us(40);
			if (!(GPIOA->IDR & (1 << 8))){
				HT[counter] = 0;
			}
			else{
				HT[counter] = 1;
			}
			counter++;
		}
		
		array_index = 0;
		for(hbyte = 8; hbyte >= 1; hbyte--){
			humidity_0 |= HT[array_index] << (hbyte - 1);
			array_index++;
		}
		array_index = 8;
		for(hbyte = 8; hbyte >= 1; hbyte--){
			humidity_1 |= HT[array_index] << (hbyte - 1);
			array_index++;
		}
			array_index = 16;
		for(tbyte = 8; tbyte >= 1; tbyte--){
			temperature_0 |= HT[array_index] << (tbyte - 1);
			array_index++;
		}
		array_index = 24;
		for(tbyte = 8; tbyte >= 1; tbyte--){
			temperature_1 |= HT[array_index] << (tbyte - 1);
			array_index++;
		}
		array_index = 32;
		for(tbyte = 8; tbyte >= 1; tbyte--){
			parity_bit |= HT[array_index] << (tbyte - 1);
			array_index++;
		}
		sum = humidity_0 + humidity_1 + temperature_0 + temperature_1;
		if(sum == parity_bit){
			Delay_us(50000);
			Delay_us(50000);
			Delay_us(50000);
			Delay_us(50000);
			Delay_us(50000);
			Delay_us(50000);
			Delay_us(50000);
			Delay_us(50000);
			Delay_us(50000);
			Delay_us(50000);
		}
		humidity = (humidity_0 << 8)+(humidity_1);
		return humidity;
}
//-------------------------------------------------------------------------------------------//
