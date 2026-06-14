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

extern SPI_InitTypeDef 				SPI_ST7789;
//====================================================================================================
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
		else{
			SPI_ST7789.Baudrate_Prescaler = SPI_BAUDRATE_DIV2;
			SPI_ST7789.Half_Word_Mode = false;
			
			SPI_Init(SPI1, &SPI_ST7789);
		}
	}
}

// Добавим глобальный флаг состояния тача
volatile uint8_t touch_pressed = 0; 
extern volatile Touch_Point touch_pos;

void EXTI0_IRQHandler(void) {
    if (EXTI->PR & EXTI_PR_PR0) {
			
        EXTI->PR = EXTI_PR_PR0; // Сброс флага EXTI
			
			

        // Читаем физическое состояние пина IRQ (PB0)
        // Если PB0 == 0, значит палец НАЖАТ на экран
        if (!(GPIOB->IDR & GPIO_IDR_IDR_0)) {
            
            uint16_t rx = XPT2046_GetRaw_Axis(0xD0); 
            uint16_t ry = XPT2046_GetRaw_Axis(0x90);
            
            // Защита от случайного дребезга контактов:
            // Валидные сырые данные XPT2046 не могут быть строго равны 0 или 4095
            if (rx > 100 && rx < 4000 && ry > 100 && ry < 4000) {
                touch_pressed = 1; // Экран действительно зажат
                XPT2046_Convert_To_Pixels(rx, ry); // Считаем пиксели
								ST77xx_DrawPixel(touch_pos.x, touch_pos.y, BLACK);
            }
            
        } else {
            // Если PB0 == 1, значит палец ОТПУЩЕН
            touch_pressed = 0; 
        }

				EXTI->IMR |= EXTI_IMR_MR0;
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
