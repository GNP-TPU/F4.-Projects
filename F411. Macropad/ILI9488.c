/*
 * LCD driver for the ILI9488 TFT LCD chips.
 *
 * Feel free to use, change, distribute this code as desired. Use under
 * GPLv3 open-source license.
 *
 * File:   ILI9488.c
 * Author: tommy
 *
 * Created on 29th March 2021
 */


#include "ILI9488.h"

/*
 * A little bit of video RAM to speed things up. This lets the HAL SPI library
 * transfer data in bulk rather than separate transactions.
 * The minimum value is 3 (1 byte each for R, G, and B).
 * The theoretical maximum is 0xFFFF - 1 but that doesn't seem to work. Pick a
 * size that suits your RAM budget and works with your controller.
 */
#define V_BUFFER_SIZE 0x3FFF
uint8_t v_buffer[V_BUFFER_SIZE];
uint16_t buffer_counter = 0;

/*
 * Writes a byte to SPI without changing chip select (CS) state.
 * Called by the write_command() and write_data() functions which
 * control these pins as required.
 */
void spi_write(uint8_t data)
{
	SPI_Transmit(SPI1, &data, 1);
}

/*
 * Writes the V-RAM buffer to the display.
 * TODO: currently only uses SPI.
 */
void write_buffer()
{
	SPI_Transmit(SPI1, v_buffer, buffer_counter);
	buffer_counter = 0;
}

/**
 * Writes data to an 8-bit parallel bus. This uses a (slow),
 * individual pin write mode just because the HAL libraries
 * don't offer a full port write.
 * If it suits your project you can use
 * GPIOx->ODR = data;
 * instead.
 */
void parallel_write(unsigned char data)
{
	//In this particular example I'm using PA5:PA12
    GPIO_Pin_Low(WR_PORT, WR_PIN);
	GPIOA->ODR = (data << 5);
	delay_ms(1);
    GPIO_Pin_High(WR_PORT, WR_PIN);
}

/*
 * Writes a data byte to the display. Pulls CS low as required.
 */
void lcd_write_data(uint8_t data)
{
   GPIO_Pin_High(DC_PORT, DC_PIN);
   GPIO_Pin_Low(CS_PORT, CS_PIN);

    	spi_write(data);

    //HAL_GPIO_WritePin(DC_PORT, DC_PIN, GPIO_PIN_SET);
    GPIO_Pin_High(CS_PORT, CS_PIN);
}
void lcd_write_data1(uint16_t data, unsigned int DataLen)
{
   GPIO_Pin_High(DC_PORT, DC_PIN);
   GPIO_Pin_Low(CS_PORT, CS_PIN);

   for (unsigned int i=0; i< DataLen; i++)
   {
	   uint8_t data1=data>>8;
	   uint8_t data2=data&0xFF;
	   SPI_Transmit(SPI1, &data1, 1);
	   SPI_Transmit(SPI1, &data2, 1);
   }


  // spi_write(data);

    //HAL_GPIO_WritePin(DC_PORT, DC_PIN, GPIO_PIN_SET);
    GPIO_Pin_High(CS_PORT, CS_PIN);
}

/*
 * Writes a command byte to the display
 */
void lcd_write_command(uint8_t data)
{
    GPIO_Pin_Low(DC_PORT, DC_PIN);
    GPIO_Pin_Low(CS_PORT, CS_PIN);
    	spi_write(data);
    //HAL_GPIO_WritePin(DC_PORT, DC_PIN, GPIO_PIN_SET);
    GPIO_Pin_High(CS_PORT, CS_PIN);
}

void lcd_write_command1(uint8_t data)
{
    GPIO_Pin_Low(DC_PORT, DC_PIN);
    spi_write(data);

}

void lcd_write_reg(unsigned int data)
{
    GPIO_Pin_High(DC_PORT, DC_PIN);
    GPIO_Pin_Low(CS_PORT, CS_PIN);
    if(SPI_MODE)
    	spi_write(data);
	else
		parallel_write(data);
    GPIO_Pin_High(CS_PORT, CS_PIN);
}

