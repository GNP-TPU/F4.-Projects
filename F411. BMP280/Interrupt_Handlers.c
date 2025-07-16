//====================================================================================================
#include "main.h"
#include "Interrupt_Handlers.h"
//====================================================================================================
extern USART_InitTypeDef HC_05;

extern DS3231_TypeDef My_Clock;
uint8_t prev_seconds;
bool new_data_available = false;
bool new_pic_available = false;

uint8_t Buffer[LED_WIDTH][LED_HEIGHT];

extern uint8_t Matrix_Pixels[LED_WIDTH][LED_HEIGHT];
extern uint16_t MAX_BRIGHTNESS;
bool Drawing_Mode = false;
bool Just_Entered_Drawing_Mode = true;
//====================================================================================================
extern volatile uint32_t msTicks;

void SysTick_Handler(void){
	msTicks++;
}
//====================================================================================================
void USART1_IRQHandler(void){
	if(USART1->SR & USART_SR_RXNE){
		
		uint8_t byte = USART1->DR;
		HC_05.RxBuffer[HC_05.RxBufferIndex++] = byte;
		
		if(HC_05.RxBuffer[HC_05.RxBufferIndex - 2] == 0x0D && 
			 HC_05.RxBuffer[HC_05.RxBufferIndex - 1] == 0x0A){
			
			switch(HC_05.RxBuffer[HC_05.RxBufferIndex - 3]){
				case 0x01:
					Drawing_Mode = true;
					for(uint8_t y = 0; y < 16; y++){
						for(uint8_t x = 0; x < 16; x++){
							Matrix_Pixels[x + 16][y] = HC_05.RxBuffer[x + y * 16];
						}
					}
					new_pic_available = true;
				break;
				case 0x02:
					My_Clock.Seconds 	= (HC_05.RxBuffer[HC_05.RxBufferIndex - 7] - 0x30) << 4 | (HC_05.RxBuffer[HC_05.RxBufferIndex - 6] - 0x30);
					My_Clock.Minutes 	= (HC_05.RxBuffer[HC_05.RxBufferIndex - 10] - 0x30) << 4 | (HC_05.RxBuffer[HC_05.RxBufferIndex - 9] - 0x30);
					My_Clock.Hours 		= (HC_05.RxBuffer[HC_05.RxBufferIndex - 13] - 0x30) << 4 | (HC_05.RxBuffer[HC_05.RxBufferIndex - 12] - 0x30);
					My_Clock.Year 		= (HC_05.RxBuffer[HC_05.RxBufferIndex - 16] - 0x30) << 4 | (HC_05.RxBuffer[HC_05.RxBufferIndex - 15] - 0x30);
					My_Clock.Month 		= (HC_05.RxBuffer[HC_05.RxBufferIndex - 21] - 0x30) << 4 | (HC_05.RxBuffer[HC_05.RxBufferIndex - 20] - 0x30);
					My_Clock.Date 		= (HC_05.RxBuffer[HC_05.RxBufferIndex - 24] - 0x30) << 4 | (HC_05.RxBuffer[HC_05.RxBufferIndex - 23] - 0x30);
					new_data_available = true;
				break;
				case 0x03:
					MAX_BRIGHTNESS = (HC_05.RxBuffer[HC_05.RxBufferIndex - 4] << 8) | (HC_05.RxBuffer[HC_05.RxBufferIndex - 5]);
				break;
				case 0x04:
					LED_Clear();
					Drawing_Mode = false;
					Just_Entered_Drawing_Mode = true;
					new_data_available = false;	
				break;
			}
			memset(HC_05.RxBuffer, 0, sizeof(HC_05.RxBuffer));
			HC_05.RxBufferIndex = 0;
		}
	}
}

