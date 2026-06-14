//====================================================================================================
#include "main.h"
#include "Interrupt_Handlers.h"
//====================================================================================================
uint8_t global_mouse_report[5]    = {0, 0, 0, 0};
uint8_t global_keyboard_report[9] = {0, 0, 0, 0, 0, 0, 0, 0};
//====================================================================================================
// Глобальные переменные для работы с энкодером
#define ENCODERS_COUNT   14

volatile uint16_t encoder_counter[ENCODERS_COUNT];
volatile uint8_t  encoder_direction[ENCODERS_COUNT];
volatile uint8_t  encoder_changed[ENCODERS_COUNT];
//====================================================================================================

#define BUTTONS_COUNT   14

// Массив портов, строго соответствующий вашему списку пинов
GPIO_TypeDef* const btn_ports[BUTTONS_COUNT] = {
    GPIOB, // 1. PB9
    GPIOB, // 2. PB8
    GPIOA, // 3. PA1
    GPIOA, // 4. PA0
    GPIOB, // 5. PB3 
    GPIOC, // 6. PC13
    GPIOC, // 7. PC15
    GPIOA, // 8. PA2
    GPIOA, // 9. PA15
    GPIOA, // 10. PA4
    GPIOC, // 11. PC14
    GPIOA, // 12. PA3
    GPIOB, // 13. Резерв
    GPIOB  // 14. Резерв 
};

// Массив номеров пинов для каждой кнопки
const uint8_t btn_pins[BUTTONS_COUNT] = {
    9,  // 1. PB9
    8,  // 2. PB8
    1,  // 3. PA1
    0,  // 4. PA0
    3,  // 5. PB3
    13, // 6. PC13
    15, // 7. PC15
    2,  // 8. PA2
    15, // 9. PA15
    4,  // 10. PA4
    14, // 11. PC14
    3,  // 12. PA3 
    1, 	// 13. Резерв
    14  // 14. Резерв
};

// Используем структуру интегрирующего фильтра (алгоритм Шмитта), которую мы обсуждали ранее
typedef struct {
    uint8_t filter;   // Счетчик фильтра (0...FILTER_MAX)
    uint8_t state;    // Текущее зафиксированное состояние кнопки (0 - отпущена, 1 - нажата)
		uint8_t last_state;    // Текущее зафиксированное состояние кнопки (0 - отпущена, 1 - нажата)
} Button_t;

Button_t buttons[BUTTONS_COUNT] = {0};

#define FILTER_MAX      4   // 30 мс для подтверждения изменения состояния (защита от дребезга)

uint8_t report_needs_clear = 0;
uint8_t usb_kb_is_active = 0;

uint8_t mod_keymap[BUTTONS_COUNT] = {
    KEY_MOD_LCTRL | KEY_MOD_LSHIFT,       
    KEY_MOD_LCTRL | KEY_MOD_LSHIFT,         
    0,
    0,     
    KEY_MOD_LCTRL,     												
    KEY_MOD_LCTRL,     												
    KEY_MOD_LCTRL,    												
    KEY_MOD_LCTRL | KEY_MOD_LSHIFT,        
    KEY_MOD_LCTRL,         								
    KEY_MOD_LCTRL,         								
    KEY_MOD_LCTRL,         								
    KEY_MOD_LCTRL,         								
    KEY_MOD_LCTRL,             												
    KEY_MOD_LCTRL              												
};

uint8_t keymap[BUTTONS_COUNT] = {
    KEY_LEFT,         
    KEY_RIGHT,       
    KEY_BACKSPACE, 
    KEY_DELETE,    
	
    KEY_Z,     
    KEY_Y,     
    KEY_F,			    
    KEY_V,         
	
    KEY_A,        
    KEY_X,        
    KEY_C,        
    KEY_V,         
	
    KEY_Z,             
    KEY_Y              
};

char* button_labels[12] = {
    // --- Строка 0 (Индексы 0, 1, 2, 3) ---
    "SELECT WORD <-",  
    "SELECT WORD ->",  
    "BACK SPACE",   
    "DELETE",  
    
    // --- Строка 1 (Индексы 4, 5, 6, 7) ---
    "UNDO",  
    "REDO",  
    "SEARCH",   
    "PASTE RAW",  // Ваш новый парсер сам перенесет RAW на вторую строчку!
    
    // --- Строка 2 (Индексы 8, 9, 10, 11) ---
    "SELECT ALL", // Автоматически разобьется на "SELECT" и "ALL"
    "CUT",  
    "COPY",   
    "PASTE"
};