/**
 * Read a number of bytes from teh display.
 */
/*void lcd_read_bytes(int byteCount)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	//Set the GPIO mode to INPUT
	GPIO_InitStruct.Pin = LCD0_Pin|LCD1_Pin|LCD2_Pin|LCD3_Pin
						  |LCD4_Pin|LCD5_Pin|LCD6_Pin|LCD7_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	//Read each byte
	while(byteCount--) {
		HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(RD_PORT, RD_PIN, GPIO_PIN_RESET);
		delay_ms(10);
		unsigned int data = GPIOA->IDR;
		data = (data >> 5) & 0xFF;
		HAL_GPIO_WritePin(RD_PORT, RD_PIN, GPIO_PIN_SET);
	}

    //Reset the GPIO mode to OUTPUT
    GPIO_InitStruct.Pin = LCD0_Pin|LCD1_Pin|LCD2_Pin|LCD3_Pin
                            |LCD4_Pin|LCD5_Pin|LCD6_Pin|LCD7_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}
*/
/*
 * Swaps two 16-bit integers
 */
void swap_int(unsigned int *num1, unsigned int *num2) {
    int temp = *num2;
    *num2 = *num1;
    *num1 = temp;
}

/*
 * Swaps two 8-bit integers
 */
void swap_char(char *num1, char *num2) {
    char temp = *num2;
    *num2 = *num1;
    *num1 = temp;
}

/*
 * Delay calcualted on 32MHz clock.
 * Does NOT adjust to clock setting
 */

/*
 * Initialisation routine for the LCD
 * I got this from the one of the ebay sellers which make them.
 * From Open-Smart
 */
void lcd_init_parallel()
{

    //SET control pins for the LCD HIGH (they are active LOW)
    GPIO_Pin_High(RESX_PORT, RESX_PIN); //RESET pin HIGH (Active LOW)
    GPIO_Pin_High(CS_PORT, CS_PIN); //Chip Select Active LOW
    GPIO_Pin_High(DC_PORT, DC_PIN); //Data / Command select Active LOW
    GPIO_Pin_High(RD_PORT, RD_PIN); //READ pin HIGH (active LOW)
    GPIO_Pin_High(WR_PORT, WR_PIN); //WRITE pin HIGH (active LOW)
    //Cycle reset pin
    delay_ms(100);
    GPIO_Pin_Low(RESX_PORT, RESX_PIN);
    delay_ms(100);
    GPIO_Pin_High(RESX_PORT, RESX_PIN);
    delay_ms(100);


    lcd_init_command_list();

}


/*
 * Same as above, but initialises with an SPI port instead.
 */
void lcd_init_spi()
{
    //SET control pins for the LCD HIGH (they are active LOW)
    GPIO_Pin_High(RESX_PORT, RESX_PIN); //RESET pin HIGH (Active LOW)
   // HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_RESET); //Chip Select Active LOW
    //HAL_GPIO_WritePin(DC_PORT, DC_PIN, GPIO_PIN_SET); //Data / Command select Active LOW

    //Cycle reset pin
    delay_ms(100);
    GPIO_Pin_Low(RESX_PORT, RESX_PIN);
    delay_ms(500);
    GPIO_Pin_High(RESX_PORT, RESX_PIN);
    delay_ms(500);



    lcd_init_command_list();

    lcd_write_command(0x11);
    delay_ms(200);

    lcd_write_command(0x29);

}

/**
 * This is the magic initialisation routine.
 */
