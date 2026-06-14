#include "USB.h"

#define Rx_Buf_Size  512

// Эти переменные видны только внутри файла библиотеки
static volatile uint8_t  Rx_Buf[Rx_Buf_Size];
static volatile uint32_t Rx_Buf_Head = 0;
static volatile uint32_t Rx_Buf_Tail = 0;


/* ИСПРАВЛЕНО: Явное побайтовое заполнение и строгое выравнивание */
__ALIGN4 const uint8_t USB_DeviceDescriptor[] = {
    0x12,                       // 0 bLength (Размер дескриптора)
    0x01,                       // 1 bDescriptorType (Device Descriptor)
    0x00, 0x02,                 // 2 bcdUSB (USB 2.0)
    0xEF,                       // 4 bDeviceClass 
    0x02,                       // 5 bDeviceSubClass
    0x01,                       // 6 bDeviceProtocol
    64,                         // 7 bMaxPacketSize0 (СТРОГО 64!)
	
    LOBYTE(USB_VID), HIBYTE(USB_VID),                 // 8 idVendor (STMicroelectronics 0x0483)
    LOBYTE(USB_PID), HIBYTE(USB_PID),                  // 10 idProduct (Custom HID 0x5740)
    0x00, 0x02,                 // 12 bcdDevice (Версия 2.00)
    0x01,                       // 14 iManufacturer (Индекс строки)
    0x02,                       // 15 iProduct (Индекс строки)
    0x03,                       // 16 iSerialNumber (Индекс строки)
    0x01                        // 17 bNumConfigurations (1 конфигурация)
};

__ALIGN4 const uint8_t USB_DeviceQualifierDescriptor[] = {
    0x0A,               // 0 bLength (10 байт)
    0x06,               // 1 bDescriptorType (DEVICE_QUALIFIER)
    0x00, 0x02,         // 2 bcdUSB (USB 2.0)
    0x00,               // 4 bDeviceClass
    0x00,               // 5 bDeviceSubClass
    0x00,               // 6 bDeviceProtocol
    64,                 // 7 bMaxPacketSize0 (Строго как у EP0)
    0x01,               // 8 bNumConfigurations (1 конфигурация)
    0x00                // 9 Reserved
};



__ALIGN4 const uint8_t USB_StringLangID[]  = { 0x04, 0x03, LOBYTE(USB_LANGID_STRING), HIBYTE(USB_LANGID_STRING) };
__ALIGN4 const uint8_t USB_StringVendor[]  = { 30, 0x03, 'G',0,'N',0,'P',0,'E',0,'l',0,'e',0,'c',0,'t',0,'r',0,'o',0,'n',0,'i',0,'c',0,'s',0};
__ALIGN4 const uint8_t USB_StringProduct[] = { 28, 0x03, 'G',0,'N',0,'P',0,'E',0,'_',0,'M',0,'A',0,'C',0,'R',0,'O',0,'P',0,'A',0,'D',0};
__ALIGN4 const uint8_t USB_StringSerial[]  = { 0x0A, 0x03, '0',0,'0',0,'0',0,'1',0 };

__ALIGN4 const uint8_t HID_MouseReportDescriptor[] =
{
  0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
  0x09, 0x02,        // Usage (Mouse)
  0xA1, 0x01,        // Collection (Application)
  0x09, 0x01,        //   Usage (Pointer)
  0xA1, 0x00,        //   Collection (Physical)
  0x05, 0x09,        //     Usage Page (Button)
  0x19, 0x01,        //     Usage Minimum (0x01)
  0x29, 0x03,        //     Usage Maximum (0x03)
  0x15, 0x00,        //     Logical Minimum (0)
  0x25, 0x01,        //     Logical Maximum (1)
  0x95, 0x03,        //     Report Count (3)
  0x75, 0x01,        //     Report Size (1)
  0x81, 0x02,        //     Input (Data,Var,Abs)
  0x95, 0x01,        //     Report Count (1)
  0x75, 0x05,        //     Report Size (5)
  0x81, 0x03,        //     Input (Const,Var,Abs)
  0x05, 0x01,        //     Usage Page (Generic Desktop Ctrls)
  0x09, 0x30,        //     Usage (X)
  0x09, 0x31,        //     Usage (Y)
  0x15, 0x81,        //     Logical Minimum (-127)
  0x25, 0x7F,        //     Logical Maximum (127)
  0x75, 0x08,        //     Report Size (8)
  0x95, 0x02,        //     Report Count (2)
  0x81, 0x06,        //     Input (Data,Var,Rel)
  0x09, 0x38,        //     Usage (Wheel) <-- Добавили колёсико
  0x15, 0x81,        //     Logical Minimum (-127)
  0x25, 0x7F,        //     Logical Maximum (127)
  0x75, 0x08,        //     Report Size (8)
  0x95, 0x01,        //     Report Count (1)
  0x81, 0x06,        //     Input (Data,Var,Rel)
  0xC0,              //   End Collection
  0xC0               // End Collection
};