void On_Button_End_Cycle(void) {
    // 1. Полностью очищаем текущий репорт нулями с помощью memset
    memset(global_keyboard_report, 0, 9);
    global_keyboard_report[0] = 1; // Устанавливаем Report ID = 1

    uint8_t key_slot = 3; // Свободный слот для обычных клавиш в репорте (начиная с 3-го байта)

    // 2. Проходим циклом по всем кнопкам и смотрим их отфильтрованный статус
    for (uint8_t i = 0; i < BUTTONS_COUNT; i++) {
        if (buttons[i].state == 1 && keymap[i] != 0) {
            
            // Если это модификатор (например, левый Ctrl), записываем его во 1-й байт данных
            if (mod_keymap[i] != 0) {
                global_keyboard_report[1] |= mod_keymap[i];
            } 
						// Если у кнопки задана обычная клавиша (или спец-клавиша вроде KEY_RIGHT, KEY_ENTER)
            if (keymap[i] != 0) {
                // Если кнопка сама по себе является одиночным модификатором (как ваша кнопка 3: PA1)
                // то её код (0x01) совпадет с маской модификатора. 
                // Чтобы не слать код модификатора в слоты для обычных букв, фильтруем его:
                if (keymap[i] >= KEY_LCTRL && keymap[i] <= KEY_RGUI) {
                    global_keyboard_report[1] |= (1 << (keymap[i] - KEY_LCTRL)); // На всякий случай дублируем в байт модификаторов
                } 
                else {
                    // Обычная клавиша (A, B, V, ENTER и т.д.) идет в свой слот
                    if (key_slot <= 8) {
                        global_keyboard_report[key_slot++] = keymap[i];
                    }
                }
            }
        }
				
    }

    // 3. Безопасная отправка в USB (только при изменении состояния)
    // Проверяем, нажат ли хоть один модификатор или обычная клавиша
    if (global_keyboard_report[1] != 0 || global_keyboard_report[3] != 0) {
        
        // Отправляем текущий аккорд зажатых клавиш в ПК
				
        USB_EP_Tx(1, global_keyboard_report, 9); 
        usb_kb_is_active = 1; // Запоминаем, что клавиатура сейчас активна
    } 
    else {
        // Если все кнопки были отпущены
        if (usb_kb_is_active == 1) {
            // Отправляем ОДИН пустой пакет (все нули), чтобы компьютер отпустил клавиши
            USB_EP_Tx(1, global_keyboard_report, 9); 
            
            // Сбрасываем флаг только если точка готова и пакет ушел. 
            // Если шина USB была занята, на следующем тике таймер попытается отправить нули еще раз.
            if (USB_EP_IsReady(1)) {
                usb_kb_is_active = 0; 
            }
        }
    }
}

void TIM5_IRQHandler(void) {
    // Проверяем, что прерывание вызвано обновлением счетчика (UIF)
    if (TIM5->SR & TIM_SR_UIF) {
        TIM5->SR = ~TIM_SR_UIF; // ОБЯЗАТЕЛЬНО сбрасываем флаг прерывания в CMSIS

        // Цикл сканирования физического состояния всех 14 кнопок
        for (uint8_t i = 0; i < BUTTONS_COUNT; i++) {
            
            // Считываем бит конкретного пина на его порту из карты.
            // Так как кнопки замыкают на GND, логический '0' в регистре IDR означает нажатие.
            uint8_t raw_pressed = ((btn_ports[i]->IDR & (1U << btn_pins[i])) == 0);

            // Алгоритм интегрирующего фильтра дребезга контактов
            if (raw_pressed) {
                // Если кнопка прижата, плавно наращиваем фильтр до максимума
                if (buttons[i].filter < FILTER_MAX) buttons[i].filter++;
            } 
						else {
                // Если кнопка отпущена, плавно уменьшаем фильтр до нуля
                if (buttons[i].filter > 0) buttons[i].filter--;
            }

            // Анализ и фиксация стабильного состояния в памяти
            if (buttons[i].state == 0 && buttons[i].filter == FILTER_MAX) {
                buttons[i].state = 1; // Кнопка гарантированно НАЖАТА (дребезг пройден)
            } 
            else if (buttons[i].state == 1 && buttons[i].filter == 0) {
                buttons[i].state = 0; // Кнопка гарантированно ОТПУЩЕНА (искрение прекратилось)
            }
        }

        // Вызываем функцию формирования и умной отправки репорта
        On_Button_End_Cycle();
    }
}

