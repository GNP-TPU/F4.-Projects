//===============================================================================
#include <stm32f401xe.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
//===============================================================================
#include "main.h"
#include "UART.h"
#include "RC522.h"
#include "SPI.h"
//===============================================================================
#define RS485_TX_MODE() 			GPIOC->ODR |= GPIO_ODR_OD11
#define RS485_RX_MODE() 			GPIOC->ODR &= ~GPIO_ODR_OD11
//===============================================================================
/*Connection variables*/
uint8_t From_PC_CMD = FROM_PC_WAITING;
uint8_t From_PC_Sector = 0;
uint8_t From_PC_Block = 0;
uint8_t From_PC_Data[16];
uint8_t From_PC_CheckZero[16];
uint8_t To_PC_Data[16];
uint8_t Receive_Flag = 0;
//===============================================================================
/*UART*/
char USART1_Buffer_Tx[100], USART1_Buffer_Rx[100], USART1_Byte;
char USART2_Buffer_Tx[100], USART2_Buffer_Rx[100], USART2_Byte;
uint32_t USART1_Buffer_Pointer = 0;
uint32_t USART2_Buffer_Pointer = 0;
//===============================================================================
/*MFRC522*/

uint8_t UID_Read[4], Data_Read[17], status[4], Data_Write[17];

