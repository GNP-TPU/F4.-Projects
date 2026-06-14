#include "ST77xx.h"
#include "SPI.h"

void ST77xx_WriteData(uint8_t data)
{
	uint8_t dummy_read = 0;
	ST77xx_DC_HIGH();
	ST77xx_CS_LOW();
	while(!(SPI1->SR & SPI_SR_TXE));
	SPI1->DR = data;
	while((SPI1->SR & SPI_SR_BSY));
	ST77xx_CS_HIGH();
}
void ST77xx_WriteCmd(uint8_t data)
{
	uint8_t dummy_read = 0;
	ST77xx_DC_LOW();
	ST77xx_CS_LOW();
	while(!(SPI1->SR & SPI_SR_TXE));
	SPI1->DR = data;
	while((SPI1->SR & SPI_SR_BSY));
	ST77xx_CS_HIGH();
}

void ST77xx_HardReset(void)
{
  ST77xx_RST_LOW();
	delay_ms(100);
	ST77xx_RST_HIGH();
	delay_ms(100);
}

void ST77xx_SleepMode(uint8_t Mode)
{
  if (Mode)
    ST77xx_WriteCmd(ST77xx_Cmd_SLPIN);
  else
    ST77xx_WriteCmd(ST77xx_Cmd_SLPOUT);
  
  delay_ms(100);
}

void ST77xx_ColorModeSet(uint8_t ColorMode)
{
  ST77xx_WriteCmd(ST77xx_Cmd_COLMOD);
  ST77xx_WriteData(ColorMode & 0x77);  
}

void ST77xx_MemAccessModeSet(uint8_t Rotation, uint8_t VertMirror, uint8_t HorizMirror, uint8_t IsBGR)
{
  uint8_t Value;
  Rotation &= 7; 

  ST77xx_WriteCmd(ST77xx_Cmd_MADCTL);
  
  // ????????? ??????????? ?????????? ??????
  switch (Rotation)
  {
  case 0:
    Value = 0;
    break;
  case 1:
    Value = ST77xx_MADCTL_MX;
    break;
  case 2:
    Value = ST77xx_MADCTL_MY;
    break;
  case 3:
    Value = ST77xx_MADCTL_MX | ST77xx_MADCTL_MY;
    break;
  case 4:
    Value = ST77xx_MADCTL_MV;
    break;
  case 5:
    Value = ST77xx_MADCTL_MV | ST77xx_MADCTL_MX;
		Value &= ~ST77xx_MADCTL_ML;
    break;
  case 6:
    Value = ST77xx_MADCTL_MV | ST77xx_MADCTL_MY;
    break;
	case 7:
    Value = ST77xx_MADCTL_MV | ST77xx_MADCTL_MX | ST77xx_MADCTL_ML;
    break;
  }
  
  if (VertMirror)
    Value = ST77xx_MADCTL_ML;
  if (HorizMirror)
    Value = ST77xx_MADCTL_MH;
  
  if (IsBGR)
    Value |= ST77xx_MADCTL_BGR;
  
  ST77xx_WriteData(Value);
}

void ST77xx_InversionMode(uint8_t Mode)
{
  if (Mode)
    ST77xx_WriteCmd(ST77xx_Cmd_INVON);
  else
    ST77xx_WriteCmd(ST77xx_Cmd_INVOFF);
}

void ST77xx_ColumnSet(uint16_t ColumnStart, uint16_t ColumnEnd)
{
  if (ColumnStart > ColumnEnd)
    return;
  if (ColumnEnd > ST77xx_Width)
    return;
  
  ColumnStart += ST77xx_X_Start;
  ColumnEnd += ST77xx_X_Start;
  
  ST77xx_WriteCmd(ST77xx_Cmd_CASET);
  ST77xx_WriteData(ColumnStart >> 8);  
  ST77xx_WriteData(ColumnStart & 0xFF);  
  ST77xx_WriteData(ColumnEnd >> 8);  
  ST77xx_WriteData(ColumnEnd & 0xFF);  
}

void ST77xx_RowSet(uint16_t RowStart, uint16_t RowEnd)
{
  if (RowStart > RowEnd)
    return;
  if (RowEnd > ST77xx_Height)
    return;
  
  RowStart += ST77xx_Y_Start;
  RowEnd += ST77xx_Y_Start;
  
  ST77xx_WriteCmd(ST77xx_Cmd_RASET);
  ST77xx_WriteData(RowStart >> 8);  
  ST77xx_WriteData(RowStart & 0xFF);  
  ST77xx_WriteData(RowEnd >> 8);  
  ST77xx_WriteData(RowEnd & 0xFF);  
}

