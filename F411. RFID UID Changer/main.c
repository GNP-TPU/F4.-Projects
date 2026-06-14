//====================================================================================================
#include "main.h"
//====================================================================================================
void System_Configure(void);

void RCC_Configure(void);
void GPIO_Configure(void);
void DMA_Configure(void);
void SPI_Configure(void);
//====================================================================================================
// Struct initialization
SPI_InitTypeDef		SPI_RC522, SPI_ST7789;

RC522_TypeDef			RC522_Init_Struct;
RC522_Buffer			RC522_Data;
//====================================================================================================
char Display_String_Buffer[100];

uint8_t PICC_ReadDataBuffer[16];

uint8_t PICC_GotDataStatus = NoCardDetected;
bool PICC_SetDataStatus = false;

uint8_t Card_0[16] = {0xD2, 0xFF, 0x50, 0x43, 0x3E, 0x08, 0x04, 0x00, 0x03, 0x03, 0x41, 0xE2, 0x0F, 0x53, 0x3A, 0xC9};
//====================================================================================================
int main(void){
	System_Configure();
	
	while(1){
		switch(PICC_GotDataStatus){
			case NoCardDetected:{
				RC522_Read_PICC_Data(&RC522_Data);
				if((RC522_Data.PICC_ReadData[5] != 0) || (RC522_Data.PICC_ReadData[6] != 0)){
					for(uint8_t count = 0; count < 16; count++){
						PICC_ReadDataBuffer[count] = RC522_Data.PICC_ReadData[count];
					}
					PICC_GotDataStatus = CardDetected;
				}
				break;
			}
			case CardDetected:{
				sprintf(Display_String_Buffer, "Card detected.Place and hold another   tag to copy UID    until verification.");
				ST77xx_WriteString(Display_String_Buffer, 0, 80, WHITE, BLACK);
				delay_ms(500);
				PICC_GotDataStatus = AwaitingCardToWrite;
				break;
			}
			case AwaitingCardToWrite:{
				PICC_SetDataStatus = RC522_SetUID(&RC522_Data, PICC_ReadDataBuffer);
				if(PICC_SetDataStatus == MI_OK){
					
					
					delay_ms(1000);
					
					RC522_Read_PICC_Data(&RC522_Data);
					
					sprintf(Display_String_Buffer, "                                                                                   ");
					ST77xx_WriteString(Display_String_Buffer, 0, 80, WHITE, BLACK);
					
					for(uint8_t counter = 0; counter < 16; counter++){
						if(RC522_Data.PICC_ReadData[counter] != PICC_ReadDataBuffer[counter]){
							PICC_SetDataStatus = false;
							break;
						}
						else{
							PICC_SetDataStatus = true;
						}
					}
					
					if(PICC_SetDataStatus == true){
						sprintf(Display_String_Buffer, "Card data set      successfully!");
						ST77xx_WriteString(Display_String_Buffer, 0, 80, WHITE, BLACK);
						delay_ms(3000);
						PICC_SetDataStatus = MI_NOTAGERR;
						PICC_GotDataStatus = CardWritten;
					}
					else{
						sprintf(Display_String_Buffer, "Copying error. Try again");
						ST77xx_WriteString(Display_String_Buffer, 0, 80, WHITE, BLACK);
						delay_ms(2000);
						sprintf(Display_String_Buffer, "Place the initial  RFID card near the reader");
						ST77xx_WriteString(Display_String_Buffer, 0, 80, WHITE, BLACK);
						PICC_SetDataStatus = MI_NOTAGERR;
						PICC_GotDataStatus = NoCardDetected;
					}
					
					memset(RC522_Data.PICC_ReadData, 0, sizeof(RC522_Data.PICC_ReadData));
				}
				break;
			}
			case CardWritten:{
				sprintf(Display_String_Buffer, "                                                                                       ");
				ST77xx_WriteString(Display_String_Buffer, 0, 80, WHITE, BLACK);
				sprintf(Display_String_Buffer, "Place the initial  RFID card near the reader");
				ST77xx_WriteString(Display_String_Buffer, 0, 80, WHITE, BLACK);
				delay_ms(1000);
				PICC_GotDataStatus = NoCardDetected;
				break;
			}
		}
	}
}
//====================================================================================================
void System_Configure(void){
	RCC_Configure();
	GPIO_Configure();
	SPI_Configure();
	// DMA_Configure();
	
	ST77xx_Init();
	
	RC522_Init_Struct.RST_Port = GPIOA;
	RC522_Init_Struct.RST_Pin = GPIO_PIN_8;
	
	RC522_Init_Struct.CS_Port = GPIOA;
	RC522_Init_Struct.CS_Pin = GPIO_PIN_9;
	
	RC522_Init(&RC522_Init_Struct);
	
	memset(RC522_Data.UID, 0, sizeof(RC522_Data.UID));
		
	RC522_Data.PICC_AuthMode = PICC_AUTHENT1A;
	RC522_Data.Status = 0x00;
	
	sprintf(Display_String_Buffer, "RC522 version: %X", RC522_Init_Struct.Version);
	ST77xx_WriteString(Display_String_Buffer, 0, 40, WHITE, BLACK);
	
	sprintf(Display_String_Buffer, "Place the initial  RFID card near the reader");
	ST77xx_WriteString(Display_String_Buffer, 0, 80, WHITE, BLACK);
	
	/*
	ST77xx_SetWindow(0, 0, 319, 239);
	
	SPI_ST7789.Baudrate_Prescaler = SPI_BAUDRATE_DIV2;
	SPI_ST7789.Half_Word_Mode = true;
	
	SPI_Init(SPI1, &SPI_ST7789);
	
	ST77xx_DC_HIGH();
	ST77xx_CS_LOW();
	
	DMA_SPI_Start(&SPI1_DMA, SPI1);
	*/
}

