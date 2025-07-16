#include "BMP280.h"
#include "I2C.h"
#include "RCC.h"

static uint8_t BMP_ReadRegister(uint8_t Address, uint8_t Register){
	return I2C_ReadData(I2C1, Address, Register);
}

static void BMP_WriteRegister(uint8_t Address, uint8_t Register, uint8_t Data){
	I2C_WriteData(I2C1, Address, Register, Data);
}

void BMP_Init(BMP_TypeDef* BMP_Calibration){
	uint8_t status;
	status = BMP_ReadRegister(BMP_ADDRESS, BMP_REGISTER_ID);
	BMP_Calibration->Status = status;
	
	BMP_WriteRegister(BMP_ADDRESS, BMP_REGISTER_CTRL_MEAS, 0xFF);
	BMP_WriteRegister(BMP_ADDRESS, BMP_REGISTER_CONFIG, 	 0x1C);
}

void BMP_Calibration(BMP_TypeDef* BMP_Calibration){
	BMP_Calibration->T1 = (BMP_ReadRegister(BMP_ADDRESS, BMP_REGISTER_CALIB_00)) | 
												(BMP_ReadRegister(BMP_ADDRESS, BMP_REGISTER_CALIB_01) << 8);
	BMP_Calibration->T2 = (BMP_ReadRegister(BMP_ADDRESS, BMP_REGISTER_CALIB_02)) | 
												(BMP_ReadRegister(BMP_ADDRESS, BMP_REGISTER_CALIB_03) << 8);
	BMP_Calibration->T3 = (BMP_ReadRegister(BMP_ADDRESS, BMP_REGISTER_CALIB_04)) | 
												(BMP_ReadRegister(BMP_ADDRESS, BMP_REGISTER_CALIB_05) << 8);
	BMP_Calibration->P1 = (BMP_ReadRegister(BMP_ADDRESS, BMP_REGISTER_CALIB_06)) | 
												(BMP_ReadRegister(BMP_ADDRESS, BMP_REGISTER_CALIB_07) << 8);
	BMP_Calibration->P2 = (BMP_ReadRegister(BMP_ADDRESS, BMP_REGISTER_CALIB_08)) | 
												(BMP_ReadRegister(BMP_ADDRESS, BMP_REGISTER_CALIB_09) << 8);
	BMP_Calibration->P3 = (BMP_ReadRegister(BMP_ADDRESS, BMP_REGISTER_CALIB_10)) | 
												(BMP_ReadRegister(BMP_ADDRESS, BMP_REGISTER_CALIB_11) << 8);
	BMP_Calibration->P4 = (BMP_ReadRegister(BMP_ADDRESS, BMP_REGISTER_CALIB_12)) | 
												(BMP_ReadRegister(BMP_ADDRESS, BMP_REGISTER_CALIB_13) << 8);
	BMP_Calibration->P5 = (BMP_ReadRegister(BMP_ADDRESS, BMP_REGISTER_CALIB_14)) | 
												(BMP_ReadRegister(BMP_ADDRESS, BMP_REGISTER_CALIB_15) << 8);
	BMP_Calibration->P6 = (BMP_ReadRegister(BMP_ADDRESS, BMP_REGISTER_CALIB_16)) | 
												(BMP_ReadRegister(BMP_ADDRESS, BMP_REGISTER_CALIB_17) << 8);
	BMP_Calibration->P7 = (BMP_ReadRegister(BMP_ADDRESS, BMP_REGISTER_CALIB_18)) | 
												(BMP_ReadRegister(BMP_ADDRESS, BMP_REGISTER_CALIB_19) << 8);
	BMP_Calibration->P8 = (BMP_ReadRegister(BMP_ADDRESS, BMP_REGISTER_CALIB_20)) | 
												(BMP_ReadRegister(BMP_ADDRESS, BMP_REGISTER_CALIB_21) << 8);
	BMP_Calibration->P9 = (BMP_ReadRegister(BMP_ADDRESS, BMP_REGISTER_CALIB_22)) | 
												(BMP_ReadRegister(BMP_ADDRESS, BMP_REGISTER_CALIB_23) << 8);
}