void ST77xx_SetWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
  ST77xx_ColumnSet(x0, x1);
  ST77xx_RowSet(y0, y1);
  
  ST77xx_WriteCmd(ST77xx_Cmd_RAMWR);
}

void ST77xx_RamWrite(uint16_t pBuff, uint16_t Len)
{
  while (Len--)
  {
    ST77xx_WriteData(pBuff >> 8);  
    ST77xx_WriteData(pBuff & 0xFF);
  }  
}

void ST77xx_DrawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
  // Проверка на корректность размеров
  if (w <= 0 || h <= 0) return;

  // 1. Верхняя горизонтальная линия (ширина w, высота 1)
  ST77xx_FillRect(x, y, w, 1, color);
  
  // 2. Нижня горизонтальная линия (смещена вниз на h-1)
  ST77xx_FillRect(x, y + h - 1, w, 1, color);
  
  // 3. Левая вертикальная линия (ширина 1, высота h)
  ST77xx_FillRect(x, y, 1, h, color);
  
  // 4. Правая вертикальная линия (смещена вправо на w-1)
  ST77xx_FillRect(x + w - 1, y, 1, h, color);
}

void ST77xx_FillRectGrad(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
  if ((x >= ST77xx_Width) || (y >= ST77xx_Height))
    return;
  
  if ((x + w) > ST77xx_Width)
    w = ST77xx_Width - x;
  
  if ((y + h) > ST77xx_Height)
    h = ST77xx_Height - y;

  ST77xx_SetWindow(x, y, x + w - 1, y + h - 1);

  for (int i = 0; i < (h * w); i++)
    ST77xx_RamWrite(color | (i << 0 | i << 8 | i << 16), 1);
}

void ST77xx_FillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
  if ((x >= ST77xx_Width) || (y >= ST77xx_Height))
    return;
  
  if ((x + w) > ST77xx_Width)
    w = ST77xx_Width - x;
  
  if ((y + h) > ST77xx_Height)
    h = ST77xx_Height - y;

  ST77xx_SetWindow(x, y, x + w - 1, y + h - 1);

  for (int i = 0; i < (h * w); i++)
    ST77xx_RamWrite(color, 1);
}

void ST77xx_FillScreen(uint16_t color)
{
  ST77xx_FillRect(0, 0,  ST77xx_Width, ST77xx_Height, color);
}

void ST77xx_DisplayPower(uint8_t On)
{
  if (On)
    ST77xx_WriteCmd(ST77xx_Cmd_DISPON);
  else
    ST77xx_WriteCmd(ST77xx_Cmd_DISPOFF);
}

void ST77xx_Init(void)
{
  ST77xx_HardReset();
	delay_ms(100);
  ST77xx_SleepMode(0);
  delay_ms(100);
	ST77xx_SleepMode(0);
  delay_ms(100);
	
	ST77xx_WriteCmd(0xB2);
	ST77xx_WriteData(0x0C);
	ST77xx_WriteData(0x0C);
	ST77xx_WriteData(0x00);
	ST77xx_WriteData(0x33);
	ST77xx_WriteData(0x33);
	
	ST77xx_WriteCmd(0xB7);
	ST77xx_WriteData(0x35);
	
  ST77xx_ColorModeSet(ST7789_ColorMode_65K | ST7789_ColorMode_16bit);
  delay_ms(100);
  ST77xx_MemAccessModeSet(5, 0, 0, 0);
  delay_ms(100);
  ST77xx_InversionMode(0);
  delay_ms(100);
  ST77xx_DisplayPower(1);
  delay_ms(100);
	
	ST77xx_FillScreen(BLACK);
	delay_ms(500);
}

void ST77xx_DrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
  ST77xx_SetWindow(x, y, x, y);
  ST77xx_RamWrite(color, 1);
}

