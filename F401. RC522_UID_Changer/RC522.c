//===============================================================================
#include <string.h>
//===============================================================================
#include "SPI.h"
#include "RC522.h"
#include "UART.h"
//===============================================================================
void RC522_GPIO_Init(void){
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	
	GPIOA->MODER |= GPIO_MODER_MODE11_0 | GPIO_MODER_MODE12_0;
	GPIOA->OTYPER &= ~(GPIO_OTYPER_OT11 | GPIO_OTYPER_OT12);
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR11 | GPIO_OSPEEDER_OSPEEDR12;
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD11 | GPIO_PUPDR_PUPD12);
}

uint8_t RC522_ReadRegister(uint8_t reg){
	RC522_CS_LOW();
	reg = ((reg << 1) & 0x7E) | 0x80;
	SPI_Transmit(&reg, 1);
	uint8_t dataRx = 0;
	SPI_Receive(&dataRx, 1);
	RC522_CS_HIGH();
	return dataRx;
}

void RC522_WriteRegister(uint8_t reg, uint8_t data){
	RC522_CS_LOW();
	uint8_t dataTx[2] = {0x7E & (reg << 1), data};
	SPI_Transmit(dataTx, 2);
	RC522_CS_HIGH();
}

void RC522_SetBit(uint8_t reg, uint8_t mask){
	RC522_WriteRegister(reg, RC522_ReadRegister(reg) | mask);
}

void RC522_ClearBit(uint8_t reg, uint8_t mask){
	RC522_WriteRegister(reg, RC522_ReadRegister(reg) & (~mask));
}

void RC522_Init(void){
	RC522_RST_LOW();
	for(volatile long int i = 0; i < 100000; i++);
	RC522_RST_HIGH();
	for(volatile long int i = 0; i < 100000; i++);
	
	RC522_Reset();
	
	RC522_WriteRegister(MFRC522_REG_T_MODE, 0x80);
	RC522_WriteRegister(MFRC522_REG_T_PRESCALER, 0xA9);
	RC522_WriteRegister(MFRC522_REG_T_RELOAD_L, 0xE8);
	RC522_WriteRegister(MFRC522_REG_T_RELOAD_H, 0x03);
	
	RC522_WriteRegister(MFRC522_REG_TX_AUTO, 0x40);
	RC522_WriteRegister(MFRC522_REG_MODE, 0x3D);
	
	RC522_AntennaON();
	
	uint8_t sta = 0;
	char dataSend[100];
	
	UART2_SendString("\r\n");
	
	sta = RC522_ReadRegister(0x37);
	sprintf(dataSend, "RC522 version: %02X\r\n", sta);
	UART2_SendString(dataSend);
	
	UART2_SendString("\r\n");
}

void RC522_Reset(void){
	RC522_WriteRegister(0x01, 0x0F);
}

void RC522_AntennaON(void){
	uint8_t temp;
	
	temp = RC522_ReadRegister(MFRC522_REG_TX_CONTROL);
	if(!(temp & 0x03))
		RC522_SetBit(MFRC522_REG_TX_CONTROL, 0x03);
}
//===============================================================================
uint8_t RC522_CheckCard(uint8_t* id){
	uint8_t status = MI_OK;
	status = RC522_Request(PICC_REQIDL, id);
	if(status == MI_OK){
		status = RC522_AntiColl(id);
	}
	RC522_Halt();
	
	return status;
}

uint8_t RC522_Request(uint8_t reqMode, uint8_t* tagType){
	uint8_t status;
	uint16_t backBits;
	RC522_WriteRegister(MFRC522_REG_BIT_FRAMING, 0x07);
	tagType[0] = reqMode;
	status = RC522_ToCard(PCD_TRANSCEIVE, tagType, 1, tagType, &backBits);
	if((status != MI_OK) || (backBits != 0x10)){
		status = MI_ERR;
	}
	return status;
}

