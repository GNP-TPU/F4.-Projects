#include "stm32f4xx.h"
#include <stdint.h>
#include <string.h>
#include <string.h>
// Управление самим USB-устройством (базовые регистры устройства)
#define USB_DEVICE           ((USB_OTG_DeviceTypeDef *)((uint32_t)USB_OTG_FS + USB_OTG_DEVICE_BASE))

// Для OUT конечных точек (для приема данных от ПК)
#define USB_GET_OUT_EP(epnum)  ((USB_OTG_OUTEndpointTypeDef *)((uint32_t)USB_OTG_FS + USB_OTG_OUT_ENDPOINT_BASE + ((epnum) * 0x20)))
#define EP0_OUT  ((USB_OTG_OUTEndpointTypeDef *)((uint32_t)USB_OTG_FS + USB_OTG_OUT_ENDPOINT_BASE + (0 * 0x20)))
#define EP1_OUT  ((USB_OTG_OUTEndpointTypeDef *)((uint32_t)USB_OTG_FS + USB_OTG_OUT_ENDPOINT_BASE + (1 * 0x20)))
#define EP2_OUT  ((USB_OTG_OUTEndpointTypeDef *)((uint32_t)USB_OTG_FS + USB_OTG_OUT_ENDPOINT_BASE + (2 * 0x20)))
#define EP3_OUT  ((USB_OTG_OUTEndpointTypeDef *)((uint32_t)USB_OTG_FS + USB_OTG_OUT_ENDPOINT_BASE + (3 * 0x20)))

// Для IN конечных точек (для отправки данных на ПК)
#define USB_GET_IN_EP(epnum)   ((USB_OTG_INEndpointTypeDef *)((uint32_t)USB_OTG_FS + USB_OTG_IN_ENDPOINT_BASE + ((epnum) * 0x20)))
#define EP0_IN    ((USB_OTG_INEndpointTypeDef *)((uint32_t)USB_OTG_FS + USB_OTG_IN_ENDPOINT_BASE + (0 * 0x20)))
#define EP1_IN    ((USB_OTG_INEndpointTypeDef *)((uint32_t)USB_OTG_FS + USB_OTG_IN_ENDPOINT_BASE + (1 * 0x20)))
#define EP2_IN    ((USB_OTG_INEndpointTypeDef *)((uint32_t)USB_OTG_FS + USB_OTG_IN_ENDPOINT_BASE + (2 * 0x20)))
#define EP3_IN    ((USB_OTG_INEndpointTypeDef *)((uint32_t)USB_OTG_FS + USB_OTG_IN_ENDPOINT_BASE + (3 * 0x20)))

// Для доступа к FIFO конкретной конечной точки (0, 1, 2...)
#define USB_GET_FIFO(epnum)  ((volatile uint32_t *)((uint32_t)USB_OTG_FS + USB_OTG_FIFO_BASE + ((epnum) * 0x1000)))
#define USB_FIFO0            USB_GET_FIFO(0)
//====================================================================================================
#define LOBYTE(x)  ((uint8_t)((x) & 0x00FFU))
#define HIBYTE(x)  ((uint8_t)((((x)) & 0xFF00U) >> 8U))
//====================================================================================================
/* Настройки идентификаторов USB */
#define USB_VID               0x0483
#define USB_PID               0x5740
#define USB_LANGID_STRING     0x0409
//====================================================================================================
/* Параметры конечных точек (Endpoints) */
#define EP0_MAX_PACKET_SIZE   64

#define EP1_IN_ADDR           0x81
#define EP1_MAX_PACKET_SIZE   4

#define EP2_IN_ADDR           0x82
#define EP2_MAX_PACKET_SIZE   9

#define CUSTOM_HID_FS_BINTERVAL 0x05 // Интервал опроса шины Full Speed
//====================================================================================================
typedef struct {
    uint8_t  bmRequestType;
    uint8_t  bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
} __attribute__((packed)) USB_SetupPacket_TypeDef;
//====================================================================================================
#define __ALIGN4 __attribute__((aligned(4)))
//====================================================================================================
// Маски модификаторов (USB HID Modifier Bits)
#define KEY_MOD_LCTRL      0x01
#define KEY_MOD_LSHIFT     0x02
#define KEY_MOD_LALT       0x04
#define KEY_MOD_LGUI       0x08  // Клавиша Windows / Command (левая)
#define KEY_MOD_RCTRL      0x10
#define KEY_MOD_RSHIFT     0x20
#define KEY_MOD_RALT       0x40  // AltGr
#define KEY_MOD_RGUI       0x80  // Клавиша Windows / Command (правая)

// Клавиши модификаторов
#define KEY_LCTRL   0xE0
#define KEY_LSHIFT  0xE1
#define KEY_LALT    0xE2
#define KEY_LGUI    0xE3
#define KEY_RCTRL   0xE4
#define KEY_RSHIFT  0xE5
#define KEY_RALT    0xE6
#define KEY_RGUI    0xE7
//====================================================================================================
// Специальные и системные клавиши
#define KEY_NONE           0x00  // Нет нажатой клавиши
#define KEY_ENTER          0x28  // Enter
#define KEY_ESCAPE         0x29  // Escape
#define KEY_BACKSPACE      0x2A  // Backspace
#define KEY_TAB            0x2B  // Tab
#define KEY_SPACE          0x2C  // Пробел
#define KEY_CAPSLOCK       0x39  // Caps Lock

