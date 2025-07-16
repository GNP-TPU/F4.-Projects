//====================================================================================================
#include "main.h"
//====================================================================================================
void RCC_Configure(void);
void GPIO_Configure(void);
void USART_Configure(void);
void SPI_Configure(void);
void I2C_Configure(void);
void DMA_Configure(void);
void TIM_Configure(void);
//====================================================================================================
// Struct initialization
USART_InitTypeDef				HC_05;

SPI_InitTypeDef 				SPI_ST7789;
SPI_InitTypeDef 				SPI_XPT2046;
I2C_InitTypeDef 				I2C_RTC;

DMA_InitTypeDef					SPI1_DMA;

uint16_t Color[320*80];

uint32_t Counter = 0;
//====================================================================================================
int main(void){
	RCC_Configure();
	GPIO_Configure();
	SPI_Configure();
	DMA_Configure();
	
	ST77xx_Init();
	
	for (uint32_t i = 0; i < 320*80; i++) {
    Color[i] = RED;
	}
	
	ST77xx_SetWindow(0, 0, 319, 239);
	
	SPI_ST7789.Baudrate_Prescaler = SPI_BAUDRATE_DIV2;
	SPI_ST7789.Half_Word_Mode = true;
	
	SPI_Init(SPI1, &SPI_ST7789);
	
	ST77xx_DC_HIGH();
	ST77xx_CS_LOW();
	
	DMA_SPI_Start(&SPI1_DMA, SPI1);
	
	while(1){
		
	}

}
//====================================================================================================
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
	RCC_InitStruct.PLL_N_Multiplier					= 200;
	RCC_InitStruct.PLL_P_Divider						=	PLL_P_DIV2;
	
	SystemCoreClockConfigure(&RCC_InitStruct);
}

void GPIO_Configure(void){
	GPIO_InitTypeDef GPIO_InitStruct;
	
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
	
	GPIO_InitStruct.Pin 			= GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;									
	GPIO_InitStruct.Mode 			= MODE_AF;
	GPIO_InitStruct.Type 			= TYPE_PP;
	GPIO_InitStruct.Speed 		= GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate	= AF_SPI2;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin 			= GPIO_PIN_12;									
	GPIO_InitStruct.Mode 			= MODE_OUTPUT;
	GPIO_InitStruct.Type 			= TYPE_PP;
	GPIO_InitStruct.Speed 		= GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	/*
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN; // Включаем тактирование SYSCFG
	
	GPIO_InitStruct.Pin 			= GPIO_PIN_0;			
	GPIO_InitStruct.Mode 			= MODE_INPUT;
	GPIO_InitStruct.Pull 			= PULL_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI0; // Очищаем биты для EXTI0
  SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PB; // Выбираем PB0 для EXTI0
	
	EXTI->FTSR |= EXTI_FTSR_TR0; // По спаду (falling edge)
	EXTI->RTSR |= EXTI_RTSR_TR0; // По спаду (falling edge)
	
	EXTI->IMR |= EXTI_IMR_MR0; // Разрешаем маску прерывания для EXTI0
	
	NVIC_EnableIRQ(EXTI0_IRQn); // Разрешаем прерывание EXTI0 в NVIC
	*/
}

void USART_Configure(void){
	HC_05.Baudrate = 115200;
	
	HC_05.Rx_Enable = true;
	HC_05.Tx_Enable = true;
	
	HC_05.Rx_IRq_Enable = true;
	
	USART_Init(USART1, &HC_05);
}

void SPI_Configure(void){
	SPI_ST7789.Baudrate_Prescaler = SPI_BAUDRATE_DIV2;
	
	SPI_Init(SPI1, &SPI_ST7789);
}

void TIM_Configure(void){
	TIM_InitTypeDef TIMx;
	
	TIM_Init(TIM3, &TIMx);
	
	TIM_Init(TIM2, &TIMx);
}

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

void I2C_Configure(void){
	I2C_RTC.PeripheralBusFrequency = 50000000;
	
	I2C_Init(I2C1, &I2C_RTC);
}
//====================================================================================================