uint8_t RC522_ToCard(uint8_t command, uint8_t* sendData, uint8_t sendLen, uint8_t* backData, uint16_t* backLen){
	uint8_t status = MI_ERR;
	uint8_t irqEn = 0x00;
	uint8_t waitIRq = 0x00;
	uint8_t lastBits;
	uint8_t check;
	uint16_t i;
	
	switch(command){
		case PCD_AUTHENT:{
			irqEn = 0x12;
			waitIRq = 0x10;
			break;
		}
		case PCD_TRANSCEIVE:{
			irqEn = 0x77;
			waitIRq = 0x30;
			break;
		}
		default:
			break;
	}
	
	RC522_WriteRegister(MFRC522_REG_COMM_IE_N, irqEn | 0x80);
	RC522_ClearBit(MFRC522_REG_COMM_IRQ, 0x80);
	RC522_SetBit(MFRC522_REG_FIFO_LEVEL, 0x80);
	
	RC522_WriteRegister(MFRC522_REG_COMMAND, PCD_IDLE);
	
	for(i = 0; i < sendLen; i++){
		RC522_WriteRegister(MFRC522_REG_FIFO_DATA, sendData[i]);
	}
	
	RC522_WriteRegister(MFRC522_REG_COMMAND, command);
	
	if(command == PCD_TRANSCEIVE){
		RC522_SetBit(MFRC522_REG_BIT_FRAMING, 0x80);
	}
	
	i = 30000;
	do{
		check = RC522_ReadRegister(MFRC522_REG_COMM_IRQ);
		i--;
	}while((i != 0) && !(check & 0x01) && !(check & waitIRq));
	
	RC522_ClearBit(MFRC522_REG_BIT_FRAMING, 0x80);
	
	if(i != 0){
		if(!(RC522_ReadRegister(MFRC522_REG_ERROR) & 0x1B)){
			status = MI_OK;
			if(check & irqEn & 0x01){
				status = MI_NOTAGERR;
			}
			
			if(command == PCD_TRANSCEIVE){
				check = RC522_ReadRegister(MFRC522_REG_FIFO_LEVEL);
				uint8_t prev_check = check;
				lastBits = RC522_ReadRegister(MFRC522_REG_CONTROL) & 0x07;
				if(lastBits){
					*backLen = (check - 1) * 8 + lastBits;
				}
				else{
					*backLen = check * 8;
				}
				
				if(check == 0){
					check = 1;
				}
				if(check > MFRC522_MAX_LEN){
					check = MFRC522_MAX_LEN;
				}
				
				for(i = 0; i < check; i++){
					uint8_t d = RC522_ReadRegister(MFRC522_REG_FIFO_DATA);
					backData[i] = d;
				}
				return status;
			}
		}		
		else{
			status = MI_ERR;
		}
	}
	return status;
}

uint8_t RC522_AntiColl(uint8_t* serNum){
	uint8_t status;
	uint8_t i;
	uint8_t serNumCheck = 0;
	uint16_t unLen;
	
	RC522_WriteRegister(MFRC522_REG_BIT_FRAMING, 0x00);
	serNum[0] = PICC_ANTICOLL;
	serNum[1] = 0x20;
	status = RC522_ToCard(PCD_TRANSCEIVE, serNum, 2, serNum, &unLen);
	
	if(status == MI_OK){
		for(i = 0; i < 4; i++){
			serNumCheck ^= serNum[i];
		}
		if(serNumCheck != serNum[i]){
			status = MI_ERR;
		}
	}
	return status;
}

void RC522_Halt(void){
	uint16_t unLen;
	uint8_t buff[4];
	
	buff[0] = PICC_HALT;
	buff[1] = 0;
	RC522_CalculateCRC(buff, 2, &buff[2]);
	
	RC522_ToCard(PCD_TRANSCEIVE, buff, 4, buff, &unLen);
}

void RC522_CalculateCRC(uint8_t* pIndata, uint8_t len, uint8_t* pOutData){
	uint8_t i, n;
	RC522_ClearBit(MFRC522_REG_DIV_IRQ, 0x04);
	RC522_SetBit(MFRC522_REG_FIFO_LEVEL, 0x80);
	
	for(i = 0; i < len; i++){
		RC522_WriteRegister(MFRC522_REG_FIFO_DATA, *(pIndata + i));
	}
	RC522_WriteRegister(MFRC522_REG_COMMAND, PCD_CALCCRC);
	
	i = 0xFF;
	do{
		n = RC522_ReadRegister(MFRC522_REG_DIV_IRQ);
		i--;
	}while((i != 0) && !(n & 0x04));
	
	pOutData[0] = RC522_ReadRegister(MFRC522_REG_CRC_RESULT_L);
	pOutData[1] = RC522_ReadRegister(MFRC522_REG_CRC_RESULT_M);
}