// Буквенные клавиши (Алфавит A-Z)
#define KEY_A              0x04
#define KEY_B              0x05
#define KEY_C              0x06
#define KEY_D              0x07
#define KEY_E              0x08
#define KEY_F              0x09
#define KEY_G              0x0A
#define KEY_H              0x0B
#define KEY_I              0x0C
#define KEY_J              0x0D
#define KEY_K              0x0E
#define KEY_L              0x0F
#define KEY_M              0x10
#define KEY_N              0x11
#define KEY_O              0x12
#define KEY_P              0x13
#define KEY_Q              0x14
#define KEY_R              0x15
#define KEY_S              0x16
#define KEY_T              0x17
#define KEY_U              0x18
#define KEY_V              0x19
#define KEY_W              0x1A
#define KEY_X              0x1B
#define KEY_Y              0x1C
#define KEY_Z              0x1D

// Стрелки управления курсором
#define KEY_RIGHT          0x4F  // Стрелка Вправо
#define KEY_LEFT           0x50  // Стрелка Влево
#define KEY_DOWN           0x51  // Стрелка Вниз
#define KEY_UP             0x52  // Стрелка Вверх

#define KEY_PRINTSCREEN    0x46  // Клавиша Print Screen / SysRq
#define KEY_SCROLLLOCK     0x47  // Клавиша Scroll Lock
#define KEY_PAUSE          0x48  // Клавиша Pause / Break
#define KEY_INSERT         0x49  // Клавиша Insert
#define KEY_HOME           0x4A  // Клавиша Home
#define KEY_PAGEUP         0x4B  // Клавиша Page Up
#define KEY_DELETE         0x4C  // Клавиша Delete
#define KEY_END            0x4D  // Клавиша End
#define KEY_PAGEDOWN       0x4E  // Клавиша Page Down

#define KEY_1              0x1E  // 1 и !
#define KEY_2              0x1F  // 2 и @
#define KEY_3              0x20  // 3 и #
#define KEY_4              0x21  // 4 и $
#define KEY_5              0x22  // 5 и %
#define KEY_6              0x23  // 6 и ^
#define KEY_7              0x24  // 7 и &
#define KEY_8              0x25  // 8 и *
#define KEY_9              0x26  // 9 и (
#define KEY_0              0x27  // 0 и )

#define KEY_MINUS          0x2D  // Минус и Подчеркивание (- и _)
#define KEY_EQUAL          0x2E  // Равно и Плюс (= и +)
#define KEY_LEFTBRACE      0x2F  // Квадратная скобка [ (Х в рус. раскладке)
#define KEY_RIGHTBRACE     0x30  // Квадратная скобка ] (Ъ в рус. раскладке)
#define KEY_BACKSLASH      0x31  // Обратный слэш \ .
#define KEY_SEMICOLON      0x33  // Точка с запятой ; (Ж в рус. раскладке)
#define KEY_APOSTROPHE     0x34  // Апостроф ' (Э в рус. раскладке)
#define KEY_GRAVE          0x35  // Тильда / Ё `
#define KEY_COMMA          0x36  // Запятая , (Б в рус. раскладке)
#define KEY_DOT            0x37  // Точка . (Ю в рус. раскладке)
#define KEY_SLASH          0x38  // Слэш /

#define KEY_F1             0x3A
#define KEY_F2             0x3B
#define KEY_F3             0x3C
#define KEY_F4             0x3D
#define KEY_F5             0x3E
#define KEY_F6             0x3F
#define KEY_F7             0x40
#define KEY_F8             0x41
#define KEY_F9             0x42
#define KEY_F10            0x43
#define KEY_F11            0x44
#define KEY_F12            0x45

#define KEY_NUMLOCK        0x53  // Num Lock
#define KEY_KPSLASH        0x54  // Numpad /
#define KEY_KPASTERISK     0x55  // Numpad *
#define KEY_KPMINUS        0x56  // Numpad -
#define KEY_KPPLUS         0x57  // Numpad +
#define KEY_KPENTER        0x58  // Numpad Enter
#define KEY_KP1            0x59  // Numpad 1 (End)
#define KEY_KP2            0x5A  // Numpad 2 (Стрелка вниз)
#define KEY_KP3            0x5B  // Numpad 3 (Page Down)
#define KEY_KP4            0x5C  // Numpad 4 (Стрелка влево)
#define KEY_KP5            0x5D  // Numpad 5
#define KEY_KP6            0x5E  // Numpad 6 (Стрелка вправо)
#define KEY_KP7            0x5F  // Numpad 7 (Home)
#define KEY_KP8            0x60  // Numpad 8 (Стрелка вверх)
#define KEY_KP9            0x61  // Numpad 9 (Page Up)
#define KEY_KP0            0x62  // Numpad 0 (Insert)
#define KEY_KPDOT          0x63  // Numpad Точка (Delete)
#define KEY_KPEQUAL        0x67  // Numpad = (встречается на Mac)

#define KEY_CONTEXT_MENU   0x65  // Клавиша контекстного меню (обычно рядом с правым Ctrl)
#define KEY_F13            0x68  // Расширенные F-клавиши (их нет на физических клавиатурах,
#define KEY_F14            0x69  // но ОС их понимает. Идеально подходят для назначения
#define KEY_F15            0x6A  // глобальных горячих клавиш в OBS, Discord или играх,
#define KEY_F24            0x73  // чтобы не конфликтовать со стандартными кнопками)

#define MEDIA_LAUNCH_CALC        0x0192  // Калькулятор (AL Calculator)
#define MEDIA_LAUNCH_EMAIL       0x018A  // Почтовый клиент (AL Email Reader)
#define MEDIA_LAUNCH_BROWSER     0x0194  // Браузер / Домашняя страница (AL Internet Browser)
#define MEDIA_LAUNCH_MY_COMP     0x0194  // "Мой компьютер" / Проводник (AL Local Machine Browser)
#define MEDIA_LAUNCH_MEDIA_PLAY  0x0183  // Медиа-плеер (AL Consumer Control Configuration)

