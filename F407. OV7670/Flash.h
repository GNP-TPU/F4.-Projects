//====================================================================================================
#ifndef FLASH_H
#define FLASH_H
//====================================================================================================
#include <stm32f4xx.h>
#include <string.h>
#include <stdio.h>
//====================================================================================================
#define FLASH_KEY1 							(uint32_t)0x45670123
#define FLASH_KEY2 							(uint32_t)0xCDEF89AB

#define FLASH_SECTOR0_ADDRESS 	(uint32_t)0x08000000
#define FLASH_SECTOR1_ADDRESS 	(uint32_t)0x08004000
#define FLASH_SECTOR2_ADDRESS 	(uint32_t)0x08008000
#define FLASH_SECTOR3_ADDRESS 	(uint32_t)0x0800C000
#define FLASH_SECTOR4_ADDRESS 	(uint32_t)0x08010000
#define FLASH_SECTOR5_ADDRESS 	(uint32_t)0x08020000
#define FLASH_SECTOR6_ADDRESS 	(uint32_t)0x08040000
#define FLASH_SECTOR7_ADDRESS 	(uint32_t)0x08060000
//====================================================================================================
void Flash_Unlock(void);
void Flash_Lock(void);
void Flash_Unlock_Options(void);
void Flash_Modify_Options(uint32_t option_value);
void Flash_Set_OptLock(void);
void Flash_Unlock_Write_Protection(void);

void Flash_Erase_Sector(uint32_t sector);

void Flash_Write(uint32_t address, uint32_t data);
uint32_t Flash_Read(uint32_t address);
//====================================================================================================

//====================================================================================================
#endif
//====================================================================================================
