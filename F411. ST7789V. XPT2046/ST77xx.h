#ifndef ST7789_H
#define ST7789_H
//==============================================================================
#include <stm32f4xx.h>
#include <stdio.h>
#include <stdint.h>
//==============================================================================
#include "RCC.h"
//==============================================================================
#define ST77xx_CS_HIGH() 				GPIOB->ODR |= GPIO_ODR_OD0
#define ST77xx_CS_LOW() 				GPIOB->ODR &= ~GPIO_ODR_OD0

#define	ST77xx_DC_HIGH()				GPIOB->ODR |= GPIO_ODR_OD2
#define	ST77xx_DC_LOW()  				GPIOB->ODR &= ~GPIO_ODR_OD2

#define ST77xx_RST_HIGH() 			GPIOB->ODR |= GPIO_ODR_OD10
#define ST77xx_RST_LOW() 				GPIOB->ODR &= ~GPIO_ODR_OD10
//==============================================================================
#define ST77xx_Width   	320
#define ST77xx_Height 	240
#define ST77xx_X_Start  0
#define ST77xx_Y_Start  0
//==============================================================================
//#define ST7735_Cmd_NOP        0x00
#define ST77xx_Cmd_SWRESET      0x01
//#define ST7735_Cmd_RDDID      0x04
//#define ST7735_Cmd_RDDST      0x09
#define ST77xx_Cmd_SLPIN        0x10
#define ST77xx_Cmd_SLPOUT       0x11
#define ST77xx_Cmd_PTLON        0x12
#define ST77xx_Cmd_NORON        0x13
#define ST77xx_Cmd_INVOFF       0x20
#define ST77xx_Cmd_INVON        0x21
#define ST77xx_Cmd_GAMSET       0x26
#define ST77xx_Cmd_DISPOFF      0x28
#define ST77xx_Cmd_DISPON       0x29
#define ST77xx_Cmd_CASET        0x2A
#define ST77xx_Cmd_RASET        0x2B
#define ST77xx_Cmd_RAMWR        0x2C
//#define ST7735_Cmd_RAMRD        0x2E
#define ST77xx_Cmd_PTLAR        0x30
#define ST77xx_Cmd_COLMOD       0x3A
#define Cmd_VSCRDEF             0x33
#define ST77xx_Cmd_MADCTL       0x36    // Memory data access control 
#define ST77xx_Cmd_VSCSAD       0x37
#define ST7735_Cmd_FRMCTR1      0xB1    // Frame Rate Control in normal mode
#define ST7735_Cmd_FRMCTR2      0xB2    // Frame Rate Control in idle mode
#define ST7735_Cmd_FRMCTR3      0xB3    // Frame Rate Control in partial mode
#define ST7735_Cmd_INVCTR       0xB4
#define ST7735_Cmd_DISSET5      0xB6    // Display Function set 5 
#define ST7735_Cmd_PWCTR1       0xC0    // Power control 1 
#define ST7735_Cmd_PWCTR2       0xC1    // Power control 2 
#define ST7735_Cmd_PWCTR3       0xC2    // Power control 3 
#define ST7735_Cmd_PWCTR4       0xC3    // Power control 4 
#define ST7735_Cmd_PWCTR5       0xC4    // Power control 5 
#define ST7735_Cmd_VMCTR1       0xC5    // VCOM Control 1
//==============================================================================
#define ST7789_Cmd_MADCTL_MY    0x80
#define ST7789_Cmd_MADCTL_MX    0x40
#define ST7789_Cmd_MADCTL_MV    0x20
#define ST7789_Cmd_MADCTL_ML    0x10
#define ST7789_Cmd_MADCTL_RGB   0x00
//==============================================================================
#define ST7789_Cmd_RDID1        0xDA
#define ST7789_Cmd_RDID2        0xDB
#define ST7789_Cmd_RDID3        0xDC
#define ST7789_Cmd_RDID4        0xDD
//==============================================================================
#define ST7735_ColorMode_12bit  0x03
#define ST7735_ColorMode_16bit  0x05
#define ST7735_ColorMode_18bit  0x06
//==============================================================================
#define ST77xx_MADCTL_MY        0x80
#define ST77xx_MADCTL_MX        0x40
#define ST77xx_MADCTL_MV        0x20
#define ST77xx_MADCTL_ML        0x10
#define ST77xx_MADCTL_BGR       0x08
#define ST77xx_MADCTL_MH        0x04
//==============================================================================
#define ST7789_ColorMode_65K    0x50
#define ST7789_ColorMode_262K   0x60
#define ST7789_ColorMode_12bit  0x03
#define ST7789_ColorMode_16bit  0x05
#define ST7789_ColorMode_18bit  0x06
#define ST7789_ColorMode_16M    0x07
//==============================================================================
#define BLACK    0x0000
#define BLUE     0x001F
#define RED      0xF800
#define GREEN    0x07E0
#define CYAN     0x07FF
#define MAGENTA  0xF81F
#define YELLOW   0xFFE0 
#define WHITE    0xFFFF
//==============================================================================
void ST77xx_GPIO_Init(void);

void ST77xx_WriteData(uint8_t);
void ST77xx_WriteCmd(uint8_t);
void ST77xx_HardReset(void);
void ST77xx_SleepMode(uint8_t);
void ST77xx_ColorModeSet(uint8_t);
void ST77xx_MemAccessModeSet(uint8_t, uint8_t, uint8_t, uint8_t);
void ST77xx_InversionMode(uint8_t);
void ST77xx_FillRectGrad(int16_t, int16_t, int16_t, int16_t, uint16_t);
void ST77xx_FillRect(int16_t, int16_t, int16_t, int16_t, uint16_t);
void ST77xx_SetWindow(uint16_t, uint16_t, uint16_t, uint16_t);
void ST77xx_RamWrite(uint16_t, uint16_t);
void ST77xx_ColumnSet(uint16_t, uint16_t);
void ST77xx_RowSet(uint16_t, uint16_t);
void ST77xx_FillScreen(uint16_t);
void ST77xx_DisplayPower(uint8_t);
void ST77xx_Init(void);
void ST77xx_DrawPixel(uint16_t, uint16_t, uint16_t);
void ST77xx_WriteChar(char, uint16_t, uint16_t, uint16_t, uint16_t);
void ST77xx_WriteString(char*, uint16_t, uint16_t, uint16_t, uint16_t);
void ST77xx_ScrollModeArea(uint16_t, uint16_t, uint16_t, uint16_t);
void ST77xx_Scroll(uint16_t, uint16_t);
void ST77xx_DrawLine(int16_t, int16_t, int16_t, uint16_t);
void delay(long int);
//==============================================================================
#endif
