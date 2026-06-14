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
	
	switch(DMAx_Struct->DMA_Direction ){
		case DMA_DIRECTION_PER2MEM:
			DMAx_Struct->DMA_Stream->PAR 		= (uint32_t)DMAx_Struct->DMA_SourceAddress; 		 // Адрес регистра данных
			DMAx_Struct->DMA_Stream->M0AR		= (uint32_t)DMAx_Struct->DMA_DestinationAddress; // Адрес буфера передачи
			DMAx_Struct->DMA_Stream->NDTR 	= DMAx_Struct->DMA_BufferSize; 									 // Количество данных для передачи
			
			DMAx_Struct->DMA_Stream->CR 		&= ~DMA_SxCR_PSIZE; // Размер данных (8 бит)
			DMAx_Struct->DMA_Stream->CR 		&= ~DMA_SxCR_MSIZE; // Размер данных (8 бит)
		
			switch(DMAx_Struct->DMA_Source_Size){
				case DMA_SIZE_BYTE:
					__NOP();
					break;
				case DMA_SIZE_HALF_WORD:
					DMAx_Struct->DMA_Stream->CR 		|= DMA_SxCR_PSIZE_0; 
					break;
				case DMA_SIZE_WORD:
					DMAx_Struct->DMA_Stream->CR 		|= DMA_SxCR_PSIZE_1; 
					break;
				default:
					__NOP();
					break;
			}
			switch(DMAx_Struct->DMA_Destination_Size){
				case DMA_SIZE_BYTE:
					__NOP();
					break;
				case DMA_SIZE_HALF_WORD:
					DMAx_Struct->DMA_Stream->CR 		|= DMA_SxCR_MSIZE_0; 
					break;
				case DMA_SIZE_WORD:
					DMAx_Struct->DMA_Stream->CR 		|= DMA_SxCR_MSIZE_1; 
					break;
				default:
					__NOP();
					break;
			}
			
			if(DMAx_Struct->DMA_Source_Increment)
				DMAx_Struct->DMA_Stream->CR |= 	DMA_SxCR_PINC;
			else
				DMAx_Struct->DMA_Stream->CR &= 	~DMA_SxCR_PINC;
			
			if(DMAx_Struct->DMA_Destination_Increment)
				DMAx_Struct->DMA_Stream->CR |= 	DMA_SxCR_MINC;
			else
				DMAx_Struct->DMA_Stream->CR &= 	~DMA_SxCR_MINC;
				
			break;
			
		case DMA_DIRECTION_MEM2PER:
			DMAx_Struct->DMA_Stream->PAR 		= (uint32_t)DMAx_Struct->DMA_DestinationAddress; 		 // Адрес регистра данных
			DMAx_Struct->DMA_Stream->M0AR		= (uint32_t)DMAx_Struct->DMA_SourceAddress; // Адрес буфера передачи
		
			DMAx_Struct->DMA_Stream->CR 		&= ~DMA_SxCR_PSIZE; // Размер данных (8 бит)
			DMAx_Struct->DMA_Stream->CR 		&= ~DMA_SxCR_MSIZE; // Размер данных (8 бит)
		
			switch(DMAx_Struct->DMA_Source_Size){
				case DMA_SIZE_BYTE:
					__NOP();
					break;
				case DMA_SIZE_HALF_WORD:
					DMAx_Struct->DMA_Stream->CR 		|= DMA_SxCR_MSIZE_0; 
					break;
				case DMA_SIZE_WORD:
					DMAx_Struct->DMA_Stream->CR 		|= DMA_SxCR_MSIZE_1; 
					break;
				default:
					__NOP();
					break;
			}
			switch(DMAx_Struct->DMA_Destination_Size){
				case DMA_SIZE_BYTE:
					__NOP();
					break;
				case DMA_SIZE_HALF_WORD:
					DMAx_Struct->DMA_Stream->CR 		|= DMA_SxCR_PSIZE_0; 
					break;
				case DMA_SIZE_WORD:
					DMAx_Struct->DMA_Stream->CR 		|= DMA_SxCR_PSIZE_1; 
					break;
				default:
					__NOP();
					break;
			}
			
			if(DMAx_Struct->DMA_Source_Increment)
				DMAx_Struct->DMA_Stream->CR |= 	DMA_SxCR_MINC;
			else
				DMAx_Struct->DMA_Stream->CR &= 	~DMA_SxCR_MINC;
			
			if(DMAx_Struct->DMA_Destination_Increment)
				DMAx_Struct->DMA_Stream->CR |= 	DMA_SxCR_PINC;
			else
				DMAx_Struct->DMA_Stream->CR &= 	~DMA_SxCR_PINC;
				
			break;
			
		default:
			DMAx_Struct->DMA_Stream->PAR 		= (uint32_t)DMAx_Struct->DMA_SourceAddress; 		 // Адрес регистра данных
			DMAx_Struct->DMA_Stream->M0AR		= (uint32_t)DMAx_Struct->DMA_DestinationAddress; // Адрес буфера передачи
			DMAx_Struct->DMA_Stream->NDTR 	= DMAx_Struct->DMA_BufferSize; 									 // Количество данных для передачи
		
			DMAx_Struct->DMA_Stream->CR 		&= ~DMA_SxCR_PSIZE; // Размер данных (8 бит)
			DMAx_Struct->DMA_Stream->CR 		&= ~DMA_SxCR_MSIZE; // Размер данных (8 бит)
		
			switch(DMAx_Struct->DMA_Source_Size){
				case DMA_SIZE_BYTE:
					__NOP();
					break;
				case DMA_SIZE_HALF_WORD:
					DMAx_Struct->DMA_Stream->CR 		|= DMA_SxCR_PSIZE_0; 
					break;
				case DMA_SIZE_WORD:
					DMAx_Struct->DMA_Stream->CR 		|= DMA_SxCR_PSIZE_1; 
					break;
				default:
					__NOP();
					break;
			}
			switch(DMAx_Struct->DMA_Destination_Size){
				case DMA_SIZE_BYTE:
					__NOP();
					break;
				case DMA_SIZE_HALF_WORD:
					DMAx_Struct->DMA_Stream->CR 		|= DMA_SxCR_MSIZE_0; 
					break;
				case DMA_SIZE_WORD:
					DMAx_Struct->DMA_Stream->CR 		|= DMA_SxCR_MSIZE_1; 
					break;
				default:
					__NOP();
					break;
			}
			
			if(DMAx_Struct->DMA_Source_Increment)
				DMAx_Struct->DMA_Stream->CR |= 	DMA_SxCR_PINC;
			else
				DMAx_Struct->DMA_Stream->CR &= 	~DMA_SxCR_PINC;
			
			if(DMAx_Struct->DMA_Destination_Increment)
				DMAx_Struct->DMA_Stream->CR |= 	DMA_SxCR_MINC;
			else
				DMAx_Struct->DMA_Stream->CR &= 	~DMA_SxCR_MINC;
				
			break;
	}
	
	// DMAx_Struct->DMA_Stream->CR 	|= DMA_SxCR_CIRC; // Количество данных для передачи
	
	DMAx_Struct->DMA_Stream->CR	|= DMA_SxCR_TCIE; 
	NVIC_SetPriority(DMA2_Stream2_IRQn, 3);
	NVIC_EnableIRQ(DMA2_Stream2_IRQn);
}