void TIM2_IRQHandler(void){
	if(TIM2->SR & TIM_SR_UIF){
		TIM2->SR &= ~TIM_SR_UIF;
		TIM2->CNT = 0;
		
		if(Drawing_Mode){
			if(Just_Entered_Drawing_Mode){
				Matrix_Pixels[15][1] = 0x00;
				Matrix_Pixels[15][2] = 0x00;
				Matrix_Pixels[15][5] = 0x00;
				Matrix_Pixels[15][6] = 0x00;
				Matrix_Pixels[32][1] = 0x00;
				Matrix_Pixels[32][2] = 0x00;
				Matrix_Pixels[32][5] = 0x00;
				Matrix_Pixels[32][6] = 0x00;
				Matrix_Pixels[15][14] = 0x00;
				
				Just_Entered_Drawing_Mode = false;
			}
			
			uint8_t seconds = I2C_ReadData(I2C1, DS3231_ADDRESS, DS3231_SECONDS_REG);
						
			if(seconds != prev_seconds){
				Matrix_Pixels[47][15] ^= 0x01;
				prev_seconds = seconds;
			}
			
			uint8_t hours = I2C_ReadData(I2C1, DS3231_ADDRESS, DS3231_HOURS_REG);
			LED_InsertChar(((hours >> 4) & 0x0F) + 0x30, 33, 1);
			LED_InsertChar((hours & 0x0F) + 0x30, 40, 1);
						
			uint8_t minutes = I2C_ReadData(I2C1, DS3231_ADDRESS, DS3231_MINUTES_REG);
			LED_InsertChar(((minutes >> 4) & 0x0F) + 0x30, 33, 9);
			LED_InsertChar((minutes & 0x0F) + 0x30, 40, 9);
			
			uint8_t date = I2C_ReadData(I2C1, DS3231_ADDRESS, DS3231_DATE_REG);
			LED_InsertChar(((date >> 4) & 0x0F) + 0x30, 1, 1);
			LED_InsertChar((date & 0x0F) + 0x30, 8, 1);
						
			uint8_t month = I2C_ReadData(I2C1, DS3231_ADDRESS, DS3231_MONTH_REG);
			LED_InsertChar(((month >> 4) & 0x0F) + 0x30, 1, 9);
			LED_InsertChar((month & 0x0F) + 0x30, 8, 9);
			
		}
		else{
			uint8_t hours = I2C_ReadData(I2C1, DS3231_ADDRESS, DS3231_HOURS_REG);
			LED_InsertChar(((hours >> 4) & 0x0F) + 0x30, 1, 1);
			LED_InsertChar((hours & 0x0F) + 0x30, 8, 1);
						
			Matrix_Pixels[15][1] = 0x01;
			Matrix_Pixels[15][2] = 0x01;
			Matrix_Pixels[15][5] = 0x01;
			Matrix_Pixels[15][6] = 0x01;
			
			Matrix_Pixels[16][1] = 0x01;
			Matrix_Pixels[16][2] = 0x01;
			Matrix_Pixels[16][5] = 0x01;
			Matrix_Pixels[16][6] = 0x01;
			
			uint8_t minutes = I2C_ReadData(I2C1, DS3231_ADDRESS, DS3231_MINUTES_REG);
			LED_InsertChar(((minutes >> 4) & 0x0F) + 0x30, 17, 1);
			LED_InsertChar((minutes & 0x0F) + 0x30, 24, 1);
			
			Matrix_Pixels[31][1] = 0x01;
			Matrix_Pixels[31][2] = 0x01;
			Matrix_Pixels[31][5] = 0x01;
			Matrix_Pixels[31][6] = 0x01;
			
			Matrix_Pixels[32][1] = 0x01;
			Matrix_Pixels[32][2] = 0x01;
			Matrix_Pixels[32][5] = 0x01;
			Matrix_Pixels[32][6] = 0x01;
			
			uint8_t seconds = I2C_ReadData(I2C1, DS3231_ADDRESS, DS3231_SECONDS_REG);
			LED_InsertChar(((seconds >> 4) & 0x0F) + 0x30, 33, 1);
			LED_InsertChar((seconds & 0x0F) + 0x30, 40, 1);
			
			uint8_t date = I2C_ReadData(I2C1, DS3231_ADDRESS, DS3231_DATE_REG);
			LED_InsertChar(((date >> 4) & 0x0F) + 0x30, 1, 9);
			LED_InsertChar((date & 0x0F) + 0x30, 8, 9);

			Matrix_Pixels[15][14] = 0x01;
						
			uint8_t month = I2C_ReadData(I2C1, DS3231_ADDRESS, DS3231_MONTH_REG);
			LED_InsertChar(((month >> 4) & 0x0F) + 0x30, 17, 9);
			LED_InsertChar((month & 0x0F) + 0x30, 24, 9);
			
			Matrix_Pixels[31][14] = 0x01;
			
			uint8_t year = I2C_ReadData(I2C1, DS3231_ADDRESS, DS3231_YEAR_REG);
			LED_InsertChar(((year >> 4) & 0x0F) + 0x30, 33, 9);
			LED_InsertChar((year & 0x0F) + 0x30, 40, 9);
		}
		
		if(new_data_available){
			DS3231_Set_Data(&My_Clock);
			new_data_available = false;
		}
		
		TIM2->CR1 |= TIM_CR1_CEN;
	}
	
}

void TIM3_IRQHandler(void){
	if(TIM3->SR & TIM_SR_UIF){
		TIM3->SR &= ~TIM_SR_UIF;
		TIM3->CNT = 0;
		
		LED_Display();
		
		TIM3->CR1 |= TIM_CR1_CEN;
	}
	
}
//====================================================================================================
void HardFault_Handler(void) {
    NVIC_SystemReset();
}
//====================================================================================================