uint8_t RC522_WriteBlock(uint8_t blockAddr, uint8_t* writeData){
	uint8_t status;
	uint16_t recvBits;
	uint8_t i;
	uint8_t buff[18];
	buff[0] = PICC_WRITE;
	buff[1] = blockAddr;
	RC522_CalculateCRC(buff, 2, &buff[2]);
	status = RC522_ToCard(PCD_TRANSCEIVE, buff, 4, buff, &recvBits);
	if(status != MI_OK){
		status = MI_ERR;
	}
	if(status == MI_OK){
		for(i = 0; i < 16; i++){
			buff[i] = *(writeData + i);
		}
		RC522_CalculateCRC(buff, 16, &buff[16]);
		status = RC522_ToCard(PCD_TRANSCEIVE, buff, 18, buff, &recvBits);
		if(status != MI_OK){
			status = MI_ERR;
		}
	}
	return status;
}

uint8_t RC522_ReadBlock(uint8_t blockAddr, uint8_t* recvData){
	uint8_t status;
	uint16_t unLen;
	recvData[0] = PICC_READ;
	recvData[1] = blockAddr;
	RC522_CalculateCRC(recvData, 2, &recvData[2]);
	status = RC522_ToCard(PCD_TRANSCEIVE, recvData, 4, recvData, &unLen);
	if((status != MI_OK) || (unLen != 0x90)){
		status = MI_ERR;
	}
	return status;
}

uint8_t RC522_SelectTag(uint8_t* serNum){
	uint8_t i;
	uint8_t status;
	uint8_t size;
	uint16_t recvBits;
	uint8_t buffer[9];
	buffer[0] = PICC_SElECTTAG;
	buffer[1] = 0x70;
	for(i = 0; i < 5; i++){
		buffer[i + 2] = *(serNum + i);
	}
	RC522_CalculateCRC(buffer, 7, &buffer[7]);
	status = RC522_ToCard(PCD_TRANSCEIVE, buffer, 9, buffer, &recvBits);
	if((status == MI_OK) && (recvBits == 0x18)){
		size = buffer[0];
	}
	else{
		size = 0;
	}
	return size;
}

uint8_t RC522_Auth(uint8_t authMode, uint8_t BlockAddr, uint8_t* SectorKey, uint8_t* serNum){
	uint8_t status;
	uint16_t recvBits;
	uint8_t i;
	uint8_t buff[12];
	
	buff[0] = authMode;
	buff[1] = BlockAddr;
	
	for(i = 0; i < 6; i++){
		buff[i + 2] = *(SectorKey + i);
	}
	for(i = 0; i < 4; i++){
		buff[i + 8] = *(serNum + i);
	}
	status = RC522_ToCard(PCD_AUTHENT, buff, 12, buff, &recvBits);
	if((status != MI_OK) || (!(RC522_ReadRegister(Status2Reg) & 0x08))){
		status = MI_ERR;
	}
	return status;
}

void RC522_StopCrypto1(void){
	RC522_ClearBit(Status2Reg, 0x08);
}
//===============================================================================
void RC522_Read_PICC_Data(uint8_t* UID_Read, uint8_t* Data_Read, uint8_t PICC_Auth_Mode, uint8_t* Key, uint8_t Sector_Num, uint8_t Block, uint8_t* access){
	uint8_t Block_Num = Block + (Sector_Num * 4);
	uint8_t Key_Block;
	uint8_t cardstr[17], str[17];
	uint8_t i = 0, j = 0;
	uint8_t sta = 0;
	char dataSend[100];
	memset(access, 0, sizeof(access));
	memset(cardstr, 0, sizeof(cardstr));
	sta = RC522_Request(PICC_REQIDL, cardstr);
	if(sta == MI_OK){
		access[0] = 1;
		sta = RC522_AntiColl(cardstr);
		if(sta == MI_OK){
			access[1] = 1;
			for(uint8_t count = 0; count < 4; count++){
				UID_Read[count] = cardstr[count];
			}
			sta = RC522_SelectTag(cardstr);
			if(sta > 0){
				access[2] = 1;
				Key_Block = (Sector_Num * 4) + 3;
				sta = RC522_Auth(PICC_Auth_Mode, Key_Block, Key, cardstr);
				sta = RC522_ReadBlock(Block_Num, str);
				for(uint8_t count = 0; count < 16; count++){
					Data_Read[count] = str[count];
				}
				access[3] = 1;
				RC522_Halt();
				RC522_StopCrypto1();
			}
		}
	}
}