__ALIGN4 const uint8_t HID_KeyboardReportDescriptor[] = {
    // ==========================================
    // REPORT ID 1: СТАНДАРТНАЯ КЛАВИАТУРА (65 байт)
    // ==========================================
    0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
    0x09, 0x06,        // Usage (Keyboard)
    0xA1, 0x01,        // Collection (Application)
    0x85, 0x01,        //   REPORT_ID (1)
    
    0x05, 0x07,        //   Usage Page (Kbrd/Keypad)
    0x19, 0xE0,        //   Usage Minimum (0xE0 - Left Control)
    0x29, 0xE7,        //   Usage Maximum (0xE7 - Right GUI)
    0x15, 0x00,        //   Logical Minimum (0)
    0x25, 0x01,        //   Logical Maximum (1)
    0x75, 0x01,        //   Report Size (1)
    0x95, 0x08,        //   Report Count (8) - Модификаторы
    0x81, 0x02,        //   Input (Data,Var,Abs)
    
    0x95, 0x01,        //   Report Count (1)
    0x75, 0x08,        //   Report Size (8) - Зарезервированный байт
    0x81, 0x03,        //   Input (Const,Var,Abs)
    
    0x95, 0x05,        //   Report Count (5) - Светодиоды
    0x75, 0x01,        //   Report Size (1)
    0x05, 0x08,        //   Usage Page (LEDs)
    0x19, 0x01,        //   Usage Minimum (Num Lock)
    0x29, 0x05,        //   Usage Maximum (Kana)
    0x91, 0x02,        //   Output (Data,Var,Abs)
    
    0x95, 0x01,        //   Report Count (1)
    0x75, 0x03,        //   Report Size (3) - Выравнивание светодиодов
    0x91, 0x03,        //   Output (Const,Var,Abs)
    
    0x95, 0x06,        //   Report Count (6) - Скан-коды 6 клавиш
    0x75, 0x08,        //   Report Size (8)
    0x15, 0x00,        //   Logical Minimum (0)
    0x25, 0x65,        //   Logical Maximum (101)
    0x05, 0x07,        //   Usage Page (Kbrd/Keypad)
    0x19, 0x00,        //   Usage Minimum (0)
    0x29, 0x65,        //   Usage Maximum (101)
    0x81, 0x00,        //   Input (Data,Array,Abs)
    0xC0,              // End Collection

    // ==========================================
    // REPORT ID 2: МЕДИА-КЛАВИШИ (56 байт)
    // ==========================================
    0x05, 0x0C,        // Usage Page (Consumer)
    0x09, 0x01,        // Usage (Consumer Control)
    0xA1, 0x01,        // Collection (Application)
    0x85, 0x02,        //   REPORT_ID (2)
    
    0x05, 0x0C,        //   Usage Page (Consumer)
    0x15, 0x00,        //   Logical Minimum (0)
    0x25, 0x01,        //   Logical Maximum (1)
    0x75, 0x01,        //   Report Size (1)
    0x95, 0x05,        //   Report Count (5)
    0x09, 0xB5,        //   Usage (Scan Next Track)
    0x09, 0xB6,        //   Usage (Scan Previous Track)
    0x09, 0xB7,        //   Usage (Stop)
    0x09, 0xCD,        //   Usage (Play/Pause)
    0x09, 0xE2,        //   Usage (Mute)
    0x81, 0x02,        //   Input (Data,Var,Abs)
    
    0x95, 0x01,        //   Report Count (1)
    0x75, 0x03,        //   Report Size (3) - Выравнивание байта 1
    0x81, 0x03,        //   Input (Const,Var,Abs)
    
    0x95, 0x03,        //   Report Count (3)
    0x75, 0x01,        //   Report Size (1)
    0x09, 0xE9,        //   Usage (Volume Increment)
    0x09, 0xEA,        //   Usage (Volume Decrement)
    0x0A, 0x23, 0x02,  //   Usage (WWW Home / Browser)
    0x81, 0x02,        //   Input (Data,Var,Abs)
    
    0x95, 0x01,        //   Report Count (1)
    0x75, 0x05,        //   Report Size (5) - Выравнивание байта 2
    0x81, 0x03,        //   Input (Const,Var,Abs)
    0xC0,               // End Collection
		
		// ==========================================
    // REPORT ID 3: Мышь
    // ==========================================
	  0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
		0x09, 0x02,        // Usage (Mouse)
		0xA1, 0x01,        // Collection (Application)
		0x85, 0x03,        //   REPORT_ID (3)
		
		0x09, 0x01,        //   Usage (Pointer)
		0xA1, 0x00,        //   Collection (Physical)
		0x05, 0x09,        //     Usage Page (Button)
		0x19, 0x01,        //     Usage Minimum (0x01)
		0x29, 0x03,        //     Usage Maximum (0x03)
		0x15, 0x00,        //     Logical Minimum (0)
		0x25, 0x01,        //     Logical Maximum (1)
		0x95, 0x03,        //     Report Count (3)
		0x75, 0x01,        //     Report Size (1)
		0x81, 0x02,        //     Input (Data,Var,Abs)
		0x95, 0x01,        //     Report Count (1)
		0x75, 0x05,        //     Report Size (5)
		0x81, 0x03,        //     Input (Const,Var,Abs)
		0x05, 0x01,        //     Usage Page (Generic Desktop Ctrls)
		0x09, 0x30,        //     Usage (X)
		0x09, 0x31,        //     Usage (Y)
		0x15, 0x81,        //     Logical Minimum (-127)
		0x25, 0x7F,        //     Logical Maximum (127)
		0x75, 0x08,        //     Report Size (8)
		0x95, 0x02,        //     Report Count (2)
		0x81, 0x06,        //     Input (Data,Var,Rel)
		0x09, 0x38,        //     Usage (Wheel) <-- Добавили колёсико
		0x15, 0x81,        //     Logical Minimum (-127)
		0x25, 0x7F,        //     Logical Maximum (127)
		0x75, 0x08,        //     Report Size (8)
		0x95, 0x01,        //     Report Count (1)
		0x81, 0x06,        //     Input (Data,Var,Rel)
		0xC0,              //   End Collection
		0xC0               // End Collection
};

