#include "I2C.h"

void I2C_Init(I2C_TypeDef* I2Cx, I2C_InitTypeDef* I2Cx_Struct){
	
	I2Cx->CR1 = 0x00;
	I2Cx->CR2 = 0x00;
	
	switch((uint32_t)I2Cx){
		case (uint32_t)I2C1:
			RCC->APB1ENR |= RCC_APB1ENR_I2C1EN; 
			break;
		case (uint32_t)I2C2:
			RCC->APB1ENR |= RCC_APB1ENR_I2C2EN; 
			break;
		case (uint32_t)I2C3:
			RCC->APB1ENR |= RCC_APB1ENR_I2C3EN; 
			break;
	}
	
	I2Cx->CR2 |= I2Cx_Struct->PeripheralBusFrequency / 1000000; //APB1 bus freq
	
	I2Cx->CCR |= I2Cx_Struct->PeripheralBusFrequency / (2 * 100000);
	
	I2Cx->TRISE &= ~0xFF;
	I2Cx->TRISE |= (I2Cx_Struct->PeripheralBusFrequency / 1000000) + 1;
	
	I2Cx->CR1 = I2C_CR1_PE; //enable peripheral, needs to be done last
}

void I2C_WriteData(I2C_TypeDef* I2Cx, uint8_t Address, uint8_t Register, uint8_t Data){
	uint32_t temp;
	
	I2Cx->CR1 |= I2C_CR1_START;
	// while(!(I2Cx->SR1 & I2C_SR1_SB));
	for(uint32_t Attempt_Number = 0; Attempt_Number < I2C_MAX_ATTEMPTS; Attempt_Number++){
		if(I2Cx->SR1 & I2C_SR1_SB) break;
	}
	
	I2Cx->DR = Address;
	// while(!(I2C1->SR1 & I2C_SR1_ADDR));
	for(uint32_t Attempt_Number = 0; Attempt_Number < I2C_MAX_ATTEMPTS; Attempt_Number++){
		if(I2Cx->SR1 & I2C_SR1_ADDR) break;
	}
	temp = I2Cx->SR2;
	
	I2Cx->DR = Register;
	// while(!(I2Cx->SR1 & I2C_SR1_TXE));
	for(uint32_t Attempt_Number = 0; Attempt_Number < I2C_MAX_ATTEMPTS; Attempt_Number++){
		if(I2Cx->SR1 & I2C_SR1_TXE) break;
	}
	
	I2Cx->DR = Data;
		// while(!(I2Cx->SR1 & I2C_SR1_TXE));
	for(uint32_t Attempt_Number = 0; Attempt_Number < I2C_MAX_ATTEMPTS; Attempt_Number++){
		if(I2Cx->SR1 & I2C_SR1_TXE) break;
	}
	
	I2Cx->CR1 |= I2C_CR1_STOP;
}

uint8_t I2C_ReadData(I2C_TypeDef* I2Cx, uint8_t Address, uint8_t Register){
	uint32_t temp;
	uint8_t rx = 0;
	
	I2Cx->CR1 |= I2C_CR1_START;
	// while(!(I2Cx->SR1 & I2C_SR1_SB));
	for(uint32_t Attempt_Number = 0; Attempt_Number < I2C_MAX_ATTEMPTS; Attempt_Number++){
		if(I2Cx->SR1 & I2C_SR1_SB){ 
			break;
		}
	}
	
	I2Cx->DR = Address;
	// while(!(I2Cx->SR1 & I2C_SR1_ADDR));
	for(uint32_t Attempt_Number = 0; Attempt_Number < I2C_MAX_ATTEMPTS; Attempt_Number++){
		if(I2Cx->SR1 & I2C_SR1_ADDR){ 
			I2Cx->CR1 |= I2C_CR1_ACK; 
			break;
		}
	}
	temp = I2Cx->SR2;
	
	I2Cx->DR = Register;
	// while(!(I2Cx->SR1 & I2C_SR1_TXE));
	for(uint32_t Attempt_Number = 0; Attempt_Number < I2C_MAX_ATTEMPTS; Attempt_Number++){
		if(I2Cx->SR1 & I2C_SR1_TXE) break;
	}
	I2Cx->CR1 |= I2C_CR1_STOP;
	
	__NOP();
	
	I2Cx->CR1 |= I2C_CR1_START;
	// while(!(I2Cx->SR1 & I2C_SR1_SB));
	for(uint32_t Attempt_Number = 0; Attempt_Number < I2C_MAX_ATTEMPTS; Attempt_Number++){
		if(I2Cx->SR1 & I2C_SR1_SB) break;
	}
	
	I2Cx->DR = Address + 1;
	// while(!(I2Cx->SR1 & I2C_SR1_ADDR));
	for(uint32_t Attempt_Number = 0; Attempt_Number < I2C_MAX_ATTEMPTS; Attempt_Number++){
		if(I2Cx->SR1 & I2C_SR1_ADDR) break;
	}
	temp = I2Cx->SR1;
	temp = I2Cx->SR2;
	
	// while(!(I2Cx->SR1 & I2C_SR1_RXNE));
	for(uint32_t Attempt_Number = 0; Attempt_Number < I2C_MAX_ATTEMPTS; Attempt_Number++){
		if(I2Cx->SR1 & I2C_SR1_RXNE) break;
	}
	
	I2Cx->CR1 &= ~I2C_CR1_ACK;
	I2Cx->CR1 |= I2C_CR1_STOP;
	
	rx = I2Cx->DR;
	
	return rx;
}