// Структура медиа-клавиш (1 байт ID + 2 байта данных = 3 байта)
uint8_t global_media_report[3] = {2, 0, 0};
// Маски для первого байта данных (global_media_report[1])

void TIM3_IRQHandler(void) {
    // Проверяем, что прерывание вызвано событием захвата на Канале 1
    if (TIM3->SR & TIM_SR_CC1IF) {
        // Сбрасываем флаг прерывания
        TIM3->SR &= ~TIM_SR_CC1IF;

        // Читаем текущие данные
        encoder_counter[0] = TIM3->CNT;
        encoder_direction[0] = (TIM3->CR1 & TIM_CR1_DIR) >> TIM_CR1_DIR_Pos;
        
        // Выставляем флаг для main, что данные изменились
        encoder_changed[0] = 1;
			
			if (USB_EP_IsReady(1) && encoder_changed[0]) {
								global_mouse_report[0] = 3; // Report ID
                global_mouse_report[1] = 0; // Кнопки
                global_mouse_report[2] = 0; // Координата X (+1)
                global_mouse_report[3] = 0; // Координата Y
                global_mouse_report[4] = 0; // Колёсико
									
				if(encoder_direction[0] == 0x01){
                // 1. НАЖАТИЕ: Выставляем бит Volume Up во втором байте данных
                global_mouse_report[4] = 1;   // Устанавливаем бит громкости во 2-м байте
				 }
				 else{
					 global_mouse_report[4] = -1;
				 }
				
				
                USB_EP_Tx(1, global_mouse_report, 5);
            }
			encoder_changed[0] = 0;
    }
    
    // На случай, если также произошло переполнение (опционально)
    if (TIM3->SR & TIM_SR_UIF) {
        TIM3->SR &= ~TIM_SR_UIF;
    }
}

void TIM4_IRQHandler(void) {
    // Проверяем, что прерывание вызвано событием захвата на Канале 1
    if (TIM4->SR & TIM_SR_CC1IF) {
        // Сбрасываем флаг прерывания
        TIM4->SR &= ~TIM_SR_CC1IF;

        // Читаем текущие данные
        encoder_counter[1] = TIM4->CNT;
        encoder_direction[1] = (TIM4->CR1 & TIM_CR1_DIR) >> TIM_CR1_DIR_Pos;
        
        // Выставляем флаг для main, что данные изменились
        encoder_changed[1] = 1;
			
			if (USB_EP_IsReady(1) && encoder_changed[1]) {
				global_media_report[1] = 0;               // На всякий случай очищаем 1-й байт данных
				global_media_report[2] = 0;
         if(encoder_direction[1] == 0x01){
                // 1. НАЖАТИЕ: Выставляем бит Volume Up во втором байте данных
                global_media_report[2] |= MEDIA_VOL_DOWN;   // Устанавливаем бит громкости во 2-м байте
				 }
				 else{
					 global_media_report[2] |= MEDIA_VOL_UP;
				 }
         // Отправляем пакет (размер 3 байта)
         USB_EP_Tx(1, global_media_report, 3);
			// 2. ОЖИДАНИЕ ЗАВЕРШЕНИЯ ПЕРЕДАЧИ
            // Ждем, пока аппаратный контроллер USB_OTG очистит бит DIEPCTL_EPENA.
            // На частоте 96 МГц это займет доли миллисекунды, пакет гарантированно уйдет.
            while (!USB_EP_IsReady(1));

            // 3. ФОРМИРУЕМ И ОТПРАВЛЯЕМ ОТПУСКАНИЕ (ОБНУЛЕНИЕ)
            global_media_report[0] = 2; // Передаем Report ID = 2
            global_media_report[1] = 0; // Полностью обнуляем данные
            global_media_report[2] = 0; 
            
            USB_EP_Tx(1, global_media_report, 3); // Отправляем пустой пакет (кнопка отпущена)
            
            // Снова коротко ждем, чтобы буфер EP2 освободился для следующего щелчка энкодера
            while (!USB_EP_IsReady(1));
        }
        
        // Исправлена опечатка в вашем коде: сбрасываем индекс [1], так как обрабатывали encoder_changed[1]
        encoder_changed[1] = 0; 
    }
    
    // На случай, если также произошло переполнение (опционально)
    if (TIM4->SR & TIM_SR_UIF) {
        TIM4->SR &= ~TIM_SR_UIF;
    }
}

