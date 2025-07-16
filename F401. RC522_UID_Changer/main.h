//===============================================================================
#include <stm32f401xe.h>
#include <string.h>
#include <stdio.h>
//===============================================================================
#define FROM_PC_START													"#START#"
#define FROM_PC_END														"#END#"
//===============================================================================
#define FROM_PC_WAITING												0x00

#define FROM_PC_WRITE_UID											0x01
#define FROM_PC_READ_UID											0x02	

#define FROM_PC_WRITE_BLOCK 									0x03
#define FROM_PC_READ_BLOCK					 					0x04
//===============================================================================
#define TO_PC_INFO														0x30

#define TO_PC_WRITE_UID_COMPLETED							0x31
#define TO_PC_READ_UID_COMPLETED							0x32

#define TO_PC_WRITE_BLOCK_COMPLETED						0x33
#define TO_PC_READ_BLOCK_COMPLETED						0x34
//===============================================================================
#define LED_ON		0x99
#define LED_OFF		0x9A
//===============================================================================