#define MEDIA_AC_HOME            0x0223  // Домой (в браузере)
#define MEDIA_AC_BACK            0x0224  // Назад (History Back)
#define MEDIA_AC_FORWARD         0x0225  // Вперед (History Forward)
#define MEDIA_AC_REFRESH         0x0221  // Обновить страницу (F5)
#define MEDIA_AC_BOOKMARKS       0x022A  // Избранное / Закладки
#define MEDIA_AC_SEARCH          0x0221  // Системный поиск / Поиск на странице

#define MEDIA_SYSTEM_SLEEP       0x0082  // Перевести ПК в спящий режим
#define MEDIA_SYSTEM_POWER       0x0081  // Выключение ПК (Power Down)
#define MEDIA_SYSTEM_WAKE        0x0083  // Пробуждение ПК (Wake Up)

#define MEDIA_NEXT_TRACK     0x01  // Бит 0 (Usage 0xB5)
#define MEDIA_PREV_TRACK     0x02  // Бит 1 (Usage 0xB6)
#define MEDIA_STOP           0x04  // Бит 2 (Usage 0xB7)
#define MEDIA_PLAY_PAUSE     0x08  // Бит 3 (Usage 0xCD) -> Для вашей кнопки PA15
#define MEDIA_MUTE           0x10  // Бит 4 (Usage 0xE2)

// Байт данных 2: Битовая маска для громкости и браузера
#define MEDIA_VOL_UP         0x01  // Бит 0 (Usage 0xE9) -> Для вашей кнопки PC15
#define MEDIA_VOL_DOWN       0x02  // Бит 1 (Usage 0xEA) -> Для вашей кнопки PA2
#define MEDIA_BROWSER_HOME   0x04 
/*========================================================================*/
/* КОНСТАНТЫ И АДРЕСА КОНЕЧНЫХ ТОЧЕК                                       */
/*========================================================================*/
#define EP1_IN_ADDR                 0x81U  // HID Combo (IN)
#define EP2_IN_ADDR                 0x82U  // CDC ACM Interrupt (IN)
#define EP3_IN_ADDR                 0x83U  // CDC ACM Bulk (IN)
#define EP3_OUT_ADDR                0x03U  // CDC ACM Bulk (OUT)

#define EP0_MAX_PACKET_SIZE         64U
#define EP1_MAX_PACKET_SIZE         64U
#define CDC_NOTIFICATION_MAX_SIZE   8U
#define CDC_DATA_MAX_SIZE           64U
#define CUSTOM_HID_FS_BINTERVAL     0x0AU

/* Переменные состояния */
static uint8_t usb_configuration = 0;
static volatile uint8_t usb_pending_address = 0;

/* Структура для хранения параметров COM-порта (9600 8-N-1) */
__ALIGN4 uint8_t cdc_line_coding[7] = { 0x80, 0x25, 0x00, 0x00, 0x00, 0x00, 0x08 };

/*========================================================================*/
/* 1. ОБЪЕДИНЕННЫЙ HID-ДЕСКРИПТОР РЕПОРТА (185 БАЙТ)                      */
/*========================================================================*/
__ALIGN4 const uint8_t Combined_HID_ReportDescriptor[] = {
    // REPORT ID 1: СТАНДАРТНАЯ КЛАВИАТУРА
    0x05, 0x01, 0x09, 0x06, 0xA1, 0x01, 0x85, 0x01,
    0x05, 0x07, 0x19, 0xE0, 0x29, 0xE7, 0x15, 0x00, 0x25, 0x01, 0x75, 0x01, 0x95, 0x08, 0x81, 0x02,
    0x95, 0x01, 0x75, 0x08, 0x81, 0x03,
    0x95, 0x05, 0x75, 0x01, 0x05, 0x08, 0x19, 0x01, 0x29, 0x05, 0x91, 0x02,
    0x95, 0x01, 0x75, 0x03, 0x91, 0x03,
    0x95, 0x06, 0x75, 0x08, 0x15, 0x00, 0x25, 0x65, 0x05, 0x07, 0x19, 0x00, 0x29, 0x65, 0x81, 0x00,
    0xC0,

    // REPORT ID 2: МЫШЬ
    0x05, 0x01, 0x09, 0x02, 0xA1, 0x01, 0x85, 0x02, 0x09, 0x01, 0xA1, 0x00,
    0x05, 0x09, 0x19, 0x01, 0x29, 0x03, 0x15, 0x00, 0x25, 0x01, 0x95, 0x03, 0x75, 0x01, 0x81, 0x02,
    0x95, 0x01, 0x75, 0x05, 0x81, 0x03,
    0x05, 0x01, 0x09, 0x30, 0x09, 0x31, 0x15, 0x81, 0x25, 0x7F, 0x75, 0x08, 0x95, 0x02, 0x81, 0x06,
    0x09, 0x38, 0x15, 0x81, 0x25, 0x7F, 0x75, 0x08, 0x95, 0x01, 0x81, 0x06,
    0xC0, 0xC0,

    // REPORT ID 3: МЕДИА-КЛАВИШИ
    0x05, 0x0C, 0x09, 0x01, 0xA1, 0x01, 0x85, 0x03,
    0x05, 0x0C, 0x15, 0x00, 0x25, 0x01, 0x75, 0x01, 0x95, 0x05,
    0x09, 0xB5, 0x09, 0xB6, 0x09, 0xB7, 0x09, 0xCD, 0x09, 0xE2, 0x81, 0x02,
    0x95, 0x01, 0x75, 0x03, 0x81, 0x03,
    0x95, 0x03, 0x75, 0x01, 0x09, 0xE9, 0x09, 0xEA, 0x0A, 0x23, 0x02, 0x81, 0x02,
    0x95, 0x01, 0x75, 0x05, 0x81, 0x03, 0xC0
};

