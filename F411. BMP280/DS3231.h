//====================================================================================================
#ifndef DS3231_H
#define DS3231_H
//====================================================================================================
#include <stm32f4xx.h>

#include <string.h>
#include <stdio.h>

#include "I2C.h"
//====================================================================================================
#define DS3231_ADDRESS				0xD0
//====================================================================================================
#define DS3231_SECONDS_REG		0x00
#define DS3231_MINUTES_REG		0x01
#define DS3231_HOURS_REG			0x02
#define DS3231_DAY_REG				0x03
#define DS3231_DATE_REG				0x04
#define DS3231_MONTH_REG			0x05
#define DS3231_YEAR_REG				0x06

#define DS3231_CONTROL_REG		0x0E

#define DS3231_TEMP_MSB_REG		0x11
#define DS3231_TEMP_LSB_REG		0x12
//====================================================================================================
#define DS3231_MONDAY				0x01
#define DS3231_TUESDAY			0x02
#define DS3231_WEDNESDAY		0x03
#define DS3231_THURSDAY			0x04
#define DS3231_FRIDAY				0x05
#define DS3231_SATURDAY			0x06
#define DS3231_SUNDAY				0x07
//====================================================================================================
#define DS3231_JANUARY			0x01
#define DS3231_FEBRUARY			0x02
#define DS3231_MARCH				0x03
#define DS3231_APRIL				0x04
#define DS3231_MAY					0x05
#define DS3231_JUNE					0x06
#define DS3231_JULY					0x07
#define DS3231_AUGUST				0x08
#define DS3231_SEPTEMBER		0x09
#define DS3231_OCTOBER			0x10
#define DS3231_NOVEMBER			0x11
#define DS3231_DECEMBER			0x12
//====================================================================================================
typedef struct{
	uint8_t  	Seconds;
	uint8_t		Minutes;
	uint8_t 	Hours;
	uint8_t 	Day;
	uint8_t 	Date;
	uint8_t 	Month;
	uint8_t 	Year;
	
	uint8_t 	Temperature;
}DS3231_TypeDef;
//====================================================================================================
uint8_t DS3231_Get_Seconds(void);
uint8_t DS3231_Get_Minutes(void);
uint8_t DS3231_Get_Hour(void);
uint8_t DS3231_Get_Day(void);
uint8_t DS3231_Get_Date(void);
uint8_t DS3231_Get_Month(void);
uint8_t DS3231_Get_Year(void);
uint8_t DS3231_Get_Temperature(void);

void DS3231_Get_Data(DS3231_TypeDef*);

uint8_t DS3231_Get_Seconds(void);
uint8_t DS3231_Get_Minutes(void);
uint8_t DS3231_Get_Hour(void);
uint8_t DS3231_Get_Day(void);
uint8_t DS3231_Get_Date(void);
uint8_t DS3231_Get_Month(void);
uint8_t DS3231_Get_Year(void);

void DS3231_Set_Seconds(uint8_t);
void DS3231_Set_Minutes(uint8_t);
void DS3231_Set_Hour(uint8_t);
void DS3231_Set_Day(uint8_t);
void DS3231_Set_Date(uint8_t);
void DS3231_Set_Month(uint8_t);
void DS3231_Set_Year(uint8_t);

void DS3231_Set_Data(DS3231_TypeDef*);
//====================================================================================================
#endif
//====================================================================================================