uint8_t KeyA[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t KeyB[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

uint8_t Card_0[16] = {0xD2, 0xFF, 0x50, 0x43, 0x3E, 0x08, 0x04, 0x00, 0x03, 0x03, 0x41, 0xE2, 0x0F, 0x53, 0x3A, 0xC9};
uint8_t Card_1[16] = {'H', 'e', 'l', 'l', 'o', 0xA8, 0xA4, 0xA0, 0xA3, 0xA3, 0xA1, 0xA2, 0xAF, 0xA3, 0xAA, 0xA9};
uint8_t Card_2[16] = {0x01, 0x02, 0x03, 0x00, 0x00, 0x00, 0x04, 0x00, 0x43, 0x44, 0xA1, 0xE2, 0x0F, 0x53, 0x3A, 0xC9};
uint8_t Card_Eraser[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

uint8_t Cards_UID_DataBase[] = {0x33, 0x51, 0xC6, 0x26,
																0x93, 0xA7, 0x18, 0x19,
																0xE3, 0xE2, 0xEB, 0x00};
//===============================================================================
int main(void){
	UART1_Init();
	UART2_Init();
	
	SPI1_Init();
	SPI2_Init();
	
	RC522_GPIO_Init();
	RC522_Init();
	
	memset(UID_Read, 0, sizeof(UID_Read));
	status[3] = 0;
	
	while(1){
		switch(From_PC_CMD){
			case FROM_PC_WRITE_UID:
				status[3] = 0;
				RC522_SetUID(UID_Read, To_PC_Data, PICC_AUTHENT1A, KeyB, 0, 0, From_PC_Data, status); 
				if(status[3] == 1){
					sprintf(USART1_Buffer_Tx, "%02X", TO_PC_WRITE_UID_COMPLETED);
					UART2_SendString(USART1_Buffer_Tx);
					sprintf(USART1_Buffer_Tx, "%02X", From_PC_CMD);
					UART2_SendString(USART1_Buffer_Tx);
					sprintf(USART1_Buffer_Tx, "%02X", From_PC_Sector);
					UART2_SendString(USART1_Buffer_Tx);
					sprintf(USART1_Buffer_Tx, "%02X", From_PC_Block);
					UART2_SendString(USART1_Buffer_Tx);
					for(uint8_t byte_in_block = 0; byte_in_block < 16; byte_in_block++){
						Data_Write[byte_in_block] = Data_Read[byte_in_block];
						sprintf(USART1_Buffer_Tx, "%02X", To_PC_Data[byte_in_block]);
						UART2_SendString(USART1_Buffer_Tx);
					}
					for(uint8_t byte_in_block = 0; byte_in_block < 16; byte_in_block++){
						Data_Write[byte_in_block] = Data_Read[byte_in_block];
						sprintf(USART1_Buffer_Tx, "%02X", From_PC_Data[byte_in_block]);
						UART2_SendString(USART1_Buffer_Tx);
					}
					UART2_SendString("\r\n");
				}
				break;
			case FROM_PC_READ_UID:
				status[3] = 0;
				RC522_Read_PICC_Data(UID_Read, To_PC_Data, PICC_AUTHENT1A, KeyB, 0, 0, status); 
				if(status[3] == 1){
					sprintf(USART1_Buffer_Tx, "%02X", TO_PC_READ_UID_COMPLETED);
					UART2_SendString(USART1_Buffer_Tx);
					sprintf(USART1_Buffer_Tx, "%02X", From_PC_CMD);
					UART2_SendString(USART1_Buffer_Tx);
					sprintf(USART1_Buffer_Tx, "%02X", From_PC_Sector);
					UART2_SendString(USART1_Buffer_Tx);
					sprintf(USART1_Buffer_Tx, "%02X", From_PC_Block);
					UART2_SendString(USART1_Buffer_Tx);
					for(uint8_t byte_in_block = 0; byte_in_block < 16; byte_in_block++){
						Data_Write[byte_in_block] = Data_Read[byte_in_block];
						sprintf(USART1_Buffer_Tx, "%02X", To_PC_Data[byte_in_block]);
						UART2_SendString(USART1_Buffer_Tx);
					}
					UART2_SendString("\r\n");
				}
				break;
			case FROM_PC_WRITE_BLOCK:	
				status[3] = 0;
				RC522_Write_PICC_Data(From_PC_Data, PICC_AUTHENT1A, KeyB, From_PC_Sector, From_PC_Block, status);
				if(status[3] == 1){
					sprintf(USART1_Buffer_Tx, "%02X", TO_PC_WRITE_BLOCK_COMPLETED);
					UART2_SendString(USART1_Buffer_Tx);
					sprintf(USART1_Buffer_Tx, "%02X", From_PC_CMD);
					UART2_SendString(USART1_Buffer_Tx);
					sprintf(USART1_Buffer_Tx, "%02X", From_PC_Sector);
					UART2_SendString(USART1_Buffer_Tx);
					sprintf(USART1_Buffer_Tx, "%02X", From_PC_Block);
					UART2_SendString(USART1_Buffer_Tx);
					for(uint8_t byte_in_block = 0; byte_in_block < 16; byte_in_block++){
						Data_Write[byte_in_block] = Data_Read[byte_in_block];
						sprintf(USART1_Buffer_Tx, "%02X", From_PC_Data[byte_in_block]);
						UART2_SendString(USART1_Buffer_Tx);
					}
					UART2_SendString("\r\n");
				}
				break;
			case FROM_PC_READ_BLOCK:
				status[3] = 0;
				RC522_Read_PICC_Data(UID_Read, To_PC_Data, PICC_AUTHENT1A, KeyB, From_PC_Sector, From_PC_Block, status); 
				if(status[3] == 1){
					sprintf(USART1_Buffer_Tx, "%02X", TO_PC_READ_BLOCK_COMPLETED);
					UART2_SendString(USART1_Buffer_Tx);
					sprintf(USART1_Buffer_Tx, "%02X", From_PC_CMD);
					UART2_SendString(USART2_Buffer_Tx);
					sprintf(USART1_Buffer_Tx, "%02X", From_PC_Sector);
					UART2_SendString(USART1_Buffer_Tx);
					sprintf(USART1_Buffer_Tx, "%02X", From_PC_Block);
					UART2_SendString(USART1_Buffer_Tx);
					for(uint8_t byte_in_block = 0; byte_in_block < 16; byte_in_block++){
						sprintf(USART2_Buffer_Tx, "%02X", To_PC_Data[byte_in_block]);
						UART2_SendString(USART2_Buffer_Tx);
						Data_Write[byte_in_block] = Data_Read[byte_in_block];
					}
					UART2_SendString("\r\n");
				}
				break;
			case FROM_PC_WAITING:
				if(!(USART2->SR & USART_SR_RXNE)){
					if(strstr(USART2_Buffer_Rx, "#END#") !=  NULL){				
						for(uint8_t i = 0; i < 2; i++){
							if(USART2_Buffer_Rx[i + 8] == 'Z')
								USART2_Buffer_Rx[i + 8] = 0;
						}
						
						From_PC_CMD = USART2_Buffer_Rx[7];
						From_PC_Sector = USART2_Buffer_Rx[8];
						From_PC_Block = USART2_Buffer_Rx[9];
						
						for(uint8_t i = 0; i < 16; i++){
							if(USART2_Buffer_Rx[i + 26] == 0x01)
								From_PC_Data[i] = 0x00;
							else
								From_PC_Data[i] = USART2_Buffer_Rx[i + 10];
						}
						
						sprintf(USART2_Buffer_Tx, "%02X", TO_PC_INFO);
						UART2_SendString(USART2_Buffer_Tx);
						sprintf(USART2_Buffer_Tx, "%02X", From_PC_CMD);
						UART2_SendString(USART2_Buffer_Tx);
						sprintf(USART2_Buffer_Tx, "%02X", From_PC_Sector);
						UART2_SendString(USART2_Buffer_Tx);
						sprintf(USART2_Buffer_Tx, "%02X", From_PC_Block);
						UART2_SendString(USART2_Buffer_Tx);
						UART2_SendString("\r\n");
						
						/*if(From_PC_CMD == FROM_PC_WRITE_UID){
							sprintf(USART2_Buffer_Tx, "%02X", TO_PC_INFO);
							UART2_SendString(USART2_Buffer_Tx);
							for(uint8_t i = 0; i < 16; i++){
								sprintf(USART2_Buffer_Tx, "%02X", From_PC_Data[i]);
								UART2_SendString(USART2_Buffer_Tx);
							}
							UART2_SendString("\r\n");
						}
						
						if(From_PC_CMD == FROM_PC_WRITE_BLOCK){
							sprintf(USART2_Buffer_Tx, "%02X", TO_PC_INFO);
							UART2_SendString(USART2_Buffer_Tx);
							for(uint8_t i = 0; i < 16; i++){
								sprintf(USART2_Buffer_Tx, "%02X", From_PC_Data[i]);
								UART2_SendString(USART2_Buffer_Tx);
							}
							UART2_SendString("\r\n");
						}*/
						
						USART2_Buffer_Pointer = 0;
						memset(USART2_Buffer_Rx, 0, sizeof(USART2_Buffer_Rx));
					}
				}
				break;
		}
	}
}
//===============================================================================
/*void USART1_IRQHandler(void){
	USART2_Byte = 0;
	while(USART1->SR & USART_SR_RXNE){
		USART2_Byte = USART1->DR;
	}
	USART2_Buffer_Rx[USART_Buffer_Pointer] = USART2_Byte;
	USART_Buffer_Pointer++;
}*/

void USART2_IRQHandler(void){
	while(USART2->SR & USART_SR_RXNE){
		USART2_Byte = USART2->DR;
	}
	
	USART2_Buffer_Rx[USART2_Buffer_Pointer] = USART2_Byte;
	USART2_Buffer_Pointer++;
	From_PC_CMD = FROM_PC_WAITING;
}
//===============================================================================