/*========================================================================*/
/* 2. СТАНДАРТНЫЕ USB ДЕСКРИПТОРЫ УСТРОЙСТВА И КОНФИГУРАЦИИ                */
/*========================================================================*/
__ALIGN4 const uint8_t USB_DeviceDescriptor[] = {
    0x12, 0x01, 0x00, 0x02,
    0xEF, 0x02, 0x01,                       // Класс IAD устройства
    EP0_MAX_PACKET_SIZE,
    0x83, 0x04,                             // idVendor: ST (0x0483)
    0x50, 0x57,                             // idProduct: Новый чистый PID (0x5750)
    0x00, 0x02, 0x01, 0x02, 0x03, 0x01
};

__ALIGN4 const uint8_t USB_DeviceQualifierDescriptor[] = {
    0x0A, 0x06, 0x00, 0x02, 0x00, 0x00, 0x00, 64, 0x01, 0x00
};

__ALIGN4 const uint8_t USB_ConfigDescriptor[] = {
    /* Конфигурация (9 байт) */
    0x09, 0x02, 108, 0x00, 0x03, 0x01, 0x00, 0xC0, 0x32,

    /* IAD ДЛЯ HID ИНТЕРФЕЙСА (8 байт) */
    0x08, 0x0B, 0x00, 0x01, 0x03, 0x00, 0x00, 0x00, 

    /* Интерфейс 0: HID Combo (9 байт) */
    0x09, 0x04, 0x00, 0x00, 0x01, 0x03, 0x00, 0x00, 0x00, 
    /* HID дескриптор (9 байт) */
    0x09, 0x21, 0x11, 0x01, 0x00, 0x01, 0x22, 185, 0x00,  
    /* Эндпоинт EP1 IN (7 байт) */
    0x07, 0x05, EP1_IN_ADDR, 0x03, EP1_MAX_PACKET_SIZE, 0x00, CUSTOM_HID_FS_BINTERVAL,

    /* IAD ДЛЯ USB CDC COM-ПОРТА (8 байт) */
    0x08, 0x0B, 0x01, 0x02, 0x02, 0x02, 0x01, 0x00, 

    /* Интерфейс 1: CDC Управление (9 байт) */
    0x09, 0x04, 0x01, 0x00, 0x01, 0x02, 0x02, 0x01, 0x00, 
    /* CDC Функциональные дескрипторы */
    0x05, 0x24, 0x00, 0x10, 0x01, 
    0x05, 0x24, 0x01, 0x00, 0x02, 
    0x04, 0x24, 0x02, 0x02,       
    0x05, 0x24, 0x06, 0x01, 0x02, 
    /* Эндпоинт EP2 IN Interrupt (7 байт) */
    0x07, 0x05, EP2_IN_ADDR, 0x03, CDC_NOTIFICATION_MAX_SIZE, 0x00, 0x10,

    /* Интерфейс 2: CDC Данные (9 байт) */
    0x09, 0x04, 0x02, 0x00, 0x02, 0x0A, 0x00, 0x00, 0x00, 
    /* Эндпоинт EP3 OUT Bulk (7 байт) */
    0x07, 0x05, EP3_OUT_ADDR, 0x02, CDC_DATA_MAX_SIZE, 0x00, 0x00,
    /* Эндпоинт EP3 IN Bulk (7 байт) */
    0x07, 0x05, EP3_IN_ADDR,  0x02, CDC_DATA_MAX_SIZE, 0x00, 0x00
};

/* Строковые дескрипторы */
__ALIGN4 const uint8_t USB_StringLangID[]  = { 0x04, 0x03, LOBYTE(USB_LANGID_STRING), HIBYTE(USB_LANGID_STRING) };
__ALIGN4 const uint8_t USB_StringVendor[]  = { 0x12, 0x03, 'S',0,'T',0,'M',0,'i',0,'c',0,'r',0,'o',0,'_',0 };
__ALIGN4 const uint8_t USB_StringProduct[] = { 0x1E, 0x03, 'S',0,'T',0,'M',0,'3',0,'2',0,' ',0,'C',0,'o',0,'m',0,'b',0,'o',0,' ',0,'D',0,'e',0,'v',0 };
__ALIGN4 const uint8_t USB_StringSerial[]  = { 0x0A, 0x03, '0',0,'0',0,'0',0,'1',0 };

/*========================================================================*/
/* 3. НИЗКОУРОВНЕВАЯ ФУНКЦИЯ ОТПРАВКИ ДАННЫХ USB_EP_TX                     */
/*========================================================================*/
void USB_EP_Tx(uint8_t epnum, const uint8_t *pdata, uint32_t len) {
  if (epnum != 0 && len > 64) len = 64; 

  if (USB_GET_IN_EP(epnum)->DIEPCTL & USB_OTG_DIEPCTL_EPENA) {
    USB_GET_IN_EP(epnum)->DIEPCTL |= USB_OTG_DIEPCTL_SNAK;
    return; 
  }

  USB_GET_IN_EP(epnum)->DIEPTSIZ = 0;
    
  if (len > 0 && pdata != NULL) {
    uint32_t word_len = (len + 3) / 4; 
    uint32_t pkt_count = (epnum == 0) ? ((len + 63) / 64) : 1;
    if (epnum == 0 && pkt_count == 0) pkt_count = 1;
        
    USB_GET_IN_EP(epnum)->DIEPTSIZ |= (len << USB_OTG_DIEPTSIZ_XFRSIZ_Pos) | (pkt_count << USB_OTG_DIEPTSIZ_PKTCNT_Pos);
    USB_GET_IN_EP(epnum)->DIEPCTL |= (USB_OTG_DIEPCTL_CNAK | USB_OTG_DIEPCTL_EPENA);

    uint32_t timeout = 100000; 
    while ((USB_GET_IN_EP(epnum)->DTXFSTS & 0xFFFF) < word_len) {
      if (timeout-- == 0) {
        USB_GET_IN_EP(epnum)->DIEPCTL |= USB_OTG_DIEPCTL_SNAK | USB_OTG_DIEPCTL_EPDIS;
        return; 
      }
    }

    for (uint32_t i = 0; i < word_len; i++) {
      uint32_t word = 0;
      uint32_t bytes_left = len - (i * 4);
      if (bytes_left >= 4) { memcpy(&word, &pdata[i * 4], 4); } 
      else { memcpy(&word, &pdata[i * 4], bytes_left); }
      *USB_GET_FIFO(epnum) = word;
    }
    __DSB(); 
  } 
  else {
    USB_GET_IN_EP(epnum)->DIEPTSIZ |= (1U << USB_OTG_DIEPTSIZ_PKTCNT_Pos); 
    USB_GET_IN_EP(epnum)->DIEPCTL |= (USB_OTG_DIEPCTL_CNAK | USB_OTG_DIEPCTL_EPENA);
  }
}