void BMP_Read_Temperature(BMP_TypeDef* BMP_Struct){
	
	while(BMP_ReadRegister(BMP_ADDRESS, BMP_REGISTER_STATUS) & 0x08 && BMP_ReadRegister(BMP_ADDRESS, BMP_REGISTER_STATUS) & 0x01);
	
	uint32_t temp[3];
	
	temp[2] = BMP_ReadRegister(BMP_ADDRESS, BMP_REGISTER_TEMP_MSB);
	temp[1] = BMP_ReadRegister(BMP_ADDRESS, BMP_REGISTER_TEMP_LSB);
	temp[0] = BMP_ReadRegister(BMP_ADDRESS, BMP_REGISTER_TEMP_XLSB);
	
	BMP_Struct->Temperature_Raw = (temp[2] << 12) + (temp[1] << 4) + (temp[0] >> 4);
	
	double var1, var2;
	var1 = (((double) BMP_Struct->Temperature_Raw) / 16384.0 - ((double) BMP_Struct->T1) / 1024.0) * ((double) BMP_Struct->T2);
	var2 = ((((double) BMP_Struct->Temperature_Raw) / 131072.0 - ((double) BMP_Struct->T1) / 8192.0) * (((double) BMP_Struct->Temperature_Raw) / 131072.0	- ((double) BMP_Struct->T1) / 8192.0)) * ((double) BMP_Struct->T3);
		
	volatile float Temp = (var1 + var2) / 5120.0;
					
	BMP_Struct->Temperature = Temp;
}

void BMP_Read_All(BMP_TypeDef* BMP_Struct){
	
	while(BMP_ReadRegister(BMP_ADDRESS, BMP_REGISTER_STATUS) & 0x08 && BMP_ReadRegister(BMP_ADDRESS, BMP_REGISTER_STATUS) & 0x01);
	
	volatile uint32_t temp[3];
	
	temp[2] = BMP_ReadRegister(BMP_ADDRESS, BMP_REGISTER_TEMP_MSB);
	temp[1] = BMP_ReadRegister(BMP_ADDRESS, BMP_REGISTER_TEMP_LSB);
	temp[0] = BMP_ReadRegister(BMP_ADDRESS, BMP_REGISTER_TEMP_XLSB);
	BMP_Struct->Temperature_Raw = (temp[2] << 12) + (temp[1] << 4) + (temp[0] >> 4);

	temp[2] = BMP_ReadRegister(BMP_ADDRESS, BMP_REGISTER_PRESS_MSB);
	temp[1] = BMP_ReadRegister(BMP_ADDRESS, BMP_REGISTER_PRESS_LSB);
	temp[0] = BMP_ReadRegister(BMP_ADDRESS, BMP_REGISTER_PRESS_XLSB);
	BMP_Struct->Pressure_Raw = (temp[2] << 12) + (temp[1] << 4) + (temp[0] >> 4);

	double var1, var2;
	var1 = (((double) BMP_Struct->Temperature_Raw) / 16384.0
			- ((double) BMP_Struct->T1) / 1024.0) * ((double) BMP_Struct->T2);
	var2 = ((((double) BMP_Struct->Temperature_Raw) / 131072.0
			- ((double) BMP_Struct->T1) / 8192.0)
			* (((double) BMP_Struct->Temperature_Raw) / 131072.0
					- ((double) BMP_Struct->T1) / 8192.0))
			* ((double) BMP_Struct->T3);

	double t_fine = (int32_t) (var1 + var2);
	BMP_Struct->Temperature = (var1 + var2) / 5120.0;

	var1 = ((double) t_fine / 2.0) - 64000.0;
	var2 = var1 * var1 * ((double) BMP_Struct->P6) / 32768.0;
	var2 = var2 + var1 * ((double) BMP_Struct->P5) * 2.0;
	var2 = (var2 / 4.0) + (((double) BMP_Struct->P4) * 65536.0);
	var1 = (((double) BMP_Struct->P3) * var1 * var1 / 524288.0
			+ ((double) BMP_Struct->P2) * var1) / 524288.0;
	var1 = (1.0 + var1 / 32768.0) * ((double) BMP_Struct->P1);
	
	volatile double p = 1048576.0 - (double) BMP_Struct->Pressure_Raw;
	p = (p - (var2 / 4096.0)) * 6250.0 / var1;
	var1 = ((double) BMP_Struct->P9) * p * p / 2147483648.0;
	var2 = p * ((double) BMP_Struct->P8) / 32768.0;
	p = p + (var1 + var2 + ((double) BMP_Struct->P7)) / 16.0;

	BMP_Struct->Pressure = p;
}



