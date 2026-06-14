#include "XPT2046.h"

// Глобальная переменная с текущими координатами пикселей
volatile Touch_Point touch_pos;

uint16_t XPT2046_GetRaw_Filtered(uint8_t cmd) {
    #define NUM_SAMPLES 6
    uint16_t samples[NUM_SAMPLES];
    uint8_t tx_buf[3];
    uint8_t rx_buf[3];
    
    // 1. Собираем серию замеров
    for (int i = 0; i < NUM_SAMPLES; i++) {
        tx_buf[0] = cmd;
        tx_buf[1] = 0x00;
        tx_buf[2] = 0x00;

        XPT2046_CS_LOW();
        SPI_TransmitReceive(SPI2, tx_buf, rx_buf, 3);
        XPT2046_CS_HIGH();

        uint16_t raw = ((uint16_t)rx_buf[1] << 8) | rx_buf[2];
        samples[i] = (raw >> 3) & 0x0FFF;
    }

    // 2. Ищем минимальное и максимальное значения в серии, чтобы выбросить их
    uint16_t min_val = samples[0];
    uint16_t max_val = samples[0];
    uint32_t sum = 0;

    for (int i = 0; i < NUM_SAMPLES; i++) {
        if (samples[i] < min_val) min_val = samples[i];
        if (samples[i] > max_val) max_val = samples[i];
        sum += samples[i];
    }

    // 3. Вычитаем экстремумы и считаем среднее арифметическое оставшихся 4-х замеров
    sum -= min_val;
    sum -= max_val;
    
    return (uint16_t)(sum / (NUM_SAMPLES - 2));
}


uint16_t XPT2046_GetRaw_Axis(uint8_t cmd) {
    uint8_t tx_buf[3];
    uint8_t rx_buf[3];
    uint16_t raw_value = 0;

    // Подготовка буфера передачи
    tx_buf[0] = cmd;  // Команда (0x90 для Y, 0xD0 для X)
    tx_buf[1] = 0x00; // Пустышка
    tx_buf[2] = 0x00; // Пустышка

    // 1. Активируем тачскрин
    XPT2046_CS_LOW();

    // 2. Обмениваемся данными по SPI2 (3 байта)
    SPI_TransmitReceive(SPI2, tx_buf, rx_buf, 3);

    // 3. Деактивируем тачскрин
    XPT2046_CS_HIGH();

    // 4. Вытаскиваем 12-битный результат из приемного буфера.
    // XPT2046 выдает данные во время 2-го и 3-его байта SPI-сессии.
    raw_value = ((uint16_t)rx_buf[1] << 8) | rx_buf[2];
    raw_value >>= 3; // Сдвиг на 3 бита вправо, убираем пустые такты контроллера

    return (raw_value & 0x0FFF); // Возвращаем чистые 12 бит (0...4095)
}

void XPT2046_Convert_To_Pixels(uint16_t raw_x, uint16_t raw_y) {
        // 1. Ограничение сырых данных под рамки тачскрина
        // 1. Ограничение сырых данных под рамки тачскрина
    if (raw_x < X_RAW_MIN) raw_x = X_RAW_MIN;
    if (raw_x > X_RAW_MAX) raw_x = X_RAW_MAX;
    if (raw_y < Y_RAW_MIN) raw_y = Y_RAW_MIN;
    if (raw_y > Y_RAW_MAX) raw_y = Y_RAW_MAX;

    // 2. Рокировка осей: сырой Y пересчитываем в ширину (320), сырой X - в высоту (240)
    int32_t calc_x = ((int32_t)(raw_y - Y_RAW_MIN) * SCREEN_WIDTH) / (Y_RAW_MAX - Y_RAW_MIN);
    int32_t calc_y = ((int32_t)(raw_x - X_RAW_MIN) * SCREEN_HEIGHT) / (X_RAW_MAX - X_RAW_MIN);
	
    // 3. Исправление инверсии (на основе вашего поведения "влево -> вверх")
    calc_y = SCREEN_HEIGHT - calc_y; 

    // 4. Сохраняем результат в пикселях
    touch_pos.x = (int16_t)calc_x;
    touch_pos.y = (int16_t)calc_y;



    // --- НАСТРОЙКА ОРИЕНТАЦИИ (ЕСЛИ НАДО) ---
    // Если при тесте X или Y будут зеркальными, раскомментируйте нужную строку:
    // touch_pos.x = SCREEN_WIDTH - touch_pos.x;  // Инверсия X
    // touch_pos.y = SCREEN_HEIGHT - touch_pos.y; // Инверсия Y
}