__ALIGN4 const uint8_t USB_ConfigDescriptor[] = {
    /*=== Descriptor Configuration (9 байт) ===*/
    0x09,                               // bLength
    0x02,                               // bDescriptorType (Configuration)
    
    // wTotalLength: Вычисляется автоматически (ровно 100 байт)
    100,        // Младший байт общей длины
    0, // Старший байт общей длины
    
    0x03,                               // bNumInterfaces: 3 интерфейса (1 HID + 2 CDC)
    0x01,                               // bConfigurationValue
    0x00,                               // iConfiguration
    0xC0,                               // bmAttributes (Self-powered)
    0x32,                               // bMaxPower (100 mA)

    /*========================================================================*/
    /* ИНТЕРФЕЙС 0: КОМБИНИРОВАННЫЙ HID (Клавиатура + Медиа + Мышь)           */
    /*========================================================================*/
    /*=== Descriptor Interface 0 (9 байт) ===*/
    0x09, 0x04, 0x00, 0x00, 0x01, 0x03, 0x00, 0x00, 0x00,

    /*=== Descriptor HID (9 байт) ===*/
    0x09, 0x21, 0x11, 0x01, 0x00, 0x01, 0x22, 
    (uint8_t)(sizeof(HID_KeyboardReportDescriptor) & 0xFF),        // Младший байт размера Report (185 байт)
    (uint8_t)((sizeof(HID_KeyboardReportDescriptor) >> 8) & 0xFF), // Старший байт размера Report

    /*=== Descriptor Endpoint 1 IN (7 байт) ===*/
    0x07, 0x05, EP1_IN_ADDR, 0x03, EP1_MAX_PACKET_SIZE, 0x00, CUSTOM_HID_FS_BINTERVAL,

    /*========================================================================*/
    /* ИНТЕРФЕЙСЫ 1 и 2: ВИРТУАЛЬНЫЙ COM ПОРТ (CDC VCP через IAD)              */
    /*========================================================================*/
    /*=== Interface Association Descriptor (IAD) (8 байт) ===*/
    0x08,                               // bLength
    0x0B,                               // bDescriptorType (Interface Association Descriptor)
    0x01,                               // bFirstInterface: Начинается с интерфейса №1
    0x02,                               // bInterfaceCount: Связывает 2 интерфейса (№1 и №2)
    0x02,                               // bFunctionClass: Communication Device Class (CDC)
    0x02,                               // bFunctionSubClass: Abstract Control Model (ACM)
    0x01,                               // bFunctionProtocol: AT Commands (VCP стандарт)
    0x00,                               // iFunction

    /*=== CDC Communication Interface (Интерфейс 1) (9 байт) ===*/
    0x09, 0x04, 0x01, 0x00, 0x01, 0x02, 0x02, 0x01, 0x00,

    /*=== CDC Functional Descriptors (19 байт) ===*/
    0x05, 0x24, 0x00, 0x10, 0x01,       // Header Functional Descriptor
    0x05, 0x24, 0x01, 0x00, 0x01,       // Call Management Functional Descriptor
    0x04, 0x24, 0x02, 0x02,             // Abstract Control Management (ACM) Descriptor
    0x05, 0x24, 0x06, 0x01, 0x02,       // Union Functional Descriptor (Связывает интерфейсы 1 и 2)

    /*=== Descriptor Endpoint 2 IN (Interrupt Notification) (7 байт) ===*/
    0x07, 0x05, EP2_IN_ADDR, 0x03, EP2_MAX_PACKET_SIZE, 0x00, 0xFF,

    /*=== CDC Data Interface (Интерфейс 2) (9 байт) ===*/
    0x09, 0x04, 0x02, 0x00, 0x02, 0x0A, 0x00, 0x00, 0x00,

    /*=== Descriptor Endpoint 3 OUT (Прием данных из терминала ПК) (7 байт) ===*/
    0x07, 0x05, EP3_OUT_ADDR, 0x02, EP3_MAX_PACKET_SIZE, 0x00, 0x00,

    /*=== Descriptor Endpoint 3 IN (Отправка данных в терминал ПК) (7 байт) ===*/
    0x07, 0x05, EP3_IN_ADDR, 0x02, EP3_MAX_PACKET_SIZE, 0x00, 0x00
};



void USB_Core_Init(void) {
  /* 1. Включаем тактирование USB OTG FS контроллера */
  RCC->AHB2ENR |= RCC_AHB2ENR_OTGFSEN;

  /* 2. Отключаем аппаратный контроль линии VBUS (критично для Black Pill) */
  USB_OTG_FS->GCCFG |= USB_OTG_GCCFG_NOVBUSSENS;
  USB_OTG_FS->GCCFG &= ~(USB_OTG_GCCFG_VBUSBSEN | USB_OTG_GCCFG_VBUSASEN);

  /* 3. Активируем встроенный FS PHY трансивер */
	USB_OTG_FS->GCCFG |= USB_OTG_GCCFG_PWRDWN;


  /* 4. Перезапускаем аппаратное ядро */
  USB_OTG_FS->GRSTCTL |= USB_OTG_GRSTCTL_CSRST;
  while (USB_OTG_FS->GRSTCTL & USB_OTG_GRSTCTL_CSRST);

  /* 5. Конфигурируем режим устройства (Device Mode) */
  USB_OTG_FS->GUSBCFG |= USB_OTG_GUSBCFG_FDMOD; 
    
  /* 6. Настраиваем задержки шины (Turnaround time) под частоту 96 МГц */
  USB_OTG_FS->GUSBCFG &= ~(USB_OTG_GUSBCFG_TRDT);
  USB_OTG_FS->GUSBCFG |= (9 << USB_OTG_GUSBCFG_TRDT_Pos);

  /* 7. Выделяем память статического Rx/Tx FIFO под конечные точки */         
	// Это происходит при RST
	
  /* 8. Выводим устройство из режима программного отключения (Soft Disconnect) */
  USB_DEVICE->DCTL &= ~USB_OTG_DCTL_SDIS;

  /* 9. Включаем маску прерываний контроллера USB */
  USB_OTG_FS->GINTMSK |= USB_OTG_GINTMSK_USBRST | USB_OTG_GINTMSK_ENUMDNEM |
                           USB_OTG_GINTMSK_RXFLVLM | USB_OTG_GINTMSK_IEPINT;
    
  /* 10. Разрешаем глобальные прерывания модуля */
  USB_OTG_FS->GAHBCFG |= USB_OTG_GAHBCFG_GINT;

  /* 11. Активируем вектор прерывания в контроллере NVIC */
  NVIC_SetPriority(OTG_FS_IRQn, 1);
  NVIC_EnableIRQ(OTG_FS_IRQn);
}

