#ifndef XPT2046_H
#define XPT2046_H
//==============================================================================
#include <stm32f4xx.h>
#include <stdio.h>
#include <stdint.h>
//==============================================================================
#include "SPI.h"
//==============================================================================
#define XPT2046_CS_LOW()  (GPIOB->BSRR = GPIO_BSRR_BR_12)  // Сброс в 0
#define XPT2046_CS_HIGH() (GPIOB->BSRR = GPIO_BSRR_BS_12)  // Установка в 1

// Константы разрешения дисплея (портретный режим)
#define SCREEN_WIDTH      320
#define SCREEN_HEIGHT     240

// Усредненные заводские границы АЦП тачскрина
#define X_RAW_MIN         200
#define X_RAW_MAX         3900
#define Y_RAW_MIN         200
#define Y_RAW_MAX         3900



// Структура для хранения координат в пикселях
typedef struct {
    int16_t x;
    int16_t y;
} Touch_Point;



uint16_t XPT2046_GetRaw_Filtered(uint8_t cmd);
uint16_t XPT2046_GetRaw_Axis(uint8_t cmd);
void XPT2046_Convert_To_Pixels(uint16_t raw_x, uint16_t raw_y);
//==============================================================================
#endif 

/*XPT2046_H*/
