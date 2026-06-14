//====================================================================================================
#include "main.h"
//====================================================================================================

void RCC_Configure(void);
void GPIO_Configure(void);
void SPI_Configure(void);
void DMA_Configure(void);

SPI_InitTypeDef 				SPI_ST7789;
DMA_InitTypeDef					SPI1_DMA;

uint16_t Display_Buffer[320*80];
//====================================================================================================


void Encoders_Init(void) {
    // 1. Включаем тактирование GPIOA и таймера TIM3
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
		RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;

    // 2. Настраиваем PA6 и PA7 в режим альтернативной функции (AF2 для TIM3)
    GPIOB->MODER   &= ~(GPIO_MODER_MODER4 | GPIO_MODER_MODER5 | GPIO_MODER_MODER6 | GPIO_MODER_MODER7);
    GPIOB->MODER   |= (GPIO_MODER_MODER4_1 | GPIO_MODER_MODER5_1 | GPIO_MODER_MODER6_1 | GPIO_MODER_MODER7_1);

    // Задаем AF2 (TIM3_CH1 и TIM3_CH2)
    GPIOB->AFR[0] &= ~((0xF << GPIO_AFRL_AFSEL4_Pos) | (0xF << GPIO_AFRL_AFSEL5_Pos) | (0xF << GPIO_AFRL_AFSEL6_Pos) | (0xF << GPIO_AFRL_AFSEL7_Pos));
    GPIOB->AFR[0] |=  ((2 << GPIO_AFRL_AFSEL4_Pos) | (2 << GPIO_AFRL_AFSEL5_Pos) | (2 << GPIO_AFRL_AFSEL6_Pos) | (2 << GPIO_AFRL_AFSEL7_Pos));

    // 3. Включаем внутреннюю подтяжку (Pull-up)
    GPIOB->PUPDR   &= ~(GPIO_PUPDR_PUPDR4 | GPIO_PUPDR_PUPDR5);
    GPIOB->PUPDR   |= (GPIO_PUPDR_PUPDR4_0 | GPIO_PUPDR_PUPDR5_0);
	
		GPIOB->PUPDR   &= ~(GPIO_PUPDR_PUPDR6 | GPIO_PUPDR_PUPDR7);
    GPIOB->PUPDR   |= (GPIO_PUPDR_PUPDR6_0 | GPIO_PUPDR_PUPDR7_0);
	
		TIM4->CR1 |= TIM_CR1_CKD_1; 

    // 4. Настройка таймера TIM3 в режим энкодера с фильтрацией дребезга
    TIM3->SMCR     &= ~TIM_SMCR_SMS;
    TIM3->SMCR     |= (TIM_SMCR_SMS_0 | TIM_SMCR_SMS_1); // Режим 3 (TI1 и TI2)
		
		TIM4->SMCR     &= ~TIM_SMCR_SMS;
     TIM4->SMCR |= TIM_SMCR_SMS_0; 

    // Входы + Максимальный цифровой фильтр (0xF) для защиты от дребезга контактов
    TIM3->CCMR1    &= ~(TIM_CCMR1_CC1S | TIM_CCMR1_IC1F | TIM_CCMR1_CC2S | TIM_CCMR1_IC2F);
    TIM3->CCMR1    |= (TIM_CCMR1_CC1S_0 | (0xF << TIM_CCMR1_IC1F_Pos)) |
                      (TIM_CCMR1_CC2S_0 | (0xF << TIM_CCMR1_IC2F_Pos));
											
		TIM4->CCMR1    &= ~(TIM_CCMR1_CC1S | TIM_CCMR1_IC1F | TIM_CCMR1_CC2S | TIM_CCMR1_IC2F);
    TIM4->CCMR1    |= (TIM_CCMR1_CC1S_0 | (0xF << TIM_CCMR1_IC1F_Pos)) |
                      (TIM_CCMR1_CC2S_0 | (0xF << TIM_CCMR1_IC2F_Pos));

    // 1. Очищаем биты полярности для обоих каналов (сброс в состояние по умолчанию)
		TIM3->CCER &= ~(TIM_CCER_CC1P | TIM_CCER_CC1NP | TIM_CCER_CC2P | TIM_CCER_CC2NP);
		
		TIM4->CCER &= ~(TIM_CCER_CC1P | TIM_CCER_CC1NP | TIM_CCER_CC2P | TIM_CCER_CC2NP);
		
		// 2. Инвертируем сигнал на Канале 1 (меняем Rising Edge на Falling Edge)
		TIM3->CCER |= TIM_CCER_CC1P; 
		
		TIM4->CCER |= TIM_CCER_CC1P; 

		// 3. Не забываем включить сами каналы (то, что добавляли для прерываний)
		TIM3->CCER |= (TIM_CCER_CC1E | TIM_CCER_CC2E);
		TIM4->CCER |= (TIM_CCER_CC1E | TIM_CCER_CC2E);
	
    // Задаем диапазон счета (например, от 0 до 100 для удобства регулировки громкости)
    TIM3->ARR       = 100; 
    TIM3->CNT       = 0;
		
		TIM4->ARR       = 100; 
    TIM4->CNT       = 0;
		
    // 5. НАСТРОЙКА ПРЕРЫВАНИЙ ТАЙМЕРА
    // Разрешаем прерывание по захвату CC1 (срабатывает при изменении состояния)
    // Также можно разрешить TIM_DIER_UIE для отслеживания переполнения через 0 или ARR
    TIM3->DIER |= (TIM_DIER_CC1IE | TIM_DIER_UIE); 
		
		TIM4->DIER |= (TIM_DIER_CC1IE | TIM_DIER_UIE); 
    
		// Включаем прерывание TIM3 в контроллере прерываний NVIC
    NVIC_SetPriority(TIM3_IRQn, 2); // Средний приоритет (выберите под вашу систему)
    NVIC_EnableIRQ(TIM3_IRQn);
		
		NVIC_SetPriority(TIM4_IRQn, 2); // Средний приоритет (выберите под вашу систему)
    NVIC_EnableIRQ(TIM4_IRQn);

    // 6. Включаем таймер
    TIM3->CR1      |= TIM_CR1_CEN;
		TIM4->CR1      |= TIM_CR1_CEN;
		
		// 1. Включаем тактование таймера TIM6 в регистре APB1
    RCC->APB2ENR |= RCC_APB2ENR_TIM9EN;
    __DSB(); // Короткая пауза, чтобы шина успела завестись

    // 2. Настраиваем предделитель и период перезагрузки
    TIM9->PSC = 9600 - 1;  // Делитель: частота шины / 10000
    TIM9->ARR = 100 - 1;   // Период: 150 тиков (ровно 15 мс)

    // 3. Разрешаем прерывание по обновлению (Update Interrupt)
    TIM9->DIER |= TIM_DIER_UIE;

    // 4. Включаем прерывание таймера в контроллере NVIC процессора Cortex-M4
    // Позиция TIM6 DAC глобального прерывания в STM32F411 — это индекс 54
    NVIC_SetPriority(TIM1_BRK_TIM9_IRQn, 4); // Ставим средний приоритет
    NVIC_EnableIRQ(TIM1_BRK_TIM9_IRQn);
		
		// 5. Запускаем таймер! (Бит CEN — Counter Enable)
    TIM9->CR1 |= TIM_CR1_CEN;
		
		RCC->APB2ENR |= RCC_APB2ENR_TIM10EN;
    __DSB(); // Короткая пауза, чтобы шина успела завестись
		
		// 2. Настраиваем предделитель и период перезагрузки
    TIM10->PSC = 9600 - 1;  // Делитель: частота шины / 10000
    TIM10->ARR = 10 - 1;   // Период: 150 тиков (ровно 15 мс)

    // 3. Разрешаем прерывание по обновлению (Update Interrupt)
    TIM10->DIER |= TIM_DIER_UIE;

    // 4. Включаем прерывание таймера в контроллере NVIC процессора Cortex-M4
    // Позиция TIM6 DAC глобального прерывания в STM32F411 — это индекс 54
    NVIC_SetPriority(TIM1_UP_TIM10_IRQn, 5); // Ставим средний приоритет
    NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);

    // 5. Запускаем таймер! (Бит CEN — Counter Enable)
    TIM10->CR1 |= TIM_CR1_CEN;
}

