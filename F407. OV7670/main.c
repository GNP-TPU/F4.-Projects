//====================================================================================================
#include "main.h"
//====================================================================================================
void System_Configure(void);

void RCC_Configure(void);
void GPIO_Configure(void);
void USART_Configure(void);
void DMA_Configure(void);
void TIM_Configure(void);
void SPI_Configure(void);
void I2C_Configure(void);
void DCMI_Configure(void);
//====================================================================================================
// Struct initialization
DMA_InitTypeDef		DCMI_DMA;
DMA_InitTypeDef   SPI_DMA;

I2C_InitTypeDef 	I2C_Camera;
SPI_InitTypeDef		SPI_ILI9488;
//====================================================================================================
int ov_init_status = 0;

uint32_t Data[(IMG_WIDTH/2) * IMG_HEIGHT];

bool data_ready = false;

bool repetition_flag = false;

bool sobel_processing = false;
bool dcmi_started = false;

bool step = false;

uint8_t grayscale_image[IMG_WIDTH*IMG_HEIGHT + 100];

int sobel_output_current, sobel_output_previous, sobel_output_previous_previous,
		sobel_max_current, sobel_max_previous;

uint8_t dir_flag = 0x00;
uint8_t sobel_counter_searching_focus = 0, sobel_counter_lost_focus = 0;
//====================================================================================================
int main(void){
	System_Configure();
	
	lcd_init_spi();
	delay_ms(100);
	GPIO_Pin_High(GPIOB, GPIO_PIN_0);
	delay_ms(100);
	
	GPIO_Pin_Low(GPIOC, GPIO_PIN_5);
	delay_ms(100);
	GPIO_Pin_High(GPIOC, GPIO_PIN_5);
	delay_ms(100);
	
	set_draw_window(90, 50, 90 + IMG_WIDTH - 1, 50 + IMG_HEIGHT - 1);
	
	SPI_ILI9488.Baudrate_Prescaler = SPI_BAUDRATE_DIV2;
	SPI_ILI9488.Clock_Polarity = 0;
	SPI_ILI9488.Clock_Phase = 0;
	SPI_ILI9488.Half_Word_Mode = true;
	
	SPI_Init(SPI1, &SPI_ILI9488);
	
	GPIO_Pin_High(DC_PORT, DC_PIN);
	GPIO_Pin_Low(CS_PORT, CS_PIN);
	
	I2C_WriteData(I2C1, 0x42, 0x12, 0x80);
	delay_ms(500);
	I2C_WriteData(I2C1, 0x42, 0x12, 0x80);
	delay_ms(500);
	
	ov_init_status = camera_config();
	
	//DCMI_DMA.DMA_Stream->CR 	|= DMA_SxCR_CIRC; // Количество данных для передачи
	DCMI_DMA.DMA_BufferSize	= (IMG_WIDTH/2) * IMG_HEIGHT;
	DMA_DCMI_Start(&DCMI_DMA, DCMI);
	
	DCMI->IER |= DCMI_IER_LINE_IE | DCMI_IER_FRAME_IE | DCMI_IER_VSYNC_IE;
	NVIC_SetPriority(DCMI_IRQn, 0);
	NVIC_EnableIRQ(DCMI_IRQn);
	
	delay_ms(110);
	
//	DCMI_DMA.DMA_BufferSize	= sizeof(Data);
//	DMA_DCMI_Start(&DCMI_DMA, DCMI);
//	DCMI->CR |= DCMI_CR_CAPTURE;
	
	GPIO_Pin_High(GPIOD, GPIO_PIN_1); // MS1
	GPIO_Pin_High(GPIOD, GPIO_PIN_2); // MS2
	GPIO_Pin_Low(GPIOD, GPIO_PIN_3); // MS3
	GPIO_Pin_High(GPIOD, GPIO_PIN_4); // Reset
	GPIO_Pin_Low(GPIOD, GPIO_PIN_5); // DIR
	
	GPIO_Pin_Low(GPIOD, GPIO_PIN_0); // Enable
	
	dir_flag = 0;
	GPIO_Pin_High(GPIOD, GPIO_PIN_12);
	GPIO_Pin_Low(GPIOD, GPIO_PIN_13);
	STEPPER_MOTOR_CCLKWISE;
	
	
	while(1){
//		if(repetition_flag){
//			repetition_flag = false;
//			delay_ms(500);
//			if(dir_flag)
//				GPIO_Pin_Low(GPIOD, GPIO_PIN_5); // DIR
//			else
//				GPIO_Pin_High(GPIOD, GPIO_PIN_5); // DIR
//			dir_flag ^= 1;
//			// TIM1->CR1 |= TIM_CR1_CEN;
//		}
		
		if(sobel_processing){
			rgb_to_grayscale(Data, grayscale_image, IMG_WIDTH, IMG_HEIGHT);
			
			sobel_output_previous_previous = sobel_output_previous;
			sobel_output_previous = sobel_output_current;
			sobel_output_current = apply_sobel(grayscale_image, IMG_WIDTH, IMG_HEIGHT) / 10000;
			
			if(step){
				if(
					(sobel_output_previous < sobel_output_previous_previous && sobel_output_current < sobel_output_previous)
				)
				{
					dir_flag ^= 1;
					if(dir_flag == 0){
						GPIO_Pin_High(GPIOD, GPIO_PIN_12);
						GPIO_Pin_Low(GPIOD, GPIO_PIN_13);
						STEPPER_MOTOR_CCLKWISE;
					}
					else{
						GPIO_Pin_High(GPIOD, GPIO_PIN_13);
						GPIO_Pin_Low(GPIOD, GPIO_PIN_12);
						STEPPER_MOTOR_CLKWISE;
					}
				}
				
				if(sobel_output_current > sobel_output_previous)
					sobel_max_current = sobel_output_current;
				
				if((sobel_output_current + 2 < sobel_max_current && sobel_output_current - 2 < sobel_max_current)){
					TIM1->CR1 &= ~TIM_CR1_CEN;
					GPIO_Pin_Low(GPIOD, GPIO_PIN_12);
					GPIO_Pin_Low(GPIOD, GPIO_PIN_13);
				}
				else{
					TIM1->CR1 |= TIM_CR1_CEN;
					if(dir_flag == 0){
						GPIO_Pin_High(GPIOD, GPIO_PIN_12);
						GPIO_Pin_Low(GPIOD, GPIO_PIN_13);
					}
					else{
						GPIO_Pin_High(GPIOD, GPIO_PIN_13);
						GPIO_Pin_Low(GPIOD, GPIO_PIN_12);
					}
				}
				
	//			if((sobel_output_current + 2 < sobel_output_previous && sobel_output_current - 2 < sobel_output_previous)){
	//				TIM1->CR1 &= ~TIM_CR1_CEN;
	//				sobel_max_current = sobel_output_previous;
	//			}
	//			else{
	//				TIM1->CR1 |= TIM_CR1_CEN;
	//			}
				
	//			if(sobel_output_current + 2 > sobel_max_current && sobel_output_current - 2 < sobel_max_current){
	//				sobel_max_current = 0;
	//				TIM1->CR1 |= TIM_CR1_CEN;
	//			}
	//			else{
	//				TIM1->CR1 &= ~TIM_CR1_CEN;
	//			}
				
				
	//			if(((sobel_output_current + 3) > sobel_output_previous) && ((sobel_output_current - 3) < sobel_output_previous)){
	//				GPIO_Pin_High(GPIOD, GPIO_PIN_0); // Enable
	//			}
	//			else{
	//				GPIO_Pin_Low(GPIOD, GPIO_PIN_0); // Enable
	//			}
			}
			sobel_processing = false;
		}
	}
}
//====================================================================================================
void System_Configure(void){
	RCC_Configure();	
	GPIO_Configure();
	// TIM_Configure();
	SPI_Configure();
	I2C_Configure();
	DCMI_Configure();
	DMA_Configure();
	
	
	// GPIO_Pin_High(GPIOD, GPIO_PIN_14);
}