uint8_t USB_EP_IsReady(uint8_t epnum) {
    return (USB_GET_IN_EP(epnum)->DIEPCTL & USB_OTG_DIEPCTL_EPENA) == 0;
}

/*========================================================================*/
/* 4. ОБРАБОТЧИК УПРАВЛЯЮЩИХ ЗАПРОСОВ ХОСТА                               */
/*========================================================================*/
void USB_Control_Handle(USB_SetupPacket_TypeDef *setup) {
  // Стандартные запросы хоста (маска 0x60 == 0x00)
  if ((setup->bmRequestType & 0x60) == 0x00) {       
    
    if (setup->bRequest == 0x06) { // GET_DESCRIPTOR
      uint8_t dtype = HIBYTE(setup->wValue);
      uint8_t didx = LOBYTE(setup->wValue);
      const uint8_t *pdesc = NULL;
      uint16_t len = 0;

      if (dtype == 0x01)       { pdesc = USB_DeviceDescriptor; len = sizeof(USB_DeviceDescriptor); }
      else if (dtype == 0x02)  { pdesc = USB_ConfigDescriptor; len = sizeof(USB_ConfigDescriptor); }
      else if (dtype == 0x06)  { pdesc = USB_DeviceQualifierDescriptor; len = sizeof(USB_DeviceQualifierDescriptor); }
      else if (dtype == 0x22)  { 
        if ((setup->wIndex & 0xFF) == 0x00) {  
          pdesc = Combined_HID_ReportDescriptor; 
          len = 185; 
        } 
      }
      else if (dtype == 0x03) {
        if (didx == 0)       { pdesc = USB_StringLangID; len = sizeof(USB_StringLangID); }
        else if (didx == 1)  { pdesc = USB_StringVendor; len = sizeof(USB_StringVendor); }
        else if (didx == 2)  { pdesc = USB_StringProduct; len = sizeof(USB_StringProduct); }
        else if (didx == 3)  { pdesc = USB_StringSerial; len = sizeof(USB_StringSerial); }
      }
            
      if (pdesc && len > 0) {
        if (len > setup->wLength) len = setup->wLength;
        uint32_t send_len = len;
        if (dtype == 0x22 && send_len > 64) { send_len = 64; } // Лимит одной посылки EP0
        USB_EP_Tx(0, pdesc, send_len);
      } else {
        EP0_IN->DIEPCTL |= USB_OTG_DIEPCTL_STALL; 
      }
    } 
    else if (setup->bRequest == 0x05) { // SET_ADDRESS
      usb_pending_address = LOBYTE(setup->wValue); 
      USB_EP_Tx(0, NULL, 0); 
      if (usb_pending_address != 0) {
        USB_DEVICE->DCFG = (USB_DEVICE->DCFG & ~0x7F0U) | ((uint32_t)usb_pending_address << 4U);
        usb_pending_address = 0; 
        EP0_OUT->DOEPTSIZ = (1U << USB_OTG_DOEPTSIZ_STUPCNT_Pos) | (EP0_MAX_PACKET_SIZE << USB_OTG_DOEPTSIZ_XFRSIZ_Pos) | (1U << USB_OTG_DOEPTSIZ_PKTCNT_Pos);
        EP0_OUT->DOEPCTL |= USB_OTG_DOEPCTL_EPENA | USB_OTG_DOEPCTL_CNAK;
      }
    }
    else if (setup->bRequest == 0x09) { // SET_CONFIGURATION
      usb_configuration = LOBYTE(setup->wValue);
      
      USB_DEVICE->DAINTMSK |= (1U << 1) | (1U << 2) | (1U << 3) | (1U << 19);
            
      // Конфигурация EP1 (HID Combo)
      EP1_IN->DIEPCTL &= ~(USB_OTG_DIEPCTL_MPSIZ | USB_OTG_DIEPCTL_EPTYP | USB_OTG_DIEPCTL_TXFNUM);
      EP1_IN->DIEPCTL |= (EP1_MAX_PACKET_SIZE << USB_OTG_DIEPCTL_MPSIZ_Pos) | (3U << USB_OTG_DIEPCTL_EPTYP_Pos) | (1U << USB_OTG_DIEPCTL_TXFNUM_Pos) | (1U << 28) | USB_OTG_DIEPCTL_USBAEP; 
      EP1_IN->DIEPCTL |= USB_OTG_DIEPCTL_SNAK; 
          
      // Конфигурация EP2 (CDC Interrupt Notification)
      USB_GET_IN_EP(2)->DIEPCTL &= ~(USB_OTG_DIEPCTL_MPSIZ | USB_OTG_DIEPCTL_EPTYP | USB_OTG_DIEPCTL_TXFNUM);
      USB_GET_IN_EP(2)->DIEPCTL |= (CDC_NOTIFICATION_MAX_SIZE << USB_OTG_DIEPCTL_MPSIZ_Pos) | (3U << USB_OTG_DIEPCTL_EPTYP_Pos) | (2U << USB_OTG_DIEPCTL_TXFNUM_Pos) | (1U << 28) | USB_OTG_DIEPCTL_USBAEP; 
      USB_GET_IN_EP(2)->DIEPCTL |= USB_OTG_DIEPCTL_SNAK; 

      // Конфигурация EP3 (CDC Bulk IN Data)
      USB_GET_IN_EP(3)->DIEPCTL &= ~(USB_OTG_DIEPCTL_MPSIZ | USB_OTG_DIEPCTL_EPTYP | USB_OTG_DIEPCTL_TXFNUM);
      USB_GET_IN_EP(3)->DIEPCTL |= (CDC_DATA_MAX_SIZE << USB_OTG_DIEPCTL_MPSIZ_Pos) | (2U << USB_OTG_DIEPCTL_EPTYP_Pos) | (3U << USB_OTG_DIEPCTL_TXFNUM_Pos) | (1U << 28) | USB_OTG_DIEPCTL_USBAEP; 
      USB_GET_IN_EP(3)->DIEPCTL |= USB_OTG_DIEPCTL_SNAK; 

      // Конфигурация EP3 (CDC Bulk OUT Data)
      USB_GET_OUT_EP(3)->DOEPCTL &= ~(USB_OTG_DOEPCTL_MPSIZ | USB_OTG_DOEPCTL_EPTYP);
      USB_GET_OUT_EP(3)->DOEPCTL |= (CDC_DATA_MAX_SIZE << USB_OTG_DOEPTSIZ_XFRSIZ_Pos) | (2U << USB_OTG_DOEPCTL_EPTYP_Pos) | (1U << 28) | USB_OTG_DOEPCTL_USBAEP; 
      
      USB_GET_OUT_EP(3)->DOEPTSIZ = (CDC_DATA_MAX_SIZE << USB_OTG_DOEPTSIZ_XFRSIZ_Pos) | (1U << USB_OTG_DOEPTSIZ_PKTCNT_Pos);
      USB_GET_OUT_EP(3)->DOEPCTL |= USB_OTG_DOEPCTL_EPENA | USB_OTG_DOEPCTL_CNAK;

      USB_EP_Tx(0, NULL, 0);
    }
  }
  // Классовые запросы (HID / CDC-ACM)
  else if ((setup->bmRequestType & 0x60) == 0x20) { 
    if (setup->bRequest == 0x0A) { USB_EP_Tx(0, NULL, 0); } 
    else if (setup->bRequest == 0x20) { // SET_LINE_CODING
      EP0_OUT->DOEPTSIZ = (7U << USB_OTG_DOEPTSIZ_XFRSIZ_Pos) | (1U << USB_OTG_DOEPTSIZ_PKTCNT_Pos);
      EP0_OUT->DOEPCTL |= USB_OTG_DOEPCTL_EPENA | USB_OTG_DOEPCTL_CNAK;
    }
    else if (setup->bRequest == 0x21) { USB_EP_Tx(0, cdc_line_coding, 7); } 
    else if (setup->bRequest == 0x22) { USB_EP_Tx(0, NULL, 0); } // SET_CONTROL_LINE_STATE
    else { EP0_IN->DIEPCTL |= USB_OTG_DIEPCTL_STALL; }
  }
}

