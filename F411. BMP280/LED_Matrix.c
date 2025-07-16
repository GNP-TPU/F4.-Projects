#include "LED_Matrix.h"

uint16_t MAX_BRIGHTNESS = 100;
uint8_t Display_Buffer[6];

uint8_t Matrix_Pixels[LED_WIDTH][LED_HEIGHT];

void LED_InsertChar(char ch, uint8_t startX, uint8_t startY) {
	int index;

  if (ch >= 32 && ch <= 126) {
		// Английские символы (ASCII)
    index = (ch - 32) * 6;
  } 
	else {
		return; // Неподдерживаемый символ
  }

  for (int i = 0; i < 6; i++) {
		uint8_t row = Font6x6[index + i];
		for (int j = 0; j < 6; j++) {
			if ((row >> (5 - j)) & 0x01) {
				// Проверяем, чтобы координаты не выходили за границы экрана
				if (startX + j >= 0 && startX + j < LED_WIDTH && startY + i >= 0 && startY + i < LED_HEIGHT) {
					Matrix_Pixels[startX + j][startY + i] = 0x01; // Включаем пиксель
        }
      } 
			else {
				if (startX + j >= 0 && startX + j < LED_WIDTH && startY + i >= 0 && startY + i < LED_HEIGHT) {
					Matrix_Pixels[startX + j][startY + i] = 0x00; // Выключаем пиксель
        }
			}
		}
	}
}

// Функция для сдвига экрана влево
void shiftLeft() {
    for (int y = 0; y < 16; y++) {
        for (int x = 0; x < 15; x++) {
            Matrix_Pixels[y][x] = Matrix_Pixels[y][x + 1];
        }
        Matrix_Pixels[y][15] = 0; // Очистка крайнего правого столбца
    }
}

// Функция для отображения бегущей строки
void scrollingText(const char *text) {
    int textLength = strlen(text);
    int offset = 0; // Начальная позиция текста (за правой границей)

    while (1) {
        LED_Clear(); // Очищаем экран

        // Отрисовываем символы текста
        for (int i = 0; i < textLength; i++) {
            int charX = offset + i * 7; // Расстояние между символами: 6 пикселей + 1 пробел
            if (charX >= -6 && charX < 16) { // Проверяем, виден ли символ
                LED_InsertChar(text[i], charX, 5); // Отрисовываем символ (строка 5)
            }
        }

        // Обновляем экран (например, отправляем данные в LED-матрицу)
        LED_Display();

        // Сдвигаем текст влево
        offset--;
        if (offset + textLength * 7 <= 0) {
            offset = 16; // Начинаем заново
        }

        // Задержка для создания эффекта анимации
        // delay_ms(1); // Например, 100 мс
    }
}

void LED_Display(void) {
	uint8_t x, y;
	for (y = 0; y < LED_HEIGHT + 1; y++) {
		for(x = 0; x < LED_WIDTH; x++){
			if(Matrix_Pixels[x][y] > 0){
				Display_Buffer[x / 8] = Display_Buffer[x / 8] << 1;
				Display_Buffer[x / 8] |= Matrix_Pixels[x][y];  					
			}
			else{
				Display_Buffer[x / 8] = Display_Buffer[x / 8] << 1;
			}
		}
		
		GPIO_Pin_High(LEDARRAY_PORT, LEDARRAY_G); // G = HIGH
		
		Display_Buffer[0] = ~Display_Buffer[0];
		Display_Buffer[1] = ~Display_Buffer[1];
		Display_Buffer[2] = ~Display_Buffer[2];
		Display_Buffer[3] = ~Display_Buffer[3];
		Display_Buffer[4] = ~Display_Buffer[4];
		Display_Buffer[5] = ~Display_Buffer[5];
		
		Scan_Line(15 - y); // Выбираем строку
		if(y != 16){
			// Отправляем данные через SPI
			SPI_Transmit(SPI1, Display_Buffer, 6);
			while((SPI1->SR & SPI_SR_BSY));
			GPIO_Pin_High(LEDARRAY_PORT, LEDARRAY_LAT); // LAT = HIGH
			GPIO_Pin_Low(LEDARRAY_PORT, LEDARRAY_LAT); // LAT = LOW
		}
		
		else{
			memset(Display_Buffer, 0xFF, sizeof(Display_Buffer));
		
			// Отправляем данные через SPI
			SPI_Transmit(SPI1, Display_Buffer, 6);
			while((SPI1->SR & SPI_SR_BSY));
			GPIO_Pin_High(LEDARRAY_PORT, LEDARRAY_LAT); // LAT = HIGH
			GPIO_Pin_Low(LEDARRAY_PORT, LEDARRAY_LAT); // LAT = LOW
		}
		
						
		GPIO_Pin_Low(LEDARRAY_PORT, LEDARRAY_G); // G = LOW
		
		for(uint16_t Matrix_Delay = 0; Matrix_Delay < MAX_BRIGHTNESS; Matrix_Delay++){
				
		}
	}
}