void TIM5_Init(void) {
    // 1. Включаем тактирование таймера TIM5 на шине APB1
    RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;

    // 2. Настраиваем частоту (расчет для частоты шины таймеров 96 МГц)
    // Делитель 9600 даст частоту счета 10 кГц (1 тик = 0.1 мс)
    TIM5->PSC = 9600 - 1;   
    TIM5->ARR = 100 - 1;    // Период перезагрузки = 100 тиков (ровно 10 мс)

    // 3. Разрешаем прерывание по обновлению счетчика (Update Interrupt)
    TIM5->DIER |= TIM_DIER_UIE;

    // 4. Настраиваем контроллер прерываний NVIC для TIM5
    NVIC_SetPriority(TIM5_IRQn, 3); // Задаем средний приоритет
    NVIC_EnableIRQ(TIM5_IRQn);

    // 5. Запускаем таймер TIM5
    TIM5->CR1 |= TIM_CR1_CEN;
}

extern char* button_labels[12];
//====================================================================================================
int main(void){
	RCC_Configure();
	GPIO_Configure();
	
	SPI_Configure();
	DMA_Configure();
	
	ST77xx_Init();
  
  for(uint8_t x = 0; x < 4; x++){
		for(uint8_t y = 0; y < 3; y++){
			ST77xx_DrawRect(x * 80, y * 80, 80, 80, 0xFFFF);
		}
	}
	
	for (uint8_t i = 0; i < 12; i++) {
		uint8_t col = i % 4; // Остаток от деления дает колонку
		uint8_t row = i / 4; // Целочисленное деление дает строку

		ST77xx_Draw_GFX_String_Centered(button_labels[i], col, row, WHITE);      
  }
	
	ST77xx_SetWindow(0, 0, 63, 63);
	
	/*
	SPI_ST7789.Baudrate_Prescaler = SPI_BAUDRATE_DIV2;
	SPI_ST7789.Half_Word_Mode = true;
	SPI_Init(SPI1, &SPI_ST7789);
	
	ST77xx_DC_HIGH();
	ST77xx_CS_LOW();
	
	SPI1_DMA.DMA_SourceAddress			= (uint32_t*)Display_Buffer;
	SPI1_DMA.DMA_BufferSize					=	64*64;
	DMA_SPI_Start(&SPI1_DMA, SPI1);
	*/
	
	USB_Core_Init();
	Encoders_Init();
	TIM5_Init();
	
	
	
	
	
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
	RCC_InitStruct.PLL_N_Multiplier					= 192;
	RCC_InitStruct.PLL_P_Divider						=	PLL_P_DIV2;
	RCC_InitStruct.PLL_Q_Divider						=	4;
	
	SystemCoreClockConfigure(&RCC_InitStruct);
}