void USB_EP_Tx(uint8_t epnum, const uint8_t *pdata, uint32_t len) {
  // Для обычных точек режем по 64, для EP0 разрешаем передать длинный дескриптор
  if (epnum != 0 && len > 64) len = 64; 

  if (USB_GET_IN_EP(epnum)->DIEPCTL & USB_OTG_DIEPCTL_EPENA) {
		USB_GET_IN_EP(epnum)->DIEPCTL |= USB_OTG_DIEPCTL_SNAK;
    USB_GET_IN_EP(epnum)->DIEPCTL |= USB_OTG_DIEPCTL_EPDIS; 
    return; 
  }

  USB_GET_IN_EP(epnum)->DIEPTSIZ = 0;
    
  if (len > 0 && pdata != NULL) {
		uint32_t word_len = (len + 3) / 4; 
        
		// ВАЖНО: Вычисляем, сколько пакетов по 64 байта потребуется для отправки всей длины
    // Для 121 байта: (121 + 63) / 64 = 184 / 64 = 2 пакета (один на 64 байта, второй на 57 байт)
    uint32_t pkt_count = (len + 63) / 64;
    if (epnum == 0 && pkt_count == 0) pkt_count = 1;
        
    // Записываем правильный XFRSIZ и PKTCNT в регистр
    USB_GET_IN_EP(epnum)->DIEPTSIZ |= (len << USB_OTG_DIEPTSIZ_XFRSIZ_Pos) | (pkt_count << USB_OTG_DIEPTSIZ_PKTCNT_Pos);
        
    USB_GET_IN_EP(epnum)->DIEPCTL |= (USB_OTG_DIEPCTL_CNAK | USB_OTG_DIEPCTL_EPENA);

    // Ожидание места в FIFO
    uint32_t timeout = 100000; 
    while ((USB_GET_IN_EP(epnum)->DTXFSTS & 0xFFFF) < word_len) {
			if (timeout-- == 0) {
				USB_GET_IN_EP(epnum)->DIEPCTL |= USB_OTG_DIEPCTL_SNAK | USB_OTG_DIEPCTL_EPDIS;
        return; 
      }
		}

    // Записываем ВСЕ 121 байт (31 слово) в FIFO управляющей точки за один раз!
    for (uint32_t i = 0; i < word_len; i++) {
			uint32_t word = 0;
      uint32_t bytes_left = len - (i * 4);
      if (bytes_left >= 4) {
				memcpy(&word, &pdata[i * 4], 4);
      } 
			else {
				memcpy(&word, &pdata[i * 4], bytes_left);
      }
      *USB_GET_FIFO(epnum) = word;
		}
    __DSB(); 
    
	} 
	else {
		// Отправка ZLP
    USB_GET_IN_EP(epnum)->DIEPTSIZ |= (1U << USB_OTG_DIEPTSIZ_PKTCNT_Pos); 
    USB_GET_IN_EP(epnum)->DIEPCTL |= (USB_OTG_DIEPCTL_CNAK | USB_OTG_DIEPCTL_EPENA);
	}
}