void LED_Clear(void){
	memset(Matrix_Pixels, 0, sizeof(Matrix_Pixels));
}

void Scan_Line(unsigned char m) {
	switch (m) {
		case 0: 
			GPIO_Pin_Low(LEDARRAY_PORT, LEDARRAY_D | LEDARRAY_C | LEDARRAY_B | LEDARRAY_A); 
		break;
				
    case 1: 
			GPIO_Pin_Low(LEDARRAY_PORT, LEDARRAY_D | LEDARRAY_C | LEDARRAY_B); 
			GPIO_Pin_High(LEDARRAY_PORT, LEDARRAY_A); 
		break;
				
    case 2: 
			GPIO_Pin_Low(LEDARRAY_PORT, LEDARRAY_D | LEDARRAY_C | LEDARRAY_A); 
			GPIO_Pin_High(LEDARRAY_PORT, LEDARRAY_B); 
		break;
    case 3: 
			GPIO_Pin_Low(LEDARRAY_PORT, LEDARRAY_D | LEDARRAY_C); 
			GPIO_Pin_High(LEDARRAY_PORT, LEDARRAY_B | LEDARRAY_A); 
		break;
    case 4: 
			GPIO_Pin_Low(LEDARRAY_PORT, LEDARRAY_D | LEDARRAY_B | LEDARRAY_A); 
			GPIO_Pin_High(LEDARRAY_PORT, LEDARRAY_C); 
		break;
    case 5: 
			GPIO_Pin_Low(LEDARRAY_PORT, LEDARRAY_D | LEDARRAY_B); 
			GPIO_Pin_High(LEDARRAY_PORT, LEDARRAY_C | LEDARRAY_A); 
		break;
    case 6: 
			GPIO_Pin_Low(LEDARRAY_PORT, LEDARRAY_D | LEDARRAY_A); 
			GPIO_Pin_High(LEDARRAY_PORT, LEDARRAY_C | LEDARRAY_B); 
		break;
    case 7: 
			GPIO_Pin_Low(LEDARRAY_PORT, LEDARRAY_D); 
			GPIO_Pin_High(LEDARRAY_PORT, LEDARRAY_C | LEDARRAY_B | LEDARRAY_A); 
		break;
    case 8: 
			GPIO_Pin_Low(LEDARRAY_PORT, LEDARRAY_C | LEDARRAY_B | LEDARRAY_A); 
			GPIO_Pin_High(LEDARRAY_PORT, LEDARRAY_D); 
		break;
    case 9: 
			GPIO_Pin_Low(LEDARRAY_PORT, LEDARRAY_C | LEDARRAY_B); 
			GPIO_Pin_High(LEDARRAY_PORT, LEDARRAY_D | LEDARRAY_A); 
		break;
		case 10: 
			GPIO_Pin_Low(LEDARRAY_PORT, LEDARRAY_C | LEDARRAY_A); 
			GPIO_Pin_High(LEDARRAY_PORT, LEDARRAY_D | LEDARRAY_B); 
		break;
    case 11: 
			GPIO_Pin_Low(LEDARRAY_PORT, LEDARRAY_C); 
			GPIO_Pin_High(LEDARRAY_PORT, LEDARRAY_D | LEDARRAY_B | LEDARRAY_A); 
		break;
    case 12: 
			GPIO_Pin_Low(LEDARRAY_PORT, LEDARRAY_B | LEDARRAY_A); 
			GPIO_Pin_High(LEDARRAY_PORT, LEDARRAY_D | LEDARRAY_C); 
		break;
    case 13: 
			GPIO_Pin_Low(LEDARRAY_PORT, LEDARRAY_B); 
			GPIO_Pin_High(LEDARRAY_PORT, LEDARRAY_D | LEDARRAY_C | LEDARRAY_A); 
		break;
    case 14: 
			GPIO_Pin_Low(LEDARRAY_PORT, LEDARRAY_A); 
			GPIO_Pin_High(LEDARRAY_PORT, LEDARRAY_D | LEDARRAY_C | LEDARRAY_B); 
		break;
    case 15: 
			GPIO_Pin_High(LEDARRAY_PORT, LEDARRAY_D | LEDARRAY_C | LEDARRAY_B | LEDARRAY_A); 
		break;
    default: break;
	}
}