void RC522_Write_PICC_Data(uint8_t* Data_Write, uint8_t PICC_Auth_Mode, uint8_t* Key, uint8_t Sector_Num, uint8_t Block, uint8_t* access){
	uint8_t Block_Num = Block + (Sector_Num * 4);
	uint8_t Key_Block;
	uint8_t cardstr[17], str[17];
	uint8_t i = 0, j = 0;
	uint8_t sta = 0;
	char dataSend[100];
	memset(cardstr, 0, sizeof(cardstr));
	sta = RC522_Request(PICC_REQIDL, cardstr);
	if(sta == MI_OK){
		sta = RC522_AntiColl(cardstr);
		if(sta == MI_OK){
			sta = RC522_SelectTag(cardstr);
			if(sta > 0){
				Key_Block = (Sector_Num * 4) + 3;
				sta = RC522_Auth(PICC_Auth_Mode, Key_Block, Key, cardstr);
				if(sta == MI_OK){
					sta = RC522_WriteBlock(Block_Num, Data_Write);
				}
				access[3] = 1;
				RC522_Halt();
				RC522_StopCrypto1();
			}
		}
	}
}

void RC522_SetUID(uint8_t* UID_Read, uint8_t* Data_Read, uint8_t PICC_Auth_Mode, uint8_t* Key, uint8_t Sector_Num, uint8_t Block, uint8_t* Data_Write, uint8_t* access){
	uint8_t Block_Num = Block + (Sector_Num * 4);
	uint8_t Key_Block;
	uint8_t cardstr[17], str[17];
	uint8_t i = 0, j = 0;
	uint8_t sta = 0;
	char dataSend[100];
	memset(cardstr, 0, sizeof(cardstr));
	sta = RC522_Request(PICC_REQIDL, cardstr);
	if(sta == MI_OK){
		sta = RC522_AntiColl(cardstr);
		if(sta == MI_OK){
			for(uint8_t count = 0; count < 4; count++){
				UID_Read[count] = cardstr[count];
			}

			sta = RC522_SelectTag(cardstr);
			//if(sta > 0){
				Key_Block = (Sector_Num * 4) + 3;
				sta = RC522_Auth(PICC_Auth_Mode, Key_Block, Key, cardstr);

				sta = RC522_ReadBlock(Block_Num, str);

				for(uint8_t count = 0; count < 16; count++){
					Data_Read[count] = str[count];

				}
			//}
		}
		sta = RC522_Auth(PICC_Auth_Mode, Key_Block, Key, cardstr);
		sta = RC522_ReadBlock(Block_Num, str);
		for(uint8_t count = 0; count < 16; count++){
			Data_Read[count] = str[count];
		}
		RC522_StopCrypto1();
				
		uint16_t unLen;
		uint8_t buff[4];
		
		memset(buff, 0, sizeof(buff));
		RC522_Halt();
		
		RC522_WriteRegister(MFRC522_REG_BIT_FRAMING, 0x07);
		buff[0] = 0x40;
		RC522_ToCard(PCD_TRANSCEIVE, &buff[0], 1, &buff[3], &unLen);

		RC522_WriteRegister(MFRC522_REG_BIT_FRAMING, 0x08);	
		buff[0] = 0x43;
		RC522_ToCard(PCD_TRANSCEIVE, &buff[0], 1, &buff[3], &unLen);

		if((buff[3] == 0x0A)){
			sta = RC522_WriteBlock(Block_Num, Data_Write);
			if(sta == MI_OK){
				
			}
			else{
				
			}
		}
		access[3] = 1;
		RC522_Halt();
		RC522_StopCrypto1();
	}
}
//===============================================================================
