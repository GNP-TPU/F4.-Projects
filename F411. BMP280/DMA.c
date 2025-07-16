#include "DMA.h"

void DMA_Init(DMA_TypeDef* DMAx, DMA_InitTypeDef* DMAx_Struct){
	if(DMAx == DMA1)
		RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN; // Включаем тактирование DMA1
	else if(DMAx == DMA2)
		RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN; // Включаем тактирование DMA2
	
  // Настройка DMA для передачи (TX)
  DMAx_Struct->DMA_Stream->CR 		&= ~DMA_SxCR_EN; // Отключаем поток
	
	DMAx_Struct->DMA_Stream->CR 		&= 0x00000000;
	
  DMAx_Struct->DMA_Stream->CR 		|= (DMAx_Struct->DMA_Channel << DMA_SxCR_CHSEL_Pos); // Выбор канала
	
  if(DMAx == DMA1)
		DMAx_Struct->DMA_Stream->CR 	|= DMA_SxCR_PL_0; // Устанавливаем приоритет
	else if(DMAx == DMA2)
		DMAx_Struct->DMA_Stream->CR 	|= DMA_SxCR_PL_1; // Устанавливаем приоритет
	
  DMAx_Struct->DMA_Stream->CR 		|= (DMAx_Struct->DMA_Direction << DMA_SxCR_DIR_Pos); // Направление
	
  DMAx_Struct->DMA_Stream->CR 		&= ~DMA_SxCR_MSIZE; // Размер данных (8 бит)
	DMAx_Struct->DMA_Stream->CR 		|= 	DMA_SxCR_MINC; // Инкремент memory
  DMAx_Struct->DMA_Stream->CR 		&= ~DMA_SxCR_PSIZE; // Размер данных (8 бит)

  DMAx_Struct->DMA_Stream->PAR 		= (uint32_t)DMAx_Struct->DMA_DestinationAddress; // Адрес регистра данных
  DMAx_Struct->DMA_Stream->M0AR		= (uint32_t)DMAx_Struct->DMA_SourceAddress; // Адрес буфера передачи
  DMAx_Struct->DMA_Stream->NDTR 	= DMAx_Struct->DMA_BufferSize; // Количество данных для передачи
}

void DMA_USART_Start(DMA_InitTypeDef* DMAx_Struct, USART_TypeDef* USARTx){
	DMAx_Struct->DMA_Stream->PAR 		= (uint32_t)DMAx_Struct->DMA_DestinationAddress; // Адрес регистра данных
  DMAx_Struct->DMA_Stream->M0AR		= (uint32_t)DMAx_Struct->DMA_SourceAddress; // Адрес буфера передачи
	DMAx_Struct->DMA_Stream->NDTR = DMAx_Struct->DMA_BufferSize; // Количество данных для передачи
	if(USARTx == USART1){
		DMAx_Struct->DMA->HIFCR |= DMA_HIFCR_CTCIF7 | DMA_HIFCR_CHTIF7 | DMA_HIFCR_CTEIF7 | DMA_HIFCR_CFEIF7;
	}
	else if(USARTx == USART2)
		DMAx_Struct->DMA->HIFCR |= DMA_HIFCR_CTCIF6 | DMA_HIFCR_CHTIF6 | DMA_HIFCR_CTEIF6 | DMA_HIFCR_CFEIF6;
	
	DMAx_Struct->DMA_Stream->CR |= DMA_SxCR_EN; // Включаем поток
	USARTx->CR3 |= USART_CR3_DMAT; // Включаем DMA для передачи
	
	// DMA_USART_Start(&USART2_DMA, PC_Connection.USART); // Пример
}

void DMA_USART_TransferCompletedWait(DMA_InitTypeDef* DMAx_Struct, USART_TypeDef* USARTx){
	if(USARTx == USART1){
		while(!(DMAx_Struct->DMA->HISR & DMA_HISR_TCIF7));
	}
	else if(USARTx == USART2)
		while(!(DMAx_Struct->DMA->HISR & DMA_HISR_TCIF6));
	
}