void USB_Control_Handle(USB_SetupPacket_TypeDef *setup) {
    // Проверяем тип запроса: Стандартный (0x00) или Стандартный к интерфейсу/конечной точке (0x80/0x81/0x01)
    // Маска 0x60 выделяет тип запроса (0x00 = Стандартный, 0x20 = Классовый, 0x40 = Вендорный)
    if ((setup->bmRequestType & 0x60) == 0x00) {       
        if (setup->bRequest == 0x06) { // GET_DESCRIPTOR
            uint8_t dtype = HIBYTE(setup->wValue);
            uint8_t didx = LOBYTE(setup->wValue);
            const uint8_t *pdesc = NULL;
            uint32_t len = 0;

            if (dtype == 0x01)       { pdesc = USB_DeviceDescriptor; len = sizeof(USB_DeviceDescriptor); }
            else if (dtype == 0x02)  { pdesc = USB_ConfigDescriptor; len = sizeof(USB_ConfigDescriptor); }
            else if (dtype == 0x06)  { pdesc = USB_DeviceQualifierDescriptor; len = sizeof(USB_DeviceQualifierDescriptor); }
            else if (dtype == 0x22)  { 
                if (setup->wIndex == 0x00) { // Интерфейс 0 - Составной HID (Клавиатура + Медиа + Мышь)
                    pdesc = HID_KeyboardReportDescriptor; 
                    len = sizeof(HID_KeyboardReportDescriptor); 
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

                // Если дескриптор короткий (до 64 байт включительно), шлем его одним махом
                if (len <= 64) {
                    USB_EP_Tx(0, pdesc, len);
                } 
                else {
                    // Включаем пошаговую отправку длинного дескриптора пачками по 64 байта
                    uint32_t rem_len = len;
                    const uint8_t *curr_ptr = pdesc;

                    while (rem_len > 0) {
                        uint32_t chunk_size = (rem_len > 64) ? 64 : rem_len;

                        USB_EP_Tx(0, curr_ptr, chunk_size);

                        curr_ptr += chunk_size;
                        rem_len -= chunk_size;

                        if (rem_len > 0) {
                            uint32_t tx_timeout = 500000;
                            while ((EP0_IN->DIEPCTL & USB_OTG_DIEPCTL_EPENA) && --tx_timeout);
                            for (volatile int delay = 0; delay < 200; delay++);
                        }
                    }
                }
            } 
            else {
                EP0_IN->DIEPCTL |= USB_OTG_DIEPCTL_STALL; // Неизвестный дескриптор -> STALL
            }
        } 
        else if (setup->bRequest == 0x05) { // SET_ADDRESS
            USB_EP_Tx(0, NULL, 0); 
            
            if (LOBYTE(setup->wValue) != 0) {
                USB_DEVICE->DCFG = (USB_DEVICE->DCFG & ~0x7F0U) | ((uint32_t)LOBYTE(setup->wValue) << 4U);
                    
                EP0_OUT->DOEPTSIZ = (1U << USB_OTG_DOEPTSIZ_STUPCNT_Pos) | 
                                    (EP0_MAX_PACKET_SIZE << USB_OTG_DOEPTSIZ_XFRSIZ_Pos) | 
                                    (1U << USB_OTG_DOEPTSIZ_PKTCNT_Pos);
                EP0_OUT->DOEPCTL |= USB_OTG_DOEPCTL_EPENA | USB_OTG_DOEPCTL_CNAK;
            }
        }
        else if (setup->bRequest == 0x09) { // SET_CONFIGURATION
            
            // Включаем маску прерываний для всех используемых точек: EP1, EP2, EP3 (как IN, так и OUT)
            USB_DEVICE->DAINTMSK |= (1U << 1) | (1U << 2) | (1U << 3) | (1U << (16 + 3)); 
            
            // --- 1. Активация EP1 IN (HID: Клавиатура, медиа, мышь) ---
            EP1_IN->DIEPCTL &= ~(USB_OTG_DIEPCTL_MPSIZ | USB_OTG_DIEPCTL_EPTYP | USB_OTG_DIEPCTL_TXFNUM);
            EP1_IN->DIEPCTL |= (EP1_MAX_PACKET_SIZE << USB_OTG_DIEPCTL_MPSIZ_Pos) |
                               (3U << USB_OTG_DIEPCTL_EPTYP_Pos) | // Interrupt
                               (1U << USB_OTG_DIEPCTL_TXFNUM_Pos) | // TX FIFO 1
                               (1U << 28)                         | // DATA0 PID
                               USB_OTG_DIEPCTL_USBAEP; 
            EP1_IN->DIEPCTL |= USB_OTG_DIEPCTL_SNAK; 
                    
            // --- 2. Активация EP2 IN (CDC Command: Прерывания уведомлений линии VCP) ---
            EP2_IN->DIEPCTL &= ~(USB_OTG_DIEPCTL_MPSIZ | USB_OTG_DIEPCTL_EPTYP | USB_OTG_DIEPCTL_TXFNUM);
            EP2_IN->DIEPCTL |= (EP2_MAX_PACKET_SIZE << USB_OTG_DIEPCTL_MPSIZ_Pos) |
                               (3U << USB_OTG_DIEPCTL_EPTYP_Pos) | // Interrupt
                               (2U << USB_OTG_DIEPCTL_TXFNUM_Pos) | // TX FIFO 2
                               (1U << 28)                         | // DATA0 PID
                               USB_OTG_DIEPCTL_USBAEP; 
            EP2_IN->DIEPCTL |= USB_OTG_DIEPCTL_SNAK; 

            // --- 3. Активация EP3 IN (CDC Data IN: Отправка данных из МК в терминал ПК) ---
            EP3_IN->DIEPCTL &= ~(USB_OTG_DIEPCTL_MPSIZ | USB_OTG_DIEPCTL_EPTYP | USB_OTG_DIEPCTL_TXFNUM);
            EP3_IN->DIEPCTL |= (EP3_MAX_PACKET_SIZE << USB_OTG_DIEPCTL_MPSIZ_Pos) |
                               (2U << USB_OTG_DIEPCTL_EPTYP_Pos) | // Bulk
                               (3U << USB_OTG_DIEPCTL_TXFNUM_Pos) | // TX FIFO 3
                               (1U << 28)                         | // DATA0 PID
                               USB_OTG_DIEPCTL_USBAEP;
            EP3_IN->DIEPCTL |= USB_OTG_DIEPCTL_SNAK;

            // --- 4. Активация EP3 OUT (CDC Data OUT: Прием данных из терминала ПК в МК) ---
            EP3_OUT->DOEPCTL &= ~(USB_OTG_DOEPCTL_MPSIZ | USB_OTG_DOEPCTL_EPTYP);
            EP3_OUT->DOEPCTL |= (EP3_MAX_PACKET_SIZE << USB_OTG_DOEPCTL_MPSIZ_Pos) |
                                (2U << USB_OTG_DOEPCTL_EPTYP_Pos) | // Bulk
                                (1U << 28)                         | // DATA0 PID
                                USB_OTG_DOEPCTL_USBAEP;
            
            // Настраиваем буфер приема для EP3 OUT, чтобы контроллер физически мог ловить Bulk пакеты
            EP3_OUT->DOEPTSIZ = (1U << USB_OTG_DOEPTSIZ_PKTCNT_Pos) | (EP3_MAX_PACKET_SIZE << USB_OTG_DOEPTSIZ_XFRSIZ_Pos);
            EP3_OUT->DOEPCTL |= USB_OTG_DOEPCTL_EPENA | USB_OTG_DOEPCTL_CNAK;
                    
            USB_EP_Tx(0, NULL, 0); // Статусный ZLP хосту, конфигурация принята успешно
        }
        else {
            EP0_IN->DIEPCTL |= USB_OTG_DIEPCTL_STALL; 
        }
    } 
    // Классовые запросы (bmRequestType & 0x60 == 0x20). 
    else if ((setup->bmRequestType & 0x60) == 0x20) { 
        if (setup->bRequest == 0x0A) { // SET_IDLE (HID)
            USB_EP_Tx(0, NULL, 0); // Отвечаем ZLP
        } 
        // 1. CDC: SET_LINE_CODING (Хост выставляет бодрейт/стоп-биты при открытии терминала)
        else if (setup->bRequest == 0x20) {
            // Хост шлет 7 байт настроек OUT-пакетом. Подтверждаем готовность принять/обработать
            USB_EP_Tx(0, NULL, 0); 
        }
        // 2. CDC: GET_LINE_CODING (Хост опрашивает текущую скорость VCP)
        else if (setup->bRequest == 0x21) {
            // Обязаны жестко вернуть 7 байт стандартной структуры (115200, 1 стоп-бит, нет четности, 8 бит данных)
            static const uint8_t default_line_coding[] = {
                0x00, 0xC2, 0x01, 0x00, // Скорсть: 115200 (0x0001C200) Little-Endian
                0x00,                   // 1 стоп-бит
                0x00,                   // Четность: None
                0x08                    // 8 бит данных
            };
            uint32_t cdc_len = sizeof(default_line_coding);
            if (cdc_len > setup->wLength) cdc_len = setup->wLength;
            USB_EP_Tx(0, default_line_coding, cdc_len);
        }
        // 3. CDC: SET_CONTROL_LINE_STATE (Хост дергает сигналы DTR/RTS)
        else if (setup->bRequest == 0x22) {
            USB_EP_Tx(0, NULL, 0); // Просто шлем ZLP-подтверждение
        }
        else {
            EP0_IN->DIEPCTL |= USB_OTG_DIEPCTL_STALL;
        }
    } 
    else {
        EP0_IN->DIEPCTL |= USB_OTG_DIEPCTL_STALL;
    }
}


void OTG_FS_IRQHandler(void) {
	uint32_t status = USB_OTG_FS->GINTSTS;
  
	/* 1. Прерывание сброса шины со стороны ПК (USB Reset) */
  if (status & USB_OTG_GINTSTS_USBRST) {
		USB_OTG_FS->GINTSTS = USB_OTG_GINTSTS_USBRST; 
    USB_DEVICE->DCFG &= ~(USB_OTG_DCFG_DAD);       
        
    USB_OTG_FS->GRXFSIZ = 128; // RX FIFO (Хватит для приема пакетов HID и CDC)

		// TX FIFO 0 (Для EP0): Размер 64 слова. Стартует со 128
		USB_OTG_FS->DIEPTXF0_HNPTXFSIZ = (64 << 16) | 128; 

		// TX FIFO 1 (Для EP1 HID): Размер 16 слов (64 байта). Стартует со 192
		USB_OTG_FS->DIEPTXF[0] = (16 << 16) | 192;         

		// TX FIFO 2 (Для EP2 CDC Command): Размер 16 слов (64 байта). Стартует со 208
		USB_OTG_FS->DIEPTXF[1] = (16 << 16) | 208;         

		// TX FIFO 3 (Для EP3 CDC Data IN): Размер 32 слова (128 байт, для Bulk-скорости VCP). Стартует со 224
		USB_OTG_FS->DIEPTXF[2] = (32 << 16) | 224; 
        
    USB_OTG_FS->GRSTCTL |= USB_OTG_GRSTCTL_RXFFLSH;
    while (USB_OTG_FS->GRSTCTL & USB_OTG_GRSTCTL_RXFFLSH) {
			// Ждем окончания сброса приемного FIFO
    }

    // Сбрасываем абсолютно все передающие очереди (TX FIFO Flush)
    USB_OTG_FS->GRSTCTL |= (0x10 << USB_OTG_GRSTCTL_TXFNUM_Pos) | USB_OTG_GRSTCTL_TXFFLSH;
    while (USB_OTG_FS->GRSTCTL & USB_OTG_GRSTCTL_TXFFLSH) {
			// Ждем окончания сброса всех TX FIFO
    }
        
    // Сброс автоматов EP0
    EP0_IN->DIEPCTL &= ~USB_OTG_DIEPCTL_MPSIZ; 
    EP0_IN->DIEPCTL |= USB_OTG_DIEPCTL_SNAK | USB_OTG_DIEPCTL_USBAEP; 
    EP0_OUT->DOEPCTL |= USB_OTG_DOEPCTL_SNAK | USB_OTG_DOEPCTL_USBAEP;

    // Включаем прерывания для EP0, EP1 и добавляем маски для EP2 (бит 2 и бит 18)
    USB_DEVICE->DAINTMSK = (1U << 0) | (1U << 1) | (1U << 2) | 
													 (1U << 16) | (1U << 17) | (1U << 18); 
        
		// ИСПРАВЛЕНИЕ 2: Задаем жесткие маски прерываний конечных точек. 
    // Для IN-направления обязательно добавляем маску таймаута (TOM)
    USB_DEVICE->DOEPMSK = USB_OTG_DOEPMSK_XFRCM;
    USB_DEVICE->DIEPMSK = USB_OTG_DIEPMSK_XFRCM | USB_OTG_DIEPMSK_TOM; 
	}


  /* 2. Завершение энумерации скорости */
  if (status & USB_OTG_GINTSTS_ENUMDNE) {
		USB_OTG_FS->GINTSTS = USB_OTG_GINTSTS_ENUMDNE;
    USB_DEVICE->DCFG &= ~USB_OTG_DCFG_DSPD; // Переход в режим Full Speed
        
    // Первичное открытие буфера OUT0 под будущий SETUP-пакет
    EP0_OUT->DOEPTSIZ = (1 << USB_OTG_DOEPTSIZ_STUPCNT_Pos) | 
											  (EP0_MAX_PACKET_SIZE << USB_OTG_DOEPTSIZ_XFRSIZ_Pos) | 
                        (1 << USB_OTG_DOEPTSIZ_PKTCNT_Pos);
		EP0_OUT->DOEPCTL |= USB_OTG_DOEPCTL_EPENA | USB_OTG_DOEPCTL_CNAK;
	}

  /* 3. Обработка приемной очереди (RX FIFO Level) */
  if (status & USB_OTG_GINTSTS_RXFLVL) {
		// Чтение из GRXSTSP выталкивает ТЕКУЩУЮ строку статуса. 
    // Повторное чтение в этом же прерывании вытолкнет следующую (если есть).
    uint32_t rx_status = USB_OTG_FS->GRXSTSP; 
    uint8_t pkt_sts = (rx_status & USB_OTG_GRXSTSP_PKTSTS) >> USB_OTG_GRXSTSP_PKTSTS_Pos;
    uint8_t epnum = rx_status & USB_OTG_GRXSTSP_EPNUM;
    uint16_t bcnt = (rx_status & USB_OTG_GRXSTSP_BCNT) >> USB_OTG_GRXSTSP_BCNT_Pos;

    if (epnum == 0) {
			// Статус 6: Прилетели сырые 8 байт SETUP-пакета
      if (pkt_sts == 6) { 
				if (bcnt == 8) { 
					USB_SetupPacket_TypeDef setup;
          uint32_t *dest = (uint32_t*)&setup;
                    
          *dest++ = *USB_FIFO0; 
          *dest   = *USB_FIFO0; 
                    
          // Передаем управление парсеру
          USB_Control_Handle(&setup);
				}
			}
      // Статус 4: Аппаратный маркер "SETUP завершен". С ним данных нет (bcnt=0),
      // но мы ОБЯЗАНЫ перевзвести буфер OUT0 именно ЗДЕСЬ, когда транзакция аппаратно закрылась!
      else if (pkt_sts == 4) {
				EP0_OUT->DOEPTSIZ = (1U << USB_OTG_DOEPTSIZ_STUPCNT_Pos) | 
														(EP0_MAX_PACKET_SIZE << USB_OTG_DOEPTSIZ_XFRSIZ_Pos) | 
                            (1U << USB_OTG_DOEPTSIZ_PKTCNT_Pos);
				EP0_OUT->DOEPCTL |= USB_OTG_DOEPCTL_EPENA | USB_OTG_DOEPCTL_CNAK;
			}
      // Статус 2: Обычные OUT данные на точку 0 (например, статусная стадия хоста)
      else if (pkt_sts == 2 && bcnt > 0) {
				if (bcnt > 64) bcnt = 64; // Защита от аппаратного мусора при дребезге контактов
					uint32_t dummy_words = (bcnt + 3) / 4;
          for (uint32_t i = 0; i < dummy_words; i++) { (void)*USB_FIFO0; }
        }
			} 
      // Данные для остальных конечных точек (если прилетят OUT пакеты)
       else if (bcnt > 0) { 
        if (bcnt > 64) bcnt = 64; // Защита от аппаратного мусора
        uint32_t words_to_read = (bcnt + 3) / 4;

        // Если данные прилетели именно для нашего виртуального COM-порта (EP3 OUT)
        if (epnum == 3 && pkt_sts == 2) { // pkt_sts == 2 означает успешный прием пакета данных
            __ALIGN4 static uint32_t rx_buffer[16]; // Локальный статический буфер

            // Очищаем буфер
            for (int i = 0; i < 16; i++) rx_buffer[i] = 0;

            // Вычитываем данные из общего окна USB_FIFO0
            for (uint32_t i = 0; i < words_to_read; i++) { 
                rx_buffer[i] = *USB_FIFO0; 
            }

            // Переводим указатель в байтовый формат для поочередной записи
            uint8_t *pbyte = (uint8_t*)rx_buffer;

            // Сгружаем байты во внутреннюю очередь библиотеки Rx_Buf
            for (uint32_t i = 0; i < bcnt; i++) {
                // Вычисляем следующий шаг для головы буфера по вашему размеру Rx_Buf_Size
                uint32_t next_head = (Rx_Buf_Head + 1) % Rx_Buf_Size;
                
                // Если буфер еще не переполнился (голова не догнала хвост) — записываем байт
                if (next_head != Rx_Buf_Tail) { 
                    Rx_Buf[Rx_Buf_Head] = pbyte[i];
                    Rx_Buf_Head = next_head; // Сдвигаем индекс головы вперед
                }
            }
        }
        else {
            // Если данные пришли для другой точки (например, OUT репорты HID, если появятся)
            // или статус пакета не дата — вычищаем FIFO, чтобы буфер не забивался
            for (uint32_t i = 0; i < words_to_read; i++) { 
                (void)*USB_FIFO0; 
            }
        }
    }
	}
	
  /* 4. Прерывания по отправке данных (IN Endpoints Interrupt) */
  if (status & USB_OTG_GINTSTS_IEPINT) {
    uint32_t ep_intr = USB_DEVICE->DAINT & 0xFFFF;
        
    if (ep_intr & (1U << 0)) { // Прерывание по EP0 IN
      uint32_t ep0_bits = EP0_IN->DIEPINT;
      EP0_IN->DIEPINT = ep0_bits; 
    }
    if (ep_intr & (1U << 1)) { // Прерывание по EP1 IN (Наш составной HID)
      uint32_t ep1_bits = EP1_IN->DIEPINT;
      EP1_IN->DIEPINT = ep1_bits; 
    }
    if (ep_intr & (1U << 2)) { // Прерывание по EP2 IN (CDC Command)
      uint32_t ep2_bits = EP2_IN->DIEPINT;
      EP2_IN->DIEPINT = ep2_bits; 
    }
    if (ep_intr & (1U << 3)) { // ДОБАВЛЕНО: Прерывание по EP3 IN (CDC Data IN)
      uint32_t ep3_bits = EP3_IN->DIEPINT;
      EP3_IN->DIEPINT = ep3_bits; // Очищаем флаги (например, XFRC - отправка в ПК завершена)
    }
  }

  /* 5. Прерывания по приему данных/подтверждений (OUT Endpoints Interrupt) */
  if (status & USB_OTG_GINTSTS_OEPINT) {
    uint32_t out_intr = (USB_DEVICE->DAINT >> 16) & 0xFFFF;
    
    if (out_intr & (1U << 0)) { // EP0 OUT
      uint32_t ep0_out_bits = EP0_OUT->DOEPINT;
      EP0_OUT->DOEPINT = ep0_out_bits; 

      if (ep0_out_bits & USB_OTG_DOEPINT_XFRC) {
        EP0_OUT->DOEPTSIZ = (1U << USB_OTG_DOEPTSIZ_STUPCNT_Pos) | 
                            (EP0_MAX_PACKET_SIZE << USB_OTG_DOEPTSIZ_XFRSIZ_Pos) | 
                            (1U << USB_OTG_DOEPTSIZ_PKTCNT_Pos);
        EP0_OUT->DOEPCTL |= USB_OTG_DOEPCTL_EPENA | USB_OTG_DOEPCTL_CNAK;
      }
    }

    // ДОБАВЛЕНО: Обработка приема данных от ПК на EP3 OUT (Данные VCP/терминала)
    // Внутри if (status & USB_OTG_GINTSTS_OEPINT) -> if (out_intr & (1U << 3))
    if (out_intr & (1U << 3)) { 
      uint32_t ep3_out_bits = EP3_OUT->DOEPINT;
      EP3_OUT->DOEPINT = ep3_out_bits; // Очищаем флаги прерывания

      if (ep3_out_bits & USB_OTG_DOEPINT_XFRC) {
        // Аппаратная транзакция успешно завершена (данные мы уже выгребли в RXFLVL)
        // Говорим контроллеру, что готовы принять следующий пакет
        EP3_OUT->DOEPTSIZ = (1U << USB_OTG_DOEPTSIZ_PKTCNT_Pos) | (EP3_MAX_PACKET_SIZE << USB_OTG_DOEPTSIZ_XFRSIZ_Pos);
        EP3_OUT->DOEPCTL |= USB_OTG_DOEPCTL_EPENA | USB_OTG_DOEPCTL_CNAK;
      }
    }

  }

}

uint8_t USB_EP_IsReady(uint8_t epnum) {
    // Если бит EPENA (Endpoint Enable) равен 0, значит аппаратный буфер пуст 
    // и контроллер USB готов принять следующий пакет через USB_EP_Tx.
    return (USB_GET_IN_EP(epnum)->DIEPCTL & USB_OTG_DIEPCTL_EPENA) == 0;
}

// Функция 1: Просто сообщает, сколько БАЙТ сейчас реально лежит в очереди Rx_Buf
uint32_t USB_VCP_Byte_Available(void) {
    if (Rx_Buf_Head >= Rx_Buf_Tail) {
        return Rx_Buf_Head - Rx_Buf_Tail;
    }
    return (Rx_Buf_Size - Rx_Buf_Tail) + Rx_Buf_Head;
}

// Функция 2: Читает чистый беззнаковый байт. 
// Мы вызываем её ТОЛЬКО если Available() > 0, поэтому она всегда возвращает легитимные данные!
uint8_t USB_VCP_Read(void) {
    uint8_t byte = Rx_Buf[Rx_Buf_Tail];
    
    // Сдвигаем хвост буфера вперед
    Rx_Buf_Tail = (Rx_Buf_Tail + 1) % Rx_Buf_Size;
    
    return byte; // Никаких -1, строго uint8_t (0...255)
}

void USB_VCP_Write_String(const char *str) {
    if (str == NULL) return;
    
    uint32_t len = strlen(str); // Автоматически вычисляем длину строки
    
    // Передаем данные строго через третью точку (EP3 IN)
    USB_EP_Tx(3, (const uint8_t*)str, len);
}