/*========================================================================*/
/* 5. ИСПРАВЛЕННЫЙ ОБРАБОТЧИК ПРЕРЫВАНИЙ И ФИКС АДРЕСОВ FIFO              */
/*========================================================================*/
void OTG_FS_IRQHandler(void) {
  uint32_t status = USB_OTG_FS->GINTSTS;
  
  if (status & USB_OTG_GINTSTS_USBRST) {
    USB_OTG_FS->GINTSTS = USB_OTG_GINTSTS_USBRST; 
    USB_DEVICE->DCFG &= ~(USB_OTG_DCFG_DAD);       
    usb_configuration = 0;
    usb_pending_address = 0;
        
    /* РАСПРЕДЕЛЕНИЕ ПАМЯТИ: Строго в рамках аппаратных индексов массива DIEPTXF */
    USB_OTG_FS->GRXFSIZ = 128; 
    USB_OTG_FS->DIEPTXF0_HNPTXFSIZ = (64 << 16) | 128; // TX FIFO 0 (EP0): 64 слова. Конец=192
    USB_OTG_FS->DIEPTXF[0] = (32 << 16) | 192;         // TX FIFO 1 (EP1 HID): 32 слова. Конец=224
    USB_OTG_FS->DIEPTXF[1] = (32 << 16) | 224;         // TX FIFO 2 (EP2 CDC Int): 32 слова. Конец=256
    USB_OTG_FS->DIEPTXF[2] = (64 << 16) | 256;         // TX FIFO 3 (EP3 CDC Bulk): 64 слова. Конец=320
    // Итого: 128 + 64 + 32 + 32 + 64 = 320 слов (Полный физический объем SRAM контроллера)
        
    USB_OTG_FS->GRSTCTL |= USB_OTG_GRSTCTL_RXFFLSH;
    while (USB_OTG_FS->GRSTCTL & USB_OTG_GRSTCTL_RXFFLSH);

    USB_OTG_FS->GRSTCTL |= (0x10 << USB_OTG_GRSTCTL_TXFNUM_Pos) | USB_OTG_GRSTCTL_TXFFLSH;
    while (USB_OTG_FS->GRSTCTL & USB_OTG_GRSTCTL_TXFFLSH);
        
    EP0_IN->DIEPCTL &= ~USB_OTG_DIEPCTL_MPSIZ; 
    EP0_IN->DIEPCTL |= USB_OTG_DIEPCTL_SNAK | USB_OTG_DIEPCTL_USBAEP; 
    EP0_OUT->DOEPCTL |= USB_OTG_DOEPCTL_SNAK | USB_OTG_DOEPCTL_USBAEP;

    // Включаем прерывания только для используемых конечных точек
    USB_DEVICE->DAINTMSK = (1U << 0) | (1U << 1) | (1U << 2) | (1U << 3) | (1U << 16) | (1U << 19); 
    USB_DEVICE->DOEPMSK = USB_OTG_DOEPMSK_XFRCM;
    USB_DEVICE->DIEPMSK = USB_OTG_DIEPMSK_XFRCM | USB_OTG_DIEPMSK_TOM; 
  }

  if (status & USB_OTG_GINTSTS_ENUMDNE) {
    USB_OTG_FS->GINTSTS = USB_OTG_GINTSTS_ENUMDNE;
    USB_DEVICE->DCFG &= ~USB_OTG_DCFG_DSPD; 
    EP0_OUT->DOEPTSIZ = (1 << USB_OTG_DOEPTSIZ_STUPCNT_Pos) | (EP0_MAX_PACKET_SIZE << USB_OTG_DOEPTSIZ_XFRSIZ_Pos) | (1 << USB_OTG_DOEPTSIZ_PKTCNT_Pos);
    EP0_OUT->DOEPCTL |= USB_OTG_DOEPCTL_EPENA | USB_OTG_DOEPCTL_CNAK;
  }

  if (status & USB_OTG_GINTSTS_RXFLVL) {
    uint32_t rx_status = USB_OTG_FS->GRXSTSP; 
    uint8_t pkt_sts = (rx_status & USB_OTG_GRXSTSP_PKTSTS) >> USB_OTG_GRXSTSP_PKTSTS_Pos;
    uint8_t epnum = rx_status & USB_OTG_GRXSTSP_EPNUM;
    uint16_t bcnt = (rx_status & USB_OTG_GRXSTSP_BCNT) >> USB_OTG_GRXSTSP_BCNT_Pos;

    if (epnum == 0) {
      if (pkt_sts == 6 && bcnt == 8) { 
        USB_SetupPacket_TypeDef setup;
        uint32_t *dest = (uint32_t*)&setup;
        *dest++ = *USB_FIFO0; *dest = *USB_FIFO0; 
        USB_Control_Handle(&setup);
      }
      else if (pkt_sts == 4) {
        EP0_OUT->DOEPTSIZ = (1U << USB_OTG_DOEPTSIZ_STUPCNT_Pos) | (EP0_MAX_PACKET_SIZE << USB_OTG_DOEPTSIZ_XFRSIZ_Pos) | (1U << USB_OTG_DOEPTSIZ_PKTCNT_Pos);
        EP0_OUT->DOEPCTL |= USB_OTG_DOEPCTL_EPENA | USB_OTG_DOEPCTL_CNAK;
      }
      else if (pkt_sts == 2 && bcnt > 0) {
        if (bcnt == 7) { // Прилетели 7 байт от запроса SET_LINE_CODING
          uint32_t w1 = *USB_FIFO0; uint32_t w2 = *USB_FIFO0;
          memcpy(&cdc_line_coding, &w1, 4); memcpy(&cdc_line_coding, &w2, 3);
          USB_EP_Tx(0, NULL, 0); // Отвечаем ZLP
        } else {
          uint32_t dummy_words = (bcnt + 3) / 4;
          for (uint32_t i = 0; i < dummy_words; i++) { (void)*USB_FIFO0; }
        }
      }
    } 
    else if (epnum == 3 && pkt_sts == 2) { // Чтение входящих данных из виртуального COM-порта компьютера
      if (bcnt > 0) {
        uint32_t words = (bcnt + 3) / 4;
        uint32_t temp_buf[16]; // Выделяем буфер под пакет до 64 байт
        for (uint32_t i = 0; i < words; i++) { temp_buf[i] = *USB_FIFO0; }
        
        // ОБРАБОТКА КОМАНД ИЗ ПК: Тут можно вызвать ваш калбек для обработки данных:
        // CDC_Receive_Callback((uint8_t*)temp_buf, bcnt);

        // Перевзводим буфер OUT-точки для ожидания следующей посылки от ПК
        USB_GET_OUT_EP(3)->DOEPTSIZ = (CDC_DATA_MAX_SIZE << USB_OTG_DOEPTSIZ_XFRSIZ_Pos) | (1U << USB_OTG_DOEPTSIZ_PKTCNT_Pos);
        USB_GET_OUT_EP(3)->DOEPCTL |= USB_OTG_DOEPCTL_EPENA | USB_OTG_DOEPCTL_CNAK;
      }
    }
    else if (bcnt > 0) { 
      uint32_t dummy_words = (bcnt + 3) / 4;
      for (uint32_t i = 0; i < dummy_words; i++) { (void)*USB_FIFO0; }
    }
  }

  if (status & USB_OTG_GINTSTS_IEPINT) {
    uint32_t ep_intr = USB_DEVICE->DAINT & 0xFFFF;
    if (ep_intr & (1U << 0)) { EP0_IN->DIEPINT = EP0_IN->DIEPINT; }
    if (ep_intr & (1U << 1)) { EP1_IN->DIEPINT = EP1_IN->DIEPINT; }
    if (ep_intr & (1U << 2)) { USB_GET_IN_EP(2)->DIEPINT = USB_GET_IN_EP(2)->DIEPINT; }
    if (ep_intr & (1U << 3)) { USB_GET_IN_EP(3)->DIEPINT = USB_GET_IN_EP(3)->DIEPINT; }
  }

  if (status & USB_OTG_GINTSTS_OEPINT) {
    uint32_t out_intr = (USB_DEVICE->DAINT >> 16) & 0xFFFF;
    if (out_intr & (1U << 0)) { 
      uint32_t ep0_out_bits = EP0_OUT->DOEPINT; EP0_OUT->DOEPINT = ep0_out_bits; 
      if (ep0_out_bits & USB_OTG_DOEPINT_XFRC) {
        EP0_OUT->DOEPTSIZ = (1U << USB_OTG_DOEPTSIZ_STUPCNT_Pos) | (EP0_MAX_PACKET_SIZE << USB_OTG_DOEPTSIZ_XFRSIZ_Pos) | (1U << USB_OTG_DOEPTSIZ_PKTCNT_Pos);
        EP0_OUT->DOEPCTL |= USB_OTG_DOEPCTL_EPENA | USB_OTG_DOEPCTL_CNAK;
      }
    }
    if (out_intr & (1U << 3)) { USB_GET_OUT_EP(3)->DOEPINT = USB_GET_OUT_EP(3)->DOEPINT; }
  }
}