void RCC_Configure(void){
	RCC_InitTypeDef 	RCC_InitStruct;
	
	RCC_InitStruct.ManualCalculatePLL				= false;
	
	RCC_InitStruct.OscillatorType						= OSC_EXTERNAL;	
	RCC_InitStruct.SystemClockSource				= PLL_CLK_SRC;
	
	RCC_InitStruct.ExternalOscillatorFreq 	= 8000000;
	
	RCC_InitStruct.AHB_Prescaler						= AHB_PRESCALER_DIV1;
	RCC_InitStruct.APB1_Prescaler						= APB_PRESCALER_DIV2;
	RCC_InitStruct.APB2_Prescaler						= APB_PRESCALER_DIV1;
		
	RCC_InitStruct.PLL_M_Divider						= 8;
	RCC_InitStruct.PLL_N_Multiplier					= 168;
	RCC_InitStruct.PLL_P_Divider						=	PLL_P_DIV2;
	
	RCC_InitStruct.MCO1_Enable							= true;
	RCC_InitStruct.MCO1_Source							= HSI_MCO_SRC;
	RCC_InitStruct.MCO1_Prescaler						= MCO_DIV1;
	
	SystemCoreClockConfigure(&RCC_InitStruct);
}

void GPIO_Configure(void){
	GPIO_InitTypeDef GPIO_InitStruct;
	
	GPIO_InitStruct.Pin 			= GPIO_PIN_14;									// TX mode for MAX485 
	GPIO_InitStruct.Mode 			= MODE_OUTPUT;
	GPIO_InitStruct.Pull 			= PULL_FLOATING;
	GPIO_InitStruct.Type 			= TYPE_PP;
	GPIO_InitStruct.Speed 		= GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_Init(GPIOD, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin 			= GPIO_PIN_5;									// TX mode for MAX485 
	GPIO_InitStruct.Mode 			= MODE_OUTPUT;
	GPIO_InitStruct.Pull 			= PULL_FLOATING;
	GPIO_InitStruct.Type 			= TYPE_PP;
	GPIO_InitStruct.Speed 		= GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin 			= GPIO_PIN_8 | GPIO_PIN_9;									
	GPIO_InitStruct.Mode 			= MODE_AF;
	GPIO_InitStruct.Pull 			= PULL_UP;
	GPIO_InitStruct.Type 			= TYPE_OD;
	GPIO_InitStruct.Speed 		= GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate	= AF_I2C1;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin 			= GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9;									
	GPIO_InitStruct.Mode 			= MODE_AF;
	GPIO_InitStruct.Pull 			= PULL_UP;
	GPIO_InitStruct.Type 			= TYPE_PP;
	GPIO_InitStruct.Speed 		= GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate	= AF_DCMI;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin 			= GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6;									
	GPIO_InitStruct.Mode 			= MODE_AF;
	GPIO_InitStruct.Pull 			= PULL_UP;
	GPIO_InitStruct.Type 			= TYPE_PP;
	GPIO_InitStruct.Speed 		= GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate	= AF_DCMI;
	GPIO_Init(GPIOE, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin 			= GPIO_PIN_6 | GPIO_PIN_7;									
	GPIO_InitStruct.Mode 			= MODE_AF;
	GPIO_InitStruct.Pull 			= PULL_UP;
	GPIO_InitStruct.Type 			= TYPE_PP;
	GPIO_InitStruct.Speed 		= GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Alternate	= AF_DCMI;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin 			= GPIO_PIN_4 | GPIO_PIN_6;									
	GPIO_InitStruct.Mode 			= MODE_AF;
	GPIO_InitStruct.Pull 			= PULL_UP;
	GPIO_InitStruct.Type 			= TYPE_PP;
	GPIO_InitStruct.Speed 		= GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate	= AF_DCMI;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin 			= GPIO_PIN_5 | GPIO_PIN_7;									
	GPIO_InitStruct.Mode 			= MODE_AF;
	GPIO_InitStruct.Pull 			= PULL_UP;
	GPIO_InitStruct.Type 			= TYPE_PP;
	GPIO_InitStruct.Speed 		= GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate	= AF_SPI1;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin 			= GPIO_PIN_4;									
	GPIO_InitStruct.Mode 			= MODE_AF;
	GPIO_InitStruct.Pull 			= PULL_DOWN;
	GPIO_InitStruct.Type 			= TYPE_PP;
	GPIO_InitStruct.Speed 		= GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate	= AF_SPI1;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin 			= GPIO_PIN_0;									
	GPIO_InitStruct.Mode 			= MODE_OUTPUT;
	GPIO_InitStruct.Pull 			= PULL_FLOATING;
	GPIO_InitStruct.Type 			= TYPE_PP;
	GPIO_InitStruct.Speed 		= GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin 			= GPIO_PIN_10 | GPIO_PIN_8 | GPIO_PIN_9;									
	GPIO_InitStruct.Mode 			= MODE_OUTPUT;
	GPIO_InitStruct.Pull 			= PULL_UP;
	GPIO_InitStruct.Type 			= TYPE_PP;
	GPIO_InitStruct.Speed 		= GPIO_SPEED_FREQ_HIGH;
	GPIO_Init(GPIOD, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin 			= GPIO_PIN_13 | GPIO_PIN_12;									
	GPIO_InitStruct.Mode 			= MODE_OUTPUT;
	GPIO_InitStruct.Pull 			= PULL_UP;
	GPIO_InitStruct.Type 			= TYPE_PP;
	GPIO_InitStruct.Speed 		= GPIO_SPEED_FREQ_HIGH;
	GPIO_Init(GPIOD, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin 			= GPIO_PIN_9;									
	GPIO_InitStruct.Mode 			= MODE_AF;
	GPIO_InitStruct.Pull 			= PULL_DOWN;
	GPIO_InitStruct.Type 			= TYPE_PP;
	GPIO_InitStruct.Speed 		= GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Alternate	= AF_TIM1;
	GPIO_Init(GPIOE, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin 			= GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5;									// TX mode for MAX485 
	GPIO_InitStruct.Mode 			= MODE_OUTPUT;
	GPIO_InitStruct.Pull 			= PULL_UP;
	GPIO_InitStruct.Type 			= TYPE_PP;
	GPIO_InitStruct.Speed 		= GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_Init(GPIOD, &GPIO_InitStruct);

	GPIO_Pin_High(GPIOD, GPIO_PIN_0); // Enable
	GPIO_Pin_Low(GPIOD, GPIO_PIN_1); // MS1
	GPIO_Pin_Low(GPIOD, GPIO_PIN_2); // MS2
	GPIO_Pin_Low(GPIOD, GPIO_PIN_3); // MS3
	GPIO_Pin_Low(GPIOD, GPIO_PIN_4); // Reset
	GPIO_Pin_Low(GPIOD, GPIO_PIN_5); // DIR
	
	
	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
	
	TIM1->CR1 |= TIM_CR1_URS;
	
	TIM1->PSC = (uint16_t)(SystemCoreClock / 1000000) - 1;
	TIM1->ARR = (uint16_t)(1000); // width
	
	TIM1->CCR1 = (uint16_t)(500); // pulse
	TIM1->CCMR1 &= ~(TIM_CCMR1_OC1M); 
	TIM1->CCMR1 |= (TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1) | TIM_CCMR1_OC1PE;
	TIM1->CCER |= TIM_CCER_CC1E;
	
	TIM1->BDTR = TIM_BDTR_MOE;
	TIM1->DIER |= TIM_DIER_UIE;
	TIM1->SR &= ~TIM_SR_UIF;
	
	TIM1->RCR = 20 - 1;
	TIM1->EGR |= TIM_EGR_UG;
	
	
	TIM1->CR1 |= TIM_CR1_CEN;
	
	NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);
}

void I2C_Configure(void){
	I2C_Camera.PeripheralBusFrequency = 42000000;
	
	I2C_Init(I2C1, &I2C_Camera);
}

void DCMI_Configure(void){
	RCC->AHB2ENR |= RCC_AHB2ENR_DCMIEN;
	
	DCMI->CR &= ~(DCMI_CR_EDM_1 | DCMI_CR_EDM_0); 	// количество бит
	DCMI->CR &= ~DCMI_CR_CM; 												// режим (continuous, snapshot)
	DCMI->CR &= ~DCMI_CR_ESS;												// синхронизация hardware
	DCMI->CR |= DCMI_CR_PCKPOL;											// rising edge
	DCMI->CR &= ~DCMI_CR_HSPOL;											// active low
	DCMI->CR |= DCMI_CR_VSPOL;											// active high
	DCMI->CR &= ~(DCMI_CR_FCRC_1 | DCMI_CR_FCRC_0);	// all frames captured
	
	DCMI->CR |= DCMI_CR_CROP;
	
	//DCMI->CWSTRTR |= ((IMG_HEIGHT*2 - 1) << 16 | (IMG_WIDTH*2 - 1));
	//DCMI->CWSIZER |= ((IMG_HEIGHT - 1) << 16 | (IMG_WIDTH - 1));
	
	//DCMI->CWSIZER |= ((IMG_HEIGHT - 1) << 16 | (IMG_WIDTH*2 - 1)); // QVGA без последствий
	
	DCMI->CWSIZER |= (((IMG_HEIGHT - 1) << 16) | (IMG_WIDTH*2 - 1)); // QVGA без последствий
	//DCMI->CWSIZER |= ((IMG_HEIGHT - 1) << 16 | (IMG_WIDTH - 1));
	
	DCMI->CR |= DCMI_CR_ENABLE;											// enable DCMI
}

void DMA_Configure(void){
	DCMI_DMA.DMA										= DMA2;
	DCMI_DMA.DMA_Stream							= DMA2_Stream7;
	DCMI_DMA.DMA_Channel						= 1;
	DCMI_DMA.DMA_Direction					= DMA_DIRECTION_PER2MEM;
	
	DCMI_DMA.DMA_SourceAddress			= (uint32_t*)&DCMI->DR;
	DCMI_DMA.DMA_Source_Increment		= false;
	DCMI_DMA.DMA_Source_Size				= DMA_SIZE_WORD;
	
	DCMI_DMA.DMA_DestinationAddress	= (uint32_t*)Data;
	DCMI_DMA.DMA_Destination_Increment	= true;
	DCMI_DMA.DMA_Destination_Size				= DMA_SIZE_WORD;
	
	DCMI_DMA.DMA_BufferSize	= IMG_HEIGHT*IMG_WIDTH/2;
	
	DMA_Init(DCMI_DMA.DMA, &DCMI_DMA);
	
	NVIC_SetPriority(DMA2_Stream7_IRQn, 1);
	NVIC_EnableIRQ(DMA2_Stream7_IRQn);
	
	
	SPI_DMA.DMA										= DMA2;
	SPI_DMA.DMA_Stream						= DMA2_Stream3;
	SPI_DMA.DMA_Channel						= 3;
	SPI_DMA.DMA_Direction					= DMA_DIRECTION_MEM2PER;
	
	SPI_DMA.DMA_SourceAddress			= (uint32_t*)Data;
	SPI_DMA.DMA_Source_Increment	= true;
	SPI_DMA.DMA_Source_Size				= DMA_SIZE_HALF_WORD;
	
	SPI_DMA.DMA_DestinationAddress	= (uint32_t*)&SPI1->DR;
	SPI_DMA.DMA_Destination_Increment	= false;
	SPI_DMA.DMA_Destination_Size				= DMA_SIZE_HALF_WORD;
	
	SPI_DMA.DMA_BufferSize	= IMG_HEIGHT*IMG_WIDTH;
	
	DMA_Init(SPI_DMA.DMA, &SPI_DMA);
	
	NVIC_SetPriority(DMA2_Stream3_IRQn, 2);
	NVIC_EnableIRQ(DMA2_Stream3_IRQn);
	
}

void TIM_Configure(void){
	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
	
	TIM1->PSC = 3499;
	TIM1->ARR = (uint16_t)555;
	
	
	TIM1->CCR1 = (uint16_t)(100);
	TIM1->CCMR1 &= ~(TIM_CCMR1_OC1M); 
	TIM1->CCMR1 |= (TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1);
	TIM1->CCER |= TIM_CCER_CC1E;
	
	TIM1->CR1 |= TIM_CR1_CEN;
	

}

void SPI_Configure(void){
	SPI_ILI9488.Baudrate_Prescaler = SPI_BAUDRATE_DIV32;
	SPI_ILI9488.Clock_Polarity = 0;
	SPI_ILI9488.Clock_Phase = 0;
	
	SPI_Init(SPI1, &SPI_ILI9488);
}
//====================================================================================================

