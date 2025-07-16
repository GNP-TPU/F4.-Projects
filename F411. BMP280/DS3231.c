#include "DS3231.h"
//====================================================================================================
uint8_t DS3231_Get_Seconds(void){
	uint8_t byte = I2C_ReadData(I2C1, DS3231_ADDRESS, DS3231_SECONDS_REG);
	return byte;
}

uint8_t DS3231_Get_Minutes(void){
	uint8_t byte = I2C_ReadData(I2C1, DS3231_ADDRESS, DS3231_MINUTES_REG);
	return byte;
}

uint8_t DS3231_Get_Hour(void){
	uint8_t byte = I2C_ReadData(I2C1, DS3231_ADDRESS, DS3231_HOURS_REG);
	return byte;
}

uint8_t DS3231_Get_Day(void){
	uint8_t byte = I2C_ReadData(I2C1, DS3231_ADDRESS, DS3231_DAY_REG);
	return byte;
}

uint8_t DS3231_Get_Date(void){
	uint8_t byte = I2C_ReadData(I2C1, DS3231_ADDRESS, DS3231_DATE_REG);
	return byte;
}

uint8_t DS3231_Get_Month(void){
	uint8_t byte = I2C_ReadData(I2C1, DS3231_ADDRESS, DS3231_MONTH_REG);
	return byte;
}

uint8_t DS3231_Get_Year(void){
	uint8_t byte = I2C_ReadData(I2C1, DS3231_ADDRESS, DS3231_YEAR_REG);
	return byte;
}

void DS3231_Get_Data(DS3231_TypeDef* DS3231_Data){
	// uint8_t reg_data = I2C_ReadData(I2C1, DS3231_ADDRESS, DS3231_CONTROL_REG);							
	// I2C_WriteData(I2C1, DS3231_ADDRESS, DS3231_CONTROL_REG, reg_data | (1 << 5));    		// Start temperature conversion
																																											// to get actual temperature
	
	DS3231_Data->Seconds 	= I2C_ReadData(I2C1, DS3231_ADDRESS, DS3231_SECONDS_REG);
	DS3231_Data->Minutes 	= I2C_ReadData(I2C1, DS3231_ADDRESS, DS3231_MINUTES_REG);
	DS3231_Data->Hours 		= I2C_ReadData(I2C1, DS3231_ADDRESS, DS3231_HOURS_REG);
	DS3231_Data->Day 			= I2C_ReadData(I2C1, DS3231_ADDRESS, DS3231_DAY_REG);
	DS3231_Data->Date 		= I2C_ReadData(I2C1, DS3231_ADDRESS, DS3231_DATE_REG);
	DS3231_Data->Month 		= I2C_ReadData(I2C1, DS3231_ADDRESS, DS3231_MONTH_REG);
	DS3231_Data->Year			= I2C_ReadData(I2C1, DS3231_ADDRESS, DS3231_YEAR_REG);
		
	// DS3231_Data->Temperature = I2C_ReadData(I2C1, DS3231_ADDRESS, DS3231_TEMP_MSB_REG);
}
//====================================================================================================
void DS3231_Set_Seconds(uint8_t seconds){
	if((seconds >= 0) && (seconds < 60))
		I2C_WriteData(I2C1, DS3231_ADDRESS, DS3231_SECONDS_REG, seconds);
}

void DS3231_Set_Minutes(uint8_t minutes){
	if((minutes >= 0) && (minutes < 60))
		I2C_WriteData(I2C1, DS3231_ADDRESS, DS3231_MINUTES_REG, minutes);
}

void DS3231_Set_Hour(uint8_t hours){
	if((hours >= 0) && (hours < 24))
		I2C_WriteData(I2C1, DS3231_ADDRESS, DS3231_HOURS_REG, hours);
}
void DS3231_Set_Day(uint8_t day){
	if((day > 0) && (day < 8))
		I2C_WriteData(I2C1, DS3231_ADDRESS, DS3231_DAY_REG, day);
}

void DS3231_Set_Date(uint8_t date){
	if((date > 0) && (date < 31))
		I2C_WriteData(I2C1, DS3231_ADDRESS, DS3231_DATE_REG, date);
}

void DS3231_Set_Month(uint8_t month){
	if((month > 0) && (month < 13))
		I2C_WriteData(I2C1, DS3231_ADDRESS, DS3231_MONTH_REG, month);
}

void DS3231_Set_Year(uint8_t year){
	if((year >= 0) && (year < 100))
		I2C_WriteData(I2C1, DS3231_ADDRESS, DS3231_YEAR_REG, year);
}

void DS3231_Set_Data(DS3231_TypeDef* DS3231_Data){
	I2C_WriteData(I2C1, DS3231_ADDRESS, DS3231_SECONDS_REG, DS3231_Data->Seconds);
	I2C_WriteData(I2C1, DS3231_ADDRESS, DS3231_MINUTES_REG, DS3231_Data->Minutes);
	I2C_WriteData(I2C1, DS3231_ADDRESS, DS3231_HOURS_REG, 	DS3231_Data->Hours);
	
	if(DS3231_Data->Day > 0)
		I2C_WriteData(I2C1, DS3231_ADDRESS, DS3231_DAY_REG, 	DS3231_Data->Day);
	if(DS3231_Data->Date > 0)
		I2C_WriteData(I2C1, DS3231_ADDRESS, DS3231_DATE_REG, 		DS3231_Data->Date);
	if(DS3231_Data->Month > 0)
		I2C_WriteData(I2C1, DS3231_ADDRESS, DS3231_MONTH_REG, 	DS3231_Data->Month);
	
	I2C_WriteData(I2C1, DS3231_ADDRESS, DS3231_YEAR_REG, 		DS3231_Data->Year);
}
//====================================================================================================