// Рисует один символ. x, y — координаты Базовой Линии (Baseline)
// Отрисовка одного символа Adafruit_GFX напрямую через пиксели экрана
void ST77xx_Draw_GFX_Char(char c, int16_t x, int16_t y, uint16_t text_color) {
    if (c < 32 || c > 126) return; // Защита от выхода за рамки таблицы ASCII
    
    uint16_t glyph_idx = c - 32;
    
    // Читаем параметры символа напрямую из вашей структуры FreeSans9pt7bGlyphs
    const GFXglyph *glyph = (const GFXglyph *)&FreeSans9pt7bGlyphs[glyph_idx];
    
    uint16_t bo = glyph->bitmapOffset;
    uint8_t  w  = glyph->width;
    uint8_t  h  = glyph->height;
    int8_t   xo = glyph->dX;
    int8_t   yo = glyph->dY;
    
    uint8_t  bits = 0;
    uint8_t  bit_mask = 0;

    // Цикл по матрице буквы
    for (uint8_t yy = 0; yy < h; yy++) {
        for (uint8_t xx = 0; xx < w; xx++) {
            
            // Если биты кончились, берем следующий байт из непрерывного массива битмапов
            if (bit_mask == 0) {
                bits = FreeSans9pt7bBitmaps[bo++];
                bit_mask = 0x80; // Сбрасываем на старший бит (MSB)
            }
            
            // Если бит равен 1, то физически рисуем этот пиксель на экране
            if (bits & bit_mask) {
                int16_t target_x = x + xo + xx;
                int16_t target_y = y + yo + yy;
                
                // Проверяем физические границы всего цветного дисплея (320x240)
                if (target_x >= 0 && target_x < 320 && target_y >= 0 && target_y < 240) {
                    // Рисуем попиксельно напрямую в дисплей через вашу функцию
                    ST77xx_DrawPixel(target_x, target_y, text_color);
                }
            }
            
            bit_mask >>= 1; // Шаг к следующему биту
        }
    }
}

// Автоматическое центрирование и вывод текста внутри ЛЮБОЙ ячейки на экране 4х3
// grid_x (0..3) — номер колонки кнопок
// grid_y (0..2) — номер строки кнопок
void ST77xx_Draw_GFX_String_Centered(const char* str, uint8_t grid_x, uint8_t grid_y, uint16_t text_color) {
    if (grid_x > 3 || grid_y > 2 || str == '\0' || str[0] == '\0') return;

    // Массив для хранения до 3 строк (максимум по 16 символов в каждой)
    char lines[3][16] = {0};
    uint8_t line_count = 0;

    // 1. ДИНАМИЧЕСКИЙ ПАРСИНГ: разбиваем текст на строки по ' ' или '_'
    const char *token_start = str;
    while (*token_start && line_count < 3) {
        // Пропускаем начальные разделители, если они есть
        while (*token_start == ' ' || *token_start == '_') {
            token_start++;
        }
        if (*token_start == '\0') break;

        // Ищем конец текущего слова
        const char *token_end = token_start;
        while (*token_end && *token_end != ' ' && *token_end != '_') {
            token_end++;
        }

        // Вычисляем длину слова
        uint32_t word_len = token_end - token_start;
        if (word_len > 15) word_len = 15; // Защита от переполнения буфера строки

        // Копируем слово в текущую строку
        strncpy(lines[line_count], token_start, word_len);
        lines[line_count][word_len] = '\0';
        line_count++;

        token_start = token_end;
    }

    // Если слово было одно, но оно гигантское (например, "CLIPBOARD" > 6 символов), пилим его аппаратно
    if (line_count == 1 && strlen(lines[0]) > 6) {
        char temp[16];
        strcpy(temp, lines[0]);
        uint8_t full_len = strlen(temp);
        uint8_t split = full_len / 2;

        strncpy(lines[0], temp, split);
        lines[0][split] = '\0';
        
        strcpy(lines[1], &temp[split]);
        line_count = 2;
    }

    // Координаты старта физической кнопки 80x80 на экране
    uint16_t button_start_x = grid_x * 80;
    uint16_t button_start_y = grid_y * 80;

    // 2. РАСЧЕТ ВЕРТИКАЛЬНЫХ БАЗОВЫХ ЛИНИЙ (Y) ДЛЯ КАЖДОЙ СТРОКИ
    int16_t y_coords[3] = {0};
    
    if (line_count == 1) {
        y_coords[0] = button_start_y + 45; // Идеальный центр для одной строки
    } 
    else if (line_count == 2) {
        y_coords[0] = button_start_y + 40 - 2; // Первая из двух
        y_coords[1] = button_start_y + 40 + 13 + 2; // Вторая из двух
    } 
    else if (line_count >= 3) {
        y_coords[0] = button_start_y + 24; // Первая из трех
        y_coords[1] = button_start_y + 45; // Вторая (ровно по центру кнопки)
        y_coords[2] = button_start_y + 66; // Третья из трех
        line_count = 3; // Страховка от мусора
    }

    // 3. ОТРИСОВКА ВСЕХ СФОРМИРОВАННЫХ СТРОК
    for (uint8_t l = 0; l < line_count; l++) {
        uint16_t w = 0;
        const char *p = lines[l];
        
        // Считаем точную физическую ширину текущей строки в пикселях
        while (*p) {
            char c = *p;
            if (c >= 32 && c <= 126) {
                const GFXglyph *glyph = (const GFXglyph *)&FreeSans9pt7bGlyphs[c - 32];
                w += (*(p + 1) == '\0') ? (glyph->dX + glyph->width) : glyph->xAdvance;
            }
            p++;
        }

        // Вычисляем стартовый X для текущей строки
        int16_t x = button_start_x + ((80 - w) / 2);
        
        // Компенсируем люфт dX первой буквы текущей строки
        const GFXglyph *first_glyph = (const GFXglyph *)&FreeSans9pt7bGlyphs[lines[l][0] - 32];
        
        p = lines[l];
        uint8_t is_first = 1;
        
        // Выводим буквы строки на экран
        while (*p) {
            if (is_first) {
                ST77xx_Draw_GFX_Char(*p, x - first_glyph->dX, y_coords[l], text_color);
                is_first = 0;
            } else {
                ST77xx_Draw_GFX_Char(*p, x, y_coords[l], text_color);
            }
            x += ((const GFXglyph *)&FreeSans9pt7bGlyphs[*p - 32])->xAdvance;
            p++;
        }
    }
}

