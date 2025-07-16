//====================================================================================================
#ifndef DMA_H
#define DMA_H
//====================================================================================================
#include <stm32f4xx.h>
#include <string.h>
#include <stdio.h>
//====================================================================================================
#define DMA_DIRECTION_PER2MEM		0x00
#define DMA_DIRECTION_MEM2PER		0x01
#define DMA_DIRECTION_MEM2MEM		0x02
//====================================================================================================
typedef struct{
	DMA_TypeDef* 				DMA;
	
	DMA_Stream_TypeDef*	DMA_Stream;
	
	uint8_t 						DMA_Channel;
	
	uint8_t							DMA_Direction;
	
	uint8_t 						DMA_DataSizeMemory;
	
	uint8_t							DMA_DataSizePeripheral;
	
	volatile uint32_t* 	DMA_SourceAddress;
	
	volatile uint32_t*	DMA_DestinationAddress;
	
	uint32_t						DMA_BufferSize;
}DMA_InitTypeDef;
//====================================================================================================
void DMA_Init(DMA_TypeDef*, DMA_InitTypeDef*);

void DMA_USART_Start(DMA_InitTypeDef*, USART_TypeDef*);

void DMA_USART_TransferCompletedWait(DMA_InitTypeDef*, USART_TypeDef*);
//====================================================================================================
#endif
//====================================================================================================