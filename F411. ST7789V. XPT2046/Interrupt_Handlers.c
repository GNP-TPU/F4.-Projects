//====================================================================================================
#include "main.h"
#include "Interrupt_Handlers.h"
//====================================================================================================
uint32_t exti_counter_fall = 0;
uint32_t exti_counter_rise = 0;

extern DMA_InitTypeDef SPI1_DMA;
extern uint16_t Color[320*80];
uint16_t Current_Color = RED;
uint32_t Color_Switch = 1;
//====================================================================================================
extern volatile uint32_t msTicks;

void SysTick_Handler(void){
	msTicks++;
}
//====================================================================================================
void DMA2_Stream2_IRQHandler(void){
	if(DMA2->LISR & DMA_LISR_TCIF2){
		DMA2->LIFCR |= DMA_LIFCR_CTCIF2;
		// DMA2_Stream2->CR &= ~DMA_SxCR_EN;
		
		Color_Switch++;
		switch(Color_Switch){
			case 2:
				Current_Color = GREEN;
				break;
			case 3:
				Current_Color = BLUE;
				break;
		}
		
		for (uint32_t i = 0; i < 320*80; i++) {
			Color[i] = Current_Color;
		}
		
		
		if(Color_Switch < 4){
			DMA_SPI_Start(&SPI1_DMA, SPI1);
		}
	}
}

void EXTI0_IRQHandler(void) {
	if (EXTI->PR & EXTI_PR_PR0) {
    // Очищаем флаг прерывания
		EXTI->PR |= EXTI_PR_PR0;
		char buffer[100];
		if(GPIOB->IDR & GPIO_IDR_ID0){
			exti_counter_rise++;
			sprintf(buffer, "%d", exti_counter_rise);
			ST77xx_WriteString(buffer, 0, 0, BLACK, WHITE);
		}
		else{
			exti_counter_fall++;
			sprintf(buffer, "%d", exti_counter_fall);
			ST77xx_WriteString(buffer, 0, 60, BLACK, WHITE);
		}
  }
}

void USART1_IRQHandler(void){
	
}

void TIM2_IRQHandler(void){
	if(TIM2->SR & TIM_SR_UIF){
		TIM2->SR &= ~TIM_SR_UIF;
		TIM2->CNT = 0;
		
		TIM2->CR1 |= TIM_CR1_CEN;
	}
	
}

void TIM3_IRQHandler(void){
	if(TIM3->SR & TIM_SR_UIF){
		TIM3->SR &= ~TIM_SR_UIF;
		TIM3->CNT = 0;
		
		TIM3->CR1 |= TIM_CR1_CEN;
	}
	
}
//====================================================================================================
void HardFault_Handler(void) {
    NVIC_SystemReset();
}
//====================================================================================================