void lcd_init_command_list(void)
{

	//********Start Initial Sequence*******//


	lcd_write_command(0xF9);
	lcd_write_data(0x00);
	lcd_write_data(0x08);

	lcd_write_command(0xC0);
	lcd_write_data(0x19);
	lcd_write_data(0x1A);

	lcd_write_command(0xC1);
	lcd_write_data(0x45);
	lcd_write_data(0x00);

	lcd_write_command(0xC2);
	lcd_write_data(0x33);

	lcd_write_command(0xC5);
	lcd_write_data(0x00);
	lcd_write_data(0x28);

	lcd_write_command(0x36);
	lcd_write_data(0x38);

	lcd_write_command(0xB1);
	lcd_write_data(0xA0);
	lcd_write_data(0x11);

	lcd_write_command(0xB4);
	lcd_write_data(0x02);

	lcd_write_command(0xB6);
	lcd_write_data(0x00);
	lcd_write_data(0x42);
	lcd_write_data(0x3B);

	lcd_write_command(0xB7);
	lcd_write_data(0x07);

	lcd_write_command(0xe0); //P-Gamma
	lcd_write_data(0x1f);
	lcd_write_data(0x25);
	lcd_write_data(0x22);
	lcd_write_data(0x0b);
	lcd_write_data(0x06);
	lcd_write_data(0x0a);
	lcd_write_data(0x4e);
	lcd_write_data(0xc6);
	lcd_write_data(0x39);
	lcd_write_data(0x00);
	lcd_write_data(0x00);
	lcd_write_data(0x00);
	lcd_write_data(0x00);
	lcd_write_data(0x00);
	lcd_write_data(0x00);


	lcd_write_command(0xE1); //N-Gamma
	lcd_write_data(0x1f);
	lcd_write_data(0x3f);
	lcd_write_data(0x3f);
	lcd_write_data(0x0f);
	lcd_write_data(0x1f);
	lcd_write_data(0x0f);
	lcd_write_data(0x46);
	lcd_write_data(0x49);
	lcd_write_data(0x31);
	lcd_write_data(0x05);
	lcd_write_data(0x09);
	lcd_write_data(0x03);
	lcd_write_data(0x1c);
	lcd_write_data(0x1a);
	lcd_write_data(0x00);

	lcd_write_command(0xF1); //N-Gamma
	lcd_write_data(0x36);
	lcd_write_data(0x04);
	lcd_write_data(0x00);
	lcd_write_data(0x3c);
	lcd_write_data(0x0f);
	lcd_write_data(0x0f);
	lcd_write_data(0xa4);
	lcd_write_data(0x02);


	lcd_write_command(0xF2); //N-Gamma
	lcd_write_data(0x18);
	lcd_write_data(0xa3);
	lcd_write_data(0x12);
	lcd_write_data(0x02);
	lcd_write_data(0x32);
	lcd_write_data(0x12);
	lcd_write_data(0xff);
	lcd_write_data(0x32);
	lcd_write_data(0x00);

	lcd_write_command(0xF4); //N-Gamma
	lcd_write_data(0x40);
	lcd_write_data(0x00);
	lcd_write_data(0x08);
	lcd_write_data(0x91);
	lcd_write_data(0x04);

	lcd_write_command(0xF8); //N-Gamma
	lcd_write_data(0x21);
	lcd_write_data(0x04);

	lcd_write_command(0x3a);
	lcd_write_data(0x55);

}


/*
 * Sets the X,Y position for following commands on the display.
 * Should only be called within a function that draws something
 * to the display.
 */
void set_draw_window(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2)
{

    //Check that the values are in order
    if(x2 < x1)
        swap_int(&x2, &x1);
    if(y2 < y1)
        swap_int(&y2, &y1);

    lcd_write_command(ILI9488_CASET);
    lcd_write_data(x1 >> 8);
    lcd_write_data(x1 & 0xFF);

    lcd_write_data(x2 >> 8);
    lcd_write_data(x2 & 0xFF);

    lcd_write_command(ILI9488_PASET);
    lcd_write_data(y1 >> 8);
    lcd_write_data(y1 & 0xFF);

    lcd_write_data(y2 >> 8);
    lcd_write_data(y2 & 0xFF);

    lcd_write_command(ILI9488_RAMWR);
}