void RCC_Configure(void){
	RCC_InitTypeDef 	RCC_InitStruct;
	
	RCC_InitStruct.ManualCalculatePLL				= false;
	
	RCC_InitStruct.OscillatorType						= OSC_EXTERNAL;	
	RCC_InitStruct.SystemClockSource				= PLL_CLK_SRC;
	
	RCC_InitStruct.ExternalOscillatorFreq 	= 25000000;
	
	RCC_InitStruct.AHB_Prescaler						= AHB_PRESCALER_DIV1;
	RCC_InitStruct.APB1_Prescaler						= APB_PRESCALER_DIV2;
	RCC_InitStruct.APB2_Prescaler						= APB_PRESCALER_DIV1;
		
	RCC_InitStruct.PLL_M_Divider						= 25;
	RCC_InitStruct.PLL_N_Multiplier					= 144;
	RCC_InitStruct.PLL_P_Divider						=	PLL_P_DIV2;
	
	RCC_InitStruct.MCO1_Enable							= false;
	RCC_InitStruct.MCO1_Source							= PLL_MCO_SRC;
	RCC_InitStruct.MCO1_Prescaler						= MCO_DIV5;
	
	SystemCoreClockConfigure(&RCC_InitStruct);
}

void GPIO_Configure(void){
	GPIO_InitTypeDef GPIO_InitStruct;	
	
	// SPI1, Display
	GPIO_InitStruct.Pin 			= GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;									
	GPIO_InitStruct.Mode 			= MODE_AF;
	GPIO_InitStruct.Type 			= TYPE_PP;
	GPIO_InitStruct.Speed 		= GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate	= AF_SPI1;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin 			= GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12;									
	GPIO_InitStruct.Mode 			= MODE_OUTPUT;
	GPIO_InitStruct.Type 			= TYPE_PP;
	GPIO_InitStruct.Speed 		= GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	// SPI2, RC522
	GPIO_InitStruct.Pin 			= GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;									
	GPIO_InitStruct.Mode 			= MODE_AF;
	GPIO_InitStruct.Type 			= TYPE_PP;
	GPIO_InitStruct.Speed 		= GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate	= AF_SPI2;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin 			= GPIO_PIN_8 | GPIO_PIN_9;									
	GPIO_InitStruct.Mode 			= MODE_OUTPUT;
	GPIO_InitStruct.Pull 			= PULL_FLOATING;
	GPIO_InitStruct.Type 			= TYPE_PP;
	GPIO_InitStruct.Speed 		= GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	GPIO_Pin_Low(GPIOA, GPIO_PIN_8);
	delay_ms(100);
	GPIO_Pin_High(GPIOA, GPIO_PIN_8);
}
/*
void DMA_Configure(void){
	SPI1_DMA.DMA										= DMA2;
	SPI1_DMA.DMA_Stream							= DMA2_Stream2;
	SPI1_DMA.DMA_Channel						= 2;
	SPI1_DMA.DMA_Direction					= DMA_DIRECTION_MEM2PER;
		
	SPI1_DMA.DMA_SourceAddress			= (uint32_t*)Color;
	SPI1_DMA.DMA_Source_Increment		= true;
	SPI1_DMA.DMA_Source_Size				= DMA_SIZE_HALF_WORD;
	
	SPI1_DMA.DMA_DestinationAddress			= (uint32_t*)&SPI1->DR;
	SPI1_DMA.DMA_Destination_Increment	= false;
	SPI1_DMA.DMA_Destination_Size				= DMA_SIZE_HALF_WORD;
	
	SPI1_DMA.DMA_BufferSize					=	320*80;
	
	DMA_Init(SPI1_DMA.DMA, &SPI1_DMA);
}
*/

void SPI_Configure(void){
	SPI_ST7789.Baudrate_Prescaler = SPI_BAUDRATE_DIV2;	
	SPI_ST7789.Half_Word_Mode = false;
	SPI_Init(SPI1, &SPI_ST7789);
	
	SPI_RC522.Baudrate_Prescaler = SPI_BAUDRATE_DIV8;
	SPI_Init(SPI2, &SPI_RC522);
}
//====================================================================================================