/*========================================================================*/
/* 6. ИНИЦИАЛИЗАЦИЯ ПЕРИФЕРИИ USB И НАСТРОЙКА NVIC                        */
/*========================================================================*/
void USB_Core_Init(void) {
  RCC->AHB2ENR |= RCC_AHB2ENR_OTGFSEN;
  USB_OTG_FS->GCCFG |= USB_OTG_GCCFG_NOVBUSSENS;
  USB_OTG_FS->GCCFG &= ~(USB_OTG_GCCFG_VBUSBSEN | USB_OTG_GCCFG_VBUSASEN);
  USB_OTG_FS->GCCFG |= USB_OTG_GCCFG_PWRDWN;
  USB_OTG_FS->GRSTCTL |= USB_OTG_GRSTCTL_CSRST;
  while (USB_OTG_FS->GRSTCTL & USB_OTG_GRSTCTL_CSRST);
  USB_OTG_FS->GUSBCFG |= USB_OTG_GUSBCFG_FDMOD; 
  USB_OTG_FS->GUSBCFG &= ~(USB_OTG_GUSBCFG_TRDT);
  USB_OTG_FS->GUSBCFG |= (9 << USB_OTG_GUSBCFG_TRDT_Pos);
  USB_DEVICE->DCTL &= ~USB_OTG_DCTL_SDIS;
  USB_OTG_FS->GINTMSK |= USB_OTG_GINTMSK_USBRST | USB_OTG_GINTMSK_ENUMDNEM | USB_OTG_GINTMSK_RXFLVLM | USB_OTG_GINTMSK_IEPINT;
  USB_OTG_FS->GAHBCFG |= USB_OTG_GAHBCFG_GINT;
  NVIC_SetPriority(OTG_FS_IRQn, 1);
  NVIC_EnableIRQ(OTG_FS_IRQn);
}