/*
 * Draws a single pixel to the LCD at position X, Y, with
 * Colour.
 *
 * 28 bytes per pixel. Use it wisely.
 */
void draw_pixel(unsigned int x, unsigned int y, uint16_t colour)
{
	    //Set the x, y position that we want to write to
	set_draw_window(x, y, x + 1, y + 1);

	GPIO_Pin_High(DC_PORT, DC_PIN);
	GPIO_Pin_Low(CS_PORT, CS_PIN);

	for (uint32_t i = 0; i < 150; i++) {
		lcd_write_data1(colour, 1);
	}

	GPIO_Pin_High(CS_PORT, CS_PIN);

}

/*
 * Fills a rectangle with a given colour
 */
void fill_rectangle(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, uint16_t colour)
{
    //All my colours are in 16-bit RGB 5-6-5 so they have to be converted to 18-bit RGB

    //Set the drawing region
    set_draw_window(x1, y1, x2, y2);

    //CS low to begin data
    GPIO_Pin_High(DC_PORT, DC_PIN);
    GPIO_Pin_Low(CS_PORT, CS_PIN);

    lcd_write_data1(colour, 1);

    //We will do the SPI write manually here for speed
    //( the data sheet says it doesn't matter if CS changes between
    // data sections but I don't trust it.)


    //Return CS to high
    GPIO_Pin_High(CS_PORT, CS_PIN);
}

/*
 * Draws a single char to the screen.
 * Called by the various string writing functions like print().
 *
 * NOTE:
 * This sends approx. 800 bytes per char to the LCD, but it does preserver
 * the background image. Use the draw_fast_char() function where possible.
 */

/*
 * Draws a char to the screen using a constant stream of pixel data whic his faster
 * than drawing individual pixels.
 * This will draw over any background image though.
 *
 * NOTE: This sends 130 bytes for a regular sized char
 */

/*
 * Writes a string to the display as an array of chars at position x, y with
 * a given colour and size.
 */

/*
 * Draws a string using the draw_fast_char() function.
 * This will not preserve any background image and so a custom background
 * colour should be provided.
 * NOTE: Can only be the regular sized font. No scaling.
 */
/*
 * Draws a bitmap by directly writing the byte stream to the LCD.
 *
 * So the scaling is done strangely here because writing individual pixels
 * has an overhead of 26 bytes each.
 */
void draw_bitmap(unsigned int x1, unsigned int y1,  const unsigned int *bmp)
{
	int width = bmp[0];
	int height = bmp[1];
	unsigned int this_byte;
	int x2 = x1 + (width);
	int y2 = y1 + (height);

	//Set the drawing region
	set_draw_window(x1, y1, x2, y2);

	//We will do the SPI write manually here for speed
    GPIO_Pin_High(DC_PORT, DC_PIN);
	//CS low to begin data
    GPIO_Pin_Low(CS_PORT, CS_PIN);

	//Write colour to each pixel
	for (int i = 0; i < height; i++) {
		//this loop does the vertical axis scaling (two of each line))
		//for (int sv = 0; sv < scale; sv++) {
			for (int j = 0; j < width; j++) {
				//Choose which byte to display depending on the screen orientation
				//NOTE: We add 2 bytes because of the first two bytes being dimension data in the array
				this_byte = bmp[(width * (i)) + j + 2];


			     lcd_write_data1(this_byte, 1);

		}
	}
	//Send the remaining bytes
	//write_buffer();

	//Return CS to high
    GPIO_Pin_High(CS_PORT, CS_PIN);

}

void draw_image(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t* data)
{
	set_draw_window(x, y, x + width - 1, y + height - 1);

	GPIO_Pin_High(DC_PORT, DC_PIN);
	GPIO_Pin_Low(CS_PORT, CS_PIN);

	for (uint32_t i = 0; i < width * height; i++) {
		lcd_write_data1(data[i], 1);
	}

	GPIO_Pin_High(CS_PORT, CS_PIN);
}