void DMA_SPI_Start(DMA_InitTypeDef* DMAx_Struct, SPI_TypeDef* SPIx){
	DMAx_Struct->DMA_Stream->PAR 		= (uint32_t)DMAx_Struct->DMA_DestinationAddress; // Адрес регистра данных
	DMAx_Struct->DMA_Stream->M0AR		= (uint32_t)DMAx_Struct->DMA_SourceAddress; // Адрес буфера передачи
	DMAx_Struct->DMA_Stream->NDTR = DMAx_Struct->DMA_BufferSize; // Количество данных для передачи
	
	if(SPIx == SPI1){
		DMAx_Struct->DMA->LIFCR |= DMA_LIFCR_CTCIF2 | DMA_LIFCR_CHTIF2 | DMA_LIFCR_CTEIF2 | DMA_LIFCR_CFEIF2;
	}
	
	DMAx_Struct->DMA_Stream->CR |= DMA_SxCR_EN; // Включаем поток
	
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

#if defined(STM32F407xx)
void DMA_DCMI_Start(DMA_InitTypeDef* DMAx_Struct, DCMI_TypeDef* DCMIx){
	// DMAx_Struct->DMA_Stream->CR 		&= ~DMA_SxCR_EN; // Отключаем поток
	
	DMAx_Struct->DMA_Stream->PAR 		= (uint32_t)DMAx_Struct->DMA_SourceAddress; // Адрес регистра данных
  DMAx_Struct->DMA_Stream->M0AR		= (uint32_t)DMAx_Struct->DMA_DestinationAddress; // Адрес буфера передачи 
	DMAx_Struct->DMA_Stream->NDTR = DMAx_Struct->DMA_BufferSize; // Количество данных для передачи
	
	DMAx_Struct->DMA->HIFCR |= DMA_HIFCR_CTCIF7 | DMA_HIFCR_CHTIF7 | DMA_HIFCR_CTEIF7 | DMA_HIFCR_CFEIF7;
	
	DMAx_Struct->DMA_Stream->CR |= DMA_SxCR_EN; // Включаем поток
}
#endif