void TIM1_BRK_TIM9_IRQHandler(void) {
    // Проверяем, что прерывание случилось по обновлению счетчика
    if (TIM9->SR & TIM_SR_UIF) {
        TIM9->SR = ~TIM_SR_UIF; // Очищаем флаг
			
			
				for (uint8_t i = 0; i < BUTTONS_COUNT; i++) {
					uint8_t col = i % 4; // Остаток от деления дает колонку
					uint8_t row = i / 4; // Целочисленное деление дает строку

					// Вычисляем левый верхний угол конкретной кнопки
					//uint16_t x = col * 80 + 1; // Смещение по X (0, 80, 160, 240)
					// uint16_t y = row * 80 + 1; // Смещение по Y (0, 80, 160)
            // Анализ и фиксация стабильного состояния в памяти
					if(i < 12){
            if ((buttons[i].state != buttons[i].last_state) && buttons[i].state == 1) {
							ST77xx_Draw_GFX_String_Centered(button_labels[i], col, row, ORANGE);
            }
						else if ((buttons[i].state != buttons[i].last_state) && buttons[i].state == 0){
							ST77xx_Draw_GFX_String_Centered(button_labels[i], col, row, WHITE);
						}
						buttons[i].last_state = buttons[i].state;
					}
					else{

					}
        }
    }
}

#define CMD_BUFFER_SIZE  64
char     cmd_string[CMD_BUFFER_SIZE];
uint32_t cmd_idx = 0;

void TIM1_UP_TIM10_IRQHandler(void){
	if (TIM10->SR & TIM_SR_UIF) {
			TIM10->SR = ~TIM_SR_UIF; // Очищаем флаг
		while (USB_VCP_Byte_Available() > 0) {
            char symbol = (char)USB_VCP_Read();

            if (symbol == '\n' || symbol == '\r') {
                if (cmd_idx > 0) {
                    cmd_string[cmd_idx] = '\0'; // Закрываем строку

                    // === ПАРСИНГ КОМАНДЫ ПРЯМО В ТАЙМЕРЕ ===
                    if (strcmp(cmd_string, "LED_ON") == 0) {
                        ST77xx_Draw_GFX_String_Centered(button_labels[0], 0, 0, ORANGE);
											USB_VCP_Write_String("ON_OK"); 
                    }
                    else if (strcmp(cmd_string, "LED_OFF") == 0) {
                        ST77xx_Draw_GFX_String_Centered(button_labels[0], 0, 0, WHITE);
											USB_VCP_Write_String("OFF_OK"); 
                    }

                    cmd_idx = 0; // Сброс для следующей команды
                }
            }
            else {
                if (cmd_idx < (CMD_BUFFER_SIZE - 1)) {
                    cmd_string[cmd_idx++] = symbol;
                }
            }
        }
	}
}


extern DMA_InitTypeDef SPI1_DMA;
extern uint16_t Display_Buffer[320*80];
uint16_t Current_Color = RED;
uint32_t Color_Switch = 1;

extern SPI_InitTypeDef 				SPI_ST7789;

void DMA2_Stream2_IRQHandler(void){
	if(DMA2->LISR & DMA_LISR_TCIF2){
		DMA2->LIFCR |= DMA_LIFCR_CTCIF2;
		// DMA2_Stream2->CR &= ~DMA_SxCR_EN;
	}
}
//====================================================================================================
void HardFault_Handler(void) {
    NVIC_SystemReset();
}
//====================================================================================================
