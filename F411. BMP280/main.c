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

SPI_InitTypeDef 				SPI_Pixel_Matrix;
I2C_InitTypeDef 				I2C_BMP;

LED_Matrix_TypeDef			LED_Matrix;
DS3231_TypeDef 					My_Clock;

BMP_TypeDef							BMP280;
//====================================================================================================
uint8_t status;
uint8_t shutdown_mode = 0;

const uint8_t digit_to_segment[] = {
    0b01111110, // 0
    0b00000110, // 1
    0b01011011, // 2
    0b01001111, // 3
    0b00100111, // 4
    0b01101101, // 5
    0b01111101, // 6
    0b01000110, // 7
		0b01111111, // 8
		0b01101111, // 9
		0b00000000, // empty
};
	
const uint8_t segment_to_segment[] = {
    0b01000000, // 0
		0b00100000, // 0
		0b00010000, // 0
		0b00001000, // 0
		0b00000100, // 0
		0b00000010, // 0
		0b00000001, // 0
};
//====================================================================================================
int main(void){
	RCC_Configure();
	GPIO_Configure();
	//USART_Configure();
	//I2C_Configure();
	SPI_Configure();
	
//	GPIO_Pin_High(GPIOA, GPIO_PIN_4);
//	GPIO_Pin_Low(GPIOA, GPIO_PIN_4);
	
	uint8_t data = 0x00;
//	GPIO_Pin_High(GPIOA, GPIO_PIN_4);
	//====================================================================================================
	GPIO_Pin_Low(GPIOA, GPIO_PIN_4);
	data = 0x0F;
	SPI_Transmit(SPI1, &data, 1);
	data = 0x00;
	SPI_Transmit(SPI1, &data, 1);
	data = 0x0F;
	SPI_Transmit(SPI1, &data, 1);
	data = 0x00;
	SPI_Transmit(SPI1, &data, 1);
	GPIO_Pin_High(GPIOA, GPIO_PIN_4);
	//====================================================================================================
	GPIO_Pin_Low(GPIOA, GPIO_PIN_4);
	data = 0x09;
	SPI_Transmit(SPI1, &data, 1);
	data = 0x00;
	SPI_Transmit(SPI1, &data, 1);
	data = 0x09;
	SPI_Transmit(SPI1, &data, 1);
	data = 0x00;
	SPI_Transmit(SPI1, &data, 1);
	GPIO_Pin_High(GPIOA, GPIO_PIN_4);
	//====================================================================================================
	GPIO_Pin_Low(GPIOA, GPIO_PIN_4);
	data = 0x0A;
	SPI_Transmit(SPI1, &data, 1);
	data = 3;
	SPI_Transmit(SPI1, &data, 1);
	data = 0x0A;
	SPI_Transmit(SPI1, &data, 1);
	data = 3;
	SPI_Transmit(SPI1, &data, 1);
	GPIO_Pin_High(GPIOA, GPIO_PIN_4);
	//====================================================================================================
	GPIO_Pin_Low(GPIOA, GPIO_PIN_4);
	data = 0x0B;
	SPI_Transmit(SPI1, &data, 1);
	data = 5;
	SPI_Transmit(SPI1, &data, 1);
	data = 0x0B;
	SPI_Transmit(SPI1, &data, 1);
	data = 5;
	SPI_Transmit(SPI1, &data, 1);
	GPIO_Pin_High(GPIOA, GPIO_PIN_4);
	//====================================================================================================
	GPIO_Pin_Low(GPIOA, GPIO_PIN_4);
	data = 0x0C;
	SPI_Transmit(SPI1, &data, 1);
	data = 0x01;
	SPI_Transmit(SPI1, &data, 1);
	
	data = 0x0C;
	SPI_Transmit(SPI1, &data, 1);
	data = 0x01;
	SPI_Transmit(SPI1, &data, 1);
	GPIO_Pin_High(GPIOA, GPIO_PIN_4);
	//====================================================================================================
	delay_ms(100);
	
	uint8_t i=6;
	do
	{
		GPIO_Pin_Low(GPIOA, GPIO_PIN_4);
		data = i;
		SPI_Transmit(SPI1, &data, 1);
		data = digit_to_segment[10];
		SPI_Transmit(SPI1, &data, 1);
		data = i;
		SPI_Transmit(SPI1, &data, 1);
		data = digit_to_segment[10];
		SPI_Transmit(SPI1, &data, 1);
		GPIO_Pin_High(GPIOA, GPIO_PIN_4);
		delay_ms(300);
	} while (--i);
	
	delay_ms(300);
	
	GPIO_Pin_Low(GPIOA, GPIO_PIN_4);
	data = 1;
	SPI_Transmit(SPI1, &data, 1);
	data = digit_to_segment[4];
	SPI_Transmit(SPI1, &data, 1);
	data = 1;
	SPI_Transmit(SPI1, &data, 1);
	data = digit_to_segment[1];
	SPI_Transmit(SPI1, &data, 1);
	
	data = 1;
	SPI_Transmit(SPI1, &data, 1);
	data = digit_to_segment[3];
	SPI_Transmit(SPI1, &data, 1);
	data = 1;
	SPI_Transmit(SPI1, &data, 1);
	data = digit_to_segment[1];
	SPI_Transmit(SPI1, &data, 1);
	GPIO_Pin_High(GPIOA, GPIO_PIN_4);
	
	GPIO_Pin_Low(GPIOA, GPIO_PIN_4);
	data = 2;
	SPI_Transmit(SPI1, &data, 1);
	data = digit_to_segment[0];
	SPI_Transmit(SPI1, &data, 1);
	data = 2;
	SPI_Transmit(SPI1, &data, 1);
	data = digit_to_segment[9];
	SPI_Transmit(SPI1, &data, 1);
	GPIO_Pin_High(GPIOA, GPIO_PIN_4);
	
	GPIO_Pin_Low(GPIOA, GPIO_PIN_4);
	data = 3;
	SPI_Transmit(SPI1, &data, 1);
	data = digit_to_segment[7];
	SPI_Transmit(SPI1, &data, 1);
	data = 3;
	SPI_Transmit(SPI1, &data, 1);
	data = digit_to_segment[0];
	SPI_Transmit(SPI1, &data, 1);
	GPIO_Pin_High(GPIOA, GPIO_PIN_4);
	
	GPIO_Pin_Low(GPIOA, GPIO_PIN_4);
	data = 4;
	SPI_Transmit(SPI1, &data, 1);
	data = digit_to_segment[7];
	SPI_Transmit(SPI1, &data, 1);
	data = 4;
	SPI_Transmit(SPI1, &data, 1);
	data = digit_to_segment[0];
	SPI_Transmit(SPI1, &data, 1);
	GPIO_Pin_High(GPIOA, GPIO_PIN_4);
	
	GPIO_Pin_Low(GPIOA, GPIO_PIN_4);
	data = 5;
	SPI_Transmit(SPI1, &data, 1);
	data = digit_to_segment[9];
	SPI_Transmit(SPI1, &data, 1);
	data = 5;
	SPI_Transmit(SPI1, &data, 1);
	data = digit_to_segment[2];
	SPI_Transmit(SPI1, &data, 1);
	GPIO_Pin_High(GPIOA, GPIO_PIN_4);
	
	GPIO_Pin_Low(GPIOA, GPIO_PIN_4);
	data = 6;
	SPI_Transmit(SPI1, &data, 1);
	data = digit_to_segment[0];
	SPI_Transmit(SPI1, &data, 1);
	data = 6;
	SPI_Transmit(SPI1, &data, 1);
	data = digit_to_segment[9];
	SPI_Transmit(SPI1, &data, 1);
	GPIO_Pin_High(GPIOA, GPIO_PIN_4);
	
	while(1){
		for(uint8_t y = 1; y <= 6; y++){
			for(uint8_t i = 0; i < 10; i++){

//				if(1){
//					GPIO_Pin_Low(GPIOA, GPIO_PIN_4);
//					data = 0x0C;
//					SPI_Transmit(SPI1, &data, 1);
//					data = 0x00;
//					SPI_Transmit(SPI1, &data, 1);
//					
//					data = 0x0C;
//					SPI_Transmit(SPI1, &data, 1);
//					data = 0x00;
//					SPI_Transmit(SPI1, &data, 1);
//					GPIO_Pin_High(GPIOA, GPIO_PIN_4);
//				}
			}
		}
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
	
	GPIO_InitStruct.Pin 			= GPIO_PIN_6 | GPIO_PIN_7;
	GPIO_InitStruct.Mode 			= MODE_AF;
	GPIO_InitStruct.Type 			= TYPE_PP;
	GPIO_InitStruct.Speed 		= GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate	= AF_USART1;
	
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin 			= GPIO_PIN_8 | GPIO_PIN_9;
	GPIO_InitStruct.Mode 			= MODE_AF;
	GPIO_InitStruct.Type 			= TYPE_OD;
	GPIO_InitStruct.Pull 			= PULL_DOWN;
	GPIO_InitStruct.Speed 		= GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate	= AF_I2C1;
	
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin 			= GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
	GPIO_InitStruct.Mode 			= MODE_AF;
	GPIO_InitStruct.Type 			= TYPE_PP;
	GPIO_InitStruct.Pull 			= PULL_UP;
	GPIO_InitStruct.Speed 		= GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate	= AF_SPI1;
	
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin 			= GPIO_PIN_4;									
	GPIO_InitStruct.Mode 			= MODE_OUTPUT;
	GPIO_InitStruct.Type 			= TYPE_PP;
	GPIO_InitStruct.Speed 		= GPIO_SPEED_FREQ_VERY_HIGH;
	
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin 			= GPIO_PIN_13;									
	GPIO_InitStruct.Mode 			= MODE_OUTPUT;
	GPIO_InitStruct.Type 			= TYPE_PP;
	GPIO_InitStruct.Speed 		= GPIO_SPEED_FREQ_VERY_HIGH;
	
	GPIO_Init(GPIOC, &GPIO_InitStruct);
}

void USART_Configure(void){
	HC_05.Baudrate = 115200;
	
	HC_05.Rx_Enable = true;
	HC_05.Tx_Enable = true;
	
	HC_05.Rx_IRq_Enable = true;
	
	USART_Init(USART1, &HC_05);
}

void SPI_Configure(void){
	SPI_Pixel_Matrix.Baudrate_Prescaler = SPI_BAUDRATE_DIV64;
	
	SPI_Init(SPI1, &SPI_Pixel_Matrix);
}

void TIM_Configure(void){
	TIM_InitTypeDef TIMx;
	
	TIM_Init(TIM3, &TIMx);
	
	TIM_Init(TIM2, &TIMx);
}

void I2C_Configure(void){
	I2C_BMP.PeripheralBusFrequency = 50000000;
	
	I2C_Init(I2C1, &I2C_BMP);
}
//====================================================================================================

