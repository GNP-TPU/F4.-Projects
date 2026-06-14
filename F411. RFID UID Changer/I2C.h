//====================================================================================================
#ifndef I2C_H
#define I2C_H
//====================================================================================================
#include <stm32f4xx.h>
#include <string.h>
#include <stdio.h>
//====================================================================================================
#define I2C_MAX_ATTEMPTS		100000
//====================================================================================================
typedef struct{
	uint32_t 	PeripheralBusFrequency;
	uint8_t 	Mode;
	
	uint8_t  	Address;
	uint8_t		Register;
	uint8_t		Data;
}I2C_InitTypeDef;

void I2C_Init(I2C_TypeDef*, I2C_InitTypeDef*);
void I2C_WriteData(I2C_TypeDef* I2Cx, uint8_t Address, uint8_t Register, uint8_t Data);
uint8_t I2C_ReadData(I2C_TypeDef* I2Cx, uint8_t Address, uint8_t Register);
//====================================================================================================
#endif
//====================================================================================================