void GPIO_Configure(void){
	
	GPIO_InitTypeDef GPIO_InitStruct;
	
	// SPI and display
	GPIO_InitStruct.Pin 			= GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;									
	GPIO_InitStruct.Mode 			= MODE_AF;
	GPIO_InitStruct.Type 			= TYPE_PP;
	GPIO_InitStruct.Speed 		= GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate	= AF_SPI1;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin 			= GPIO_PIN_0 | GPIO_PIN_2 | GPIO_PIN_10;									
	GPIO_InitStruct.Mode 			= MODE_OUTPUT;
	GPIO_InitStruct.Type 			= TYPE_PP;
	GPIO_InitStruct.Speed 		= GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	// USB (PA11(DM), PA12(DP))
	
	GPIO_InitStruct.Pin 			= GPIO_PIN_11 | GPIO_PIN_12;	
	GPIO_InitStruct.Mode 			= MODE_AF;
	GPIO_InitStruct.Type 			= TYPE_PP;
	GPIO_InitStruct.Speed 		= GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate	= AF_USB_OTG_FS;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	// Keys
	GPIO_InitStruct.Pin 			= GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_15;	
	GPIO_InitStruct.Mode 			= MODE_INPUT;
	GPIO_InitStruct.Type 			= TYPE_PP;
	GPIO_InitStruct.Speed 		= GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Pull			= PULL_UP;
	GPIO_Init(GPIOA, &GPIO_InitStruct);	
	
	GPIO_InitStruct.Pin 			= GPIO_PIN_3 | GPIO_PIN_8 | GPIO_PIN_9;	
	GPIO_InitStruct.Mode 			= MODE_INPUT;
	GPIO_InitStruct.Type 			= TYPE_PP;
	GPIO_InitStruct.Speed 		= GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Pull			= PULL_UP;
	GPIO_Init(GPIOB, &GPIO_InitStruct);		

	GPIO_InitStruct.Pin 			= GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;	
	GPIO_InitStruct.Mode 			= MODE_INPUT;
	GPIO_InitStruct.Type 			= TYPE_PP;
	GPIO_InitStruct.Speed 		= GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Pull			= PULL_UP;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	// Encoders' switches
	GPIO_InitStruct.Pin 			= GPIO_PIN_1 | GPIO_PIN_14;	
	GPIO_InitStruct.Mode 			= MODE_INPUT;
	GPIO_InitStruct.Type 			= TYPE_PP;
	GPIO_InitStruct.Speed 		= GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Pull			= PULL_UP;
	GPIO_Init(GPIOB, &GPIO_InitStruct);		

}

void SPI_Configure(void){
	SPI_ST7789.Baudrate_Prescaler = SPI_BAUDRATE_DIV2;
	// SPI_ST7789.Half_Word_Mode = true;
	SPI_Init(SPI1, &SPI_ST7789);
}

void DMA_Configure(void){
	SPI1_DMA.DMA										= DMA2;
	SPI1_DMA.DMA_Stream							= DMA2_Stream2;
	SPI1_DMA.DMA_Channel						= 2;
	SPI1_DMA.DMA_Direction					= DMA_DIRECTION_MEM2PER;
		
	SPI1_DMA.DMA_SourceAddress			= (uint32_t*)Display_Buffer;
	SPI1_DMA.DMA_Source_Increment		= true;
	SPI1_DMA.DMA_Source_Size				= DMA_SIZE_HALF_WORD;
	
	SPI1_DMA.DMA_DestinationAddress			= (uint32_t*)&SPI1->DR;
	SPI1_DMA.DMA_Destination_Increment	= false;
	SPI1_DMA.DMA_Destination_Size				= DMA_SIZE_HALF_WORD;
	
	SPI1_DMA.DMA_BufferSize					=	320*80;
	
	DMA_Init(SPI1_DMA.DMA, &SPI1_DMA);
}
//====================================================================================================




