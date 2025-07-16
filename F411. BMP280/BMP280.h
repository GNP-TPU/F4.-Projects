//====================================================================================================
#ifndef BMP280_H
#define BMP280_H
//====================================================================================================
#include <stm32f4xx.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
//====================================================================================================
#define BMP_ADDRESS 							0xEC

#define BMP_REGISTER_ID						0xD0
#define BMP_REGISTER_RESET				0xE0
#define BMP_REGISTER_STATUS				0xF3
#define BMP_REGISTER_CTRL_MEAS		0xF4
#define BMP_REGISTER_CONFIG				0xF5

#define BMP_REGISTER_PRESS_MSB		0xF7
#define BMP_REGISTER_PRESS_LSB		0xF8
#define BMP_REGISTER_PRESS_XLSB		0xF9

#define BMP_REGISTER_TEMP_MSB			0xFA
#define BMP_REGISTER_TEMP_LSB			0xFB
#define BMP_REGISTER_TEMP_XLSB		0xFC

#define BMP_REGISTER_CALIB_00			0x88
#define BMP_REGISTER_CALIB_01			0x89
#define BMP_REGISTER_CALIB_02			0x8A
#define BMP_REGISTER_CALIB_03			0x8B
#define BMP_REGISTER_CALIB_04			0x8C
#define BMP_REGISTER_CALIB_05			0x8D
#define BMP_REGISTER_CALIB_06			0x8E
#define BMP_REGISTER_CALIB_07			0x8F
#define BMP_REGISTER_CALIB_08			0x90
#define BMP_REGISTER_CALIB_09			0x91
#define BMP_REGISTER_CALIB_10			0x92
#define BMP_REGISTER_CALIB_11			0x93
#define BMP_REGISTER_CALIB_12			0x94
#define BMP_REGISTER_CALIB_13			0x95
#define BMP_REGISTER_CALIB_14			0x96
#define BMP_REGISTER_CALIB_15			0x97
#define BMP_REGISTER_CALIB_16			0x98
#define BMP_REGISTER_CALIB_17			0x99
#define BMP_REGISTER_CALIB_18			0x9A
#define BMP_REGISTER_CALIB_19			0x9B
#define BMP_REGISTER_CALIB_20			0x9C
#define BMP_REGISTER_CALIB_21			0x9D
#define BMP_REGISTER_CALIB_22			0x9E
#define BMP_REGISTER_CALIB_23			0x9F
#define BMP_REGISTER_CALIB_24			0xA0
#define BMP_REGISTER_CALIB_25			0xA1
//====================================================================================================
typedef void(*I2C_WriteRegister_Function)(void);
typedef void(*I2C_ReadRegister_Function)(void);

typedef struct{
	uint8_t Status;
	
	uint16_t T1, P1;
	
	int16_t T2, T3, P2, P3, P4, P5, P6, P7, P8, P9;
	
	int32_t Temperature_Raw, Pressure_Raw;
	float Temperature, Pressure;
	
} BMP_TypeDef;

void BMP_Init(BMP_TypeDef*);
void BMP_Calibration(BMP_TypeDef*);
void BMP_Read_Temperature(BMP_TypeDef*);
void BMP_Read_All(BMP_TypeDef*);
//====================================================================================================
#endif
//====================================================================================================