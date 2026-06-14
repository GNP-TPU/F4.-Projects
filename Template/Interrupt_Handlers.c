//====================================================================================================
#include "main.h"
#include "Interrupt_Handlers.h"
//====================================================================================================
extern volatile uint32_t 	msTicks;

extern DMA_InitTypeDef		DCMI_DMA;
extern DMA_InitTypeDef		SPI_DMA;

extern uint32_t Data[(IMG_WIDTH/2) * IMG_HEIGHT];

extern bool data_ready;

extern bool step;

extern bool sobel_processing;
extern bool dcmi_started;

extern uint8_t grayscale_image[IMG_WIDTH*IMG_HEIGHT];

uint32_t frame = 1, line = 0, hsync = 0;
uint8_t current_tile = 1, dcmi_started_flag = 0;

uint8_t pwm_counter = 0;
//====================================================================================================
void SysTick_Handler(void){
	msTicks++;
}
//====================================================================================================
void TIM1_UP_TIM10_IRQHandler(void){
	if(TIM1->SR & TIM_SR_UIF){
		TIM1->SR &= ~TIM_SR_UIF;
		pwm_counter++;
		if(pwm_counter > 1){
			TIM1->CR1 &= ~TIM_CR1_CEN;
			pwm_counter = 0;
			step = true;
		}
		
	}
}

void DCMI_IRQHandler(void){
	if(DCMI->RISR & DCMI_RISR_VSYNC_RIS){
		DCMI->ICR |= DCMI_ICR_VSYNC_ISC;	
		// DCMI->IER &= ~DCMI_IER_VSYNC_IE;
		
		DCMI->CR |= DCMI_CR_CAPTURE;
	}
	if(DCMI->RISR & DCMI_RISR_LINE_RIS){
		DCMI->ICR |= DCMI_ICR_LINE_ISC;
	}
	if(DCMI->RISR & DCMI_RISR_FRAME_RIS){
		DCMI->ICR |= DCMI_ICR_FRAME_ISC;
		// DCMI->IER &= ~DCMI_IER_FRAME_IE;
	}
}

void DMA2_Stream7_IRQHandler(void){
	if(DMA2->HISR & DMA_HISR_TCIF7){
		DMA2->HIFCR |= DMA_HIFCR_CTCIF7;
		DMA2_Stream7->CR &= ~DMA_SxCR_EN;
		DCMI->CR &= ~(DCMI_CR_CAPTURE);
		DCMI->IER &= ~(DCMI_IER_LINE_IE | DCMI_IER_FRAME_IE | DCMI_IER_VSYNC_IE);
		DCMI->CR &= ~(DCMI_CR_CAPTURE);
		dcmi_started = false;
		data_ready = true;
		
		SPI_DMA.DMA_BufferSize	= IMG_WIDTH * IMG_HEIGHT;
		DMA_SPI_Start(&SPI_DMA, SPI1);
		
	}
}

void DMA2_Stream3_IRQHandler(void){
	if(DMA2->LISR & DMA_LISR_TCIF3){
		DMA2->LIFCR |= DMA_LIFCR_CTCIF3;
		DMA2_Stream3->CR &= ~DMA_SxCR_EN;
		if(!sobel_processing){
			sobel_processing = true;
		}
		
		DCMI_DMA.DMA_BufferSize	= (IMG_WIDTH/2) * IMG_HEIGHT;
		DMA_DCMI_Start(&DCMI_DMA, DCMI);
		DCMI->IER |= DCMI_IER_LINE_IE | DCMI_IER_FRAME_IE | DCMI_IER_VSYNC_IE;
	}
}
//====================================================================================================
void USART1_IRQHandler(void){
	
}

void TIM2_IRQHandler(void){
	
}
//====================================================================================================
void USART2_IRQHandler(void){
	
}

void TIM3_IRQHandler(void){
	
}
//====================================================================================================
void HardFault_Handler(void) {
    NVIC_SystemReset();
}
//====================================================================================================
