//====================================================================================================
#ifndef MAIN_H
#define MAIN_H
//====================================================================================================
#include <stm32f407xx.h>
#include <stm32f4xx.h>

#include <string.h>
#include <stdio.h>
#include <stdbool.h>
//====================================================================================================
#include "RCC.h"
#include "GPIO.h"
#include "UART.h"
#include "SPI.h"
#include "I2C.h"
#include "Flash.h"
#include "TIM.h"
#include "DCMI.h"

#include "OV7670.h"
#include "ILI9488.h"

#include "DMA.h"

#include "Interrupt_Handlers.h"

#include "Image_Convertion.h"
//====================================================================================================
#define IMG_HEIGHT	120
#define IMG_WIDTH		320

#define STEPPER_MOTOR_CCLKWISE 	GPIO_Pin_Low(GPIOD, GPIO_PIN_5);
#define STEPPER_MOTOR_CLKWISE 	GPIO_Pin_High(GPIOD, GPIO_PIN_5);

#define STOP_TIMER							TIM1->CR1 &= ~TIM_CR1_CEN;
#define START_TIMER							TIM1->CR1 |= TIM_CR1_CEN;
//====================================================================================================
#endif
//====================================================================================================