void ST77xx_ScrollModeArea(uint16_t ColumnStart, uint16_t ColumnEnd, uint16_t RowStart, uint16_t RowEnd)
{
	ST77xx_WriteCmd(Cmd_VSCRDEF);
	ST77xx_RamWrite(0, 1);
	ST77xx_RamWrite(320, 1);
	ST77xx_RamWrite(0, 1);
	ST77xx_WriteCmd(ST77xx_Cmd_CASET);
  ST77xx_WriteData(ColumnStart >> 8);  
  ST77xx_WriteData(ColumnStart & 0xFF);  
  ST77xx_WriteData(ColumnEnd >> 8);  
  ST77xx_WriteData(ColumnEnd & 0xFF);  
	ST77xx_WriteCmd(ST77xx_Cmd_RASET);
  ST77xx_WriteData(RowStart >> 8);  
  ST77xx_WriteData(RowStart & 0xFF);  
  ST77xx_WriteData(RowEnd >> 8);  
  ST77xx_WriteData(RowEnd & 0xFF);  
	ST77xx_MemAccessModeSet(5, 0, 0, 0);
	ST77xx_WriteCmd(ST77xx_Cmd_RAMWR);
}

void ST77xx_Scroll(uint16_t Scroll_Height, uint16_t quantity)
{
	uint16_t i;
	
	for(i = 0; i < quantity; i++){
		ST77xx_WriteCmd(ST77xx_Cmd_VSCSAD);
		ST77xx_WriteData((Scroll_Height + i) >> 8);  
		ST77xx_WriteData((Scroll_Height + i) & 0xFF);
		delay_ms(100);
	}
}

void ST77xx_DrawLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{
	int16_t w = 6;
  if ((x >= ST77xx_Width) || (y >= ST77xx_Height))
    return;
  
  if ((x + w) > ST77xx_Width)
    w = ST77xx_Width - x;
  
  if ((y + h) > ST77xx_Height)
    h = ST77xx_Height - y;

  ST77xx_SetWindow(x, y, x + w - 1, y + h - 1);

  for (int i = 0; i < (h * w); i++)
    ST77xx_RamWrite(color, 1);
}

// x, y — координаты левого верхнего угла на экране, куда нужно вывести иконку
// icon_color — цвет самой иконки (например, 0xFFFF — белый)
// bg_color — цвет фона под иконкой (например, 0x0000 — черный)
void ST77xx_Draw_Mono_Icon(uint16_t x, uint16_t y, const uint8_t* bitmap, uint16_t icon_width, uint16_t icon_height, uint16_t icon_color, uint16_t bg_color)
{
	uint16_t bytes_per_row = (icon_width + 7) / 8;
  for (uint16_t row = 0; row < icon_height; row++) 
  {
		for (uint16_t col = 0; col < icon_width; col++) 
    {
			// Находим индекс байта в массиве, где спрятан текущий пиксель
      // row * 5 — это смещение целых строк, (col / 8) — смещение байта внутри строки
						
			uint32_t byte_idx = (row * bytes_per_row) + (col / 8);
            
      // Находим позицию бита внутри этого байта (от 0 до 7)
      uint8_t bit_idx = col % 8;
            
      // Извлекаем бит (в XBM формате первый пиксель — это младший бит LSB)
      uint8_t is_pixel_active = (bitmap[byte_idx] << bit_idx) & 0x80;
            
      // Вычисляем итоговые координаты пикселя на экране
      uint16_t pixel_x = x + col;
      uint16_t pixel_y = y + row;
            
      // Рисуем пиксель на экране через вашу функцию
      if (is_pixel_active) {
				ST77xx_DrawPixel(pixel_x, pixel_y, icon_color);
      } 
			else {
				ST77xx_DrawPixel(pixel_x, pixel_y, bg_color);
      }
    }
	}
}
//==============================================================================
