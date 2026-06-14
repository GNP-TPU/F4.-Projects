#include "Image_Convertion.h"

static void rgb565_to_rgb(uint16_t rgb565, uint8_t* r, uint8_t* g, uint8_t* b) {
    *r = (rgb565 >> 11) & 0x1F;  // 5 бит красного
    *g = (rgb565 >> 5) & 0x3F;   // 6 бит зеленого
    *b = rgb565 & 0x1F;          // 5 бит синего

    // Масштабирование до 8 бит
   // *r = (*r << 3) | (*r >> 2);
    //*g = (*g << 2) | (*g >> 4);
    //*b = (*b << 3) | (*b >> 2);
    // Масштабирование 5/6 бит → 8 бит (точнее, чем битовые сдвиги)
    *r = (*r * 255 + 15) / 31;  // 5 бит → 8 бит (оптимизированное округление)
    *g = (*g * 255 + 31) / 63;   // 6 бит → 8 бит
    *b = (*b * 255 + 15) / 31;   // 5 бит → 8 бит
}

//создание серего изображения
void rgb_to_grayscale(uint32_t* rgb, uint8_t* gray, int width, int height) {
    for (int i = 0; i < width * height / 2; i += 2) {
        uint8_t r, g, b;
			
				rgb565_to_rgb((uint16_t)rgb[i], &r, &g, &b);
				// gray[i] = (uint8_t)((r * 77 + g * 150 + b * 29) >> 8);  // Целочисленная оптимизация
        gray[i] = (uint8_t)((r + g + b) / 3);
			
        rgb565_to_rgb((uint16_t)((rgb[i] >> 16)), &r, &g, &b);
       // gray[i] = (uint8_t)((r * 77 + g * 150 + b * 29) >> 8);  // Целочисленная оптимизация
        gray[i + 1] = (uint8_t)((r + g + b) / 3);
    }
}

int apply_sobel(uint8_t* gray, int width, int height) {
    const int sobel_x[3][3] = {
    { -1, 0, 1 },
    { -2, 0, 2 },
    { -1, 0, 1 }
    };

    const int sobel_y[3][3] = {
        { -1, -2, -1 },
        {  0,  0,  0 },
        {  1,  2,  1 }
    };

    int sum_all = 0;

    for (int y = 1; y < height - 1; ++y) {
        for (int x = 1; x < width - 1; ++x) {
            int gx = 0, gy = 0;

            for (int ky = -1; ky <= 1; ++ky)
                for (int kx = -1; kx <= 1; ++kx) {
                    int pixel = gray[(y + ky) * width + (x + kx)];
                    gx += pixel * sobel_x[ky + 1][kx + 1];
                    gy += pixel * sobel_y[ky + 1][kx + 1];
                }

            int magnitude = sqrt(gx * gx + gy * gy);
            sum_all += magnitude;
        }
    }

    return sum_all;
}