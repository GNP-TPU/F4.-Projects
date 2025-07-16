#include "FLASH.h"

// Функция разблокировки Flash
void Flash_Unlock(void) {
    // Ожидание завершения текущей операции Flash
    while (FLASH->SR & FLASH_SR_BSY);

    // Записываем KEY1 в FLASH_KEYR
    FLASH->KEYR = FLASH_KEY1;

    // Записываем KEY2 в FLASH_KEYR
    FLASH->KEYR = FLASH_KEY2;
}

// Функция блокировки Flash
void Flash_Lock(void) {
    FLASH->CR |= FLASH_CR_LOCK; // Установка бита LOCK
}

// Функция разблокировки опций Flash
void Flash_Unlock_Options(void) {
    // 1. Проверка, что нет операций с Flash
    while (FLASH->SR & FLASH_SR_BSY);

    // 2. Запись OPTKEY1 в FLASH_OPTKEYR
    FLASH->OPTKEYR = 0x08192A3B;

    // 3. Запись OPTKEY2 в FLASH_OPTKEYR
    FLASH->OPTKEYR = 0x4C5D6E7F;
}

// Функция модификации пользовательских опций
void Flash_Modify_Options(uint32_t option_value) {
    // 1. Проверка, что нет операций с Flash
    while (FLASH->SR & FLASH_SR_BSY);

    // 2. Запись желаемого значения опции в FLASH_OPTCR
    FLASH->OPTCR |= option_value;

    // 3. Установка бита OPTSTRT для начала операции
    FLASH->OPTCR |= FLASH_OPTCR_OPTSTRT;

    // 4. Ожидание, пока флаг BSY не будет сброшен
    while (FLASH->SR & FLASH_SR_BSY);

    // 5. Очистка бита OPTSTRT (если необходимо)
    FLASH->OPTCR &= ~FLASH_OPTCR_OPTSTRT;
}

// Функция блокировки опций Flash
void Flash_Set_OptLock(void) {
    // 1. Проверка, что нет операций с Flash
    while (FLASH->SR & FLASH_SR_BSY);

    // 2. Установка бита OPTLOCK в FLASH_OPTCR
    FLASH->OPTCR |= FLASH_OPTCR_OPTLOCK;

    // 3. Ожидание, пока флаг BSY не будет сброшен
    while (FLASH->SR & FLASH_SR_BSY);
}
// Функция разблокировки записи во Flash
void Flash_Unlock_Write_Protection(void) {
    // 1. Разблокировка опций Flash
    Flash_Unlock_Options();

    // 2. Проверка, что нет операций с Flash
    while (FLASH->SR & FLASH_SR_BSY);

    // 3. Сброс битов защиты записи (например, установка значения 0x00000000)
    Flash_Modify_Options(0x1F << 16); // Измените значение на необходимое

    // 4. Установка бита OPTLOCK, если требуется
    Flash_Set_OptLock();
}

// Функция стирания сектора Flash
void Flash_Erase_Sector(uint32_t sector) {
    // 1. Проверка, что нет операций с Flash
    while (FLASH->SR & FLASH_SR_BSY);

    // 2. Установка бита SER и выбор сектора
    FLASH->CR |= FLASH_CR_SER; // Установка бита SER
    FLASH->CR |= (sector << FLASH_CR_SNB_Pos); // Выбор сектора (SNB)

    // 3. Установка бита STRT для запуска операции стирания
    FLASH->CR |= FLASH_CR_STRT;

    // 4. Ожидание завершения операции стирания
    while (FLASH->SR & FLASH_SR_BSY);

    // 5. Очистка флага EOP (End of Operation), если он установлен
    if (FLASH->SR & FLASH_SR_EOP) {
        FLASH->SR |= FLASH_SR_EOP; // Сброс флага EOP
    }

    // 6. Сброс бита SER
    FLASH->CR &= ~FLASH_CR_SER;
}

// Функция записи данных в Flash
void Flash_Write(uint32_t address, uint32_t data) {
	while (FLASH->SR & FLASH_SR_BSY);
		
	FLASH->CR &= ~FLASH_CR_PSIZE;
	FLASH->CR |= FLASH_CR_PSIZE_1;
	
	if (FLASH->SR & FLASH_SR_EOP) {
		FLASH->SR = FLASH_SR_EOP;
	}
	
  FLASH->CR |= FLASH_CR_PG; // Установка бита PG для программирования		

  *(__IO uint32_t*)address = (uint32_t)data; // Запись данных по указанному адресу
		
  while (FLASH->SR & FLASH_SR_BSY);

  FLASH->CR &= ~FLASH_CR_PG; // Сброс бита PG
}

uint32_t Flash_Read(uint32_t address){
	return *(__IO uint32_t*)address; // Чтение данных по указанному адресу
}