int main(void) {
    /* Системная инициализация всех модулей через регистры */
    USB_Core_Init();

    uint32_t last_mouse_time = 0;
    uint32_t last_kbd_time = 0;
    uint32_t last_media_time = 0;
    
    uint8_t kbd_state = 0;
    uint8_t media_state = 0;

		while (1) {
			/*
        // 1. ЛОГИКА МЫШИ (каждые 40 мс двигаем курсор вправо)
        if (GetTick() - last_mouse_time >= 40) {
            last_mouse_time = GetTick();

            if (usb_configuration && !(EP1_IN->DIEPCTL & USB_OTG_DIEPCTL_EPENA)) {
                global_mouse_report[0] = 0; // Кнопки
                global_mouse_report[1] = 1; // Координата X (+1)
                global_mouse_report[2] = 0; // Координата Y
                global_mouse_report[3] = 0; // Колёсико
                
                USB_EP_Tx(1, global_mouse_report, 4);
            }
        }

        // 2. ЛОГИКА ОБЫЧНЫХ КЛАВИШ (Каждую 1 секунду)
        if (GetTick() - last_kbd_time >= 1000) {
            last_kbd_time = GetTick();

            if (usb_configuration && !(EP2_IN->DIEPCTL & USB_OTG_DIEPCTL_EPENA)) {
              
                    // Нажимаем 'A' 
                    global_keyboard_report[0] = 1;    // Report ID
                    global_keyboard_report[1] = 0;    // Модификаторы
                    global_keyboard_report[2] = 0;    // Зарезервировано
                    global_keyboard_report[3] = 0x00; // Код 'A'
                    USB_EP_Tx(2, global_keyboard_report, 9);
										Delay_ms(20);
                    kbd_state = 1;
          
                    // Отпускаем клавиши 
                     global_keyboard_report[0] = 1;    // Report ID
                    global_keyboard_report[1] = 0;    // Модификаторы
                    global_keyboard_report[2] = 0;    // Зарезервировано
                    global_keyboard_report[3] = 0x00; // Код 'A'
							USB_EP_Tx(2, global_keyboard_report, 9);
               
                
                
            }
        }

        // 3. ЛОГИКА МЕДИА-КЛАВИШ (Каждые 3 секунды громкость +) 
               if (GetTick() - last_media_time >= 3000) {
            last_media_time = GetTick();

            // Проверяем готовность USB конфигурации и конечной точки EP2_IN
            if (usb_configuration && !(EP2_IN->DIEPCTL & USB_OTG_DIEPCTL_EPENA)) {
                
                // 1. НАЖАТИЕ: Выставляем бит Volume Up во втором байте данных
                global_media_report[1] = 0;               // На всякий случай очищаем 1-й байт данных
                global_media_report[2] |= MEDIA_VOL_UP;   // Устанавливаем бит громкости во 2-м байте
                
                // Отправляем пакет (размер 3 байта)
                USB_EP_Tx(2, global_media_report, 3);
                
                // Даем задержку хосту на обработку
                Delay_ms(20); 
                
                // 2. ОТПУСКАНИЕ: Сбрасываем все биты данных в ноль (Report ID не трогаем)
                global_media_report[1] = 0;
                global_media_report[2] = 0;
                
                // Отправляем пустой репорт (все кнопки отпущены)
                USB_EP_Tx(2, global_media_report, 3);
            }
        }
				*/
    }
}
