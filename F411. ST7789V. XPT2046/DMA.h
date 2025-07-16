//====================================================================================================
#ifndef DMA_H
#define DMA_H
//====================================================================================================
#include <stm32f4xx.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
//====================================================================================================
#define DMA_DIRECTION_PER2MEM		0x00
#define DMA_DIRECTION_MEM2PER		0x01
#define DMA_DIRECTION_MEM2MEM		0x02

#define DMA_SIZE_BYTE						0x00
#define DMA_SIZE_HALF_WORD			0x01
#define DMA_SIZE_WORD						0x02
//====================================================================================================
typedef struct{
	DMA_TypeDef* 				DMA;
	
	DMA_Stream_TypeDef*	DMA_Stream;
	
	uint8_t 						DMA_Channel;
	
	uint8_t							DMA_Direction;
	
	volatile uint32_t* 	DMA_SourceAddress;
	uint8_t 						DMA_Source_Size;
	bool								DMA_Source_Increment;
	
	volatile uint32_t*	DMA_DestinationAddress;
	uint8_t							DMA_Destination_Size;
	bool								DMA_Destination_Increment;	
	
	uint32_t						DMA_BufferSize;
}DMA_InitTypeDef;
//====================================================================================================
void DMA_Init(DMA_TypeDef*, DMA_InitTypeDef*);

void DMA_SPI_Start(DMA_InitTypeDef* DMAx_Struct, SPI_TypeDef* SPIx);

// void DMA_DCMI_Start(DMA_InitTypeDef*, DCMI_TypeDef*);

void DMA_USART_Start(DMA_InitTypeDef*, USART_TypeDef*);

void DMA_USART_TransferCompletedWait(DMA_InitTypeDef*, USART_TypeDef*);
//====================================================================================================
#endif
//====================================================================================================