#ifndef FLASH_VARIABLES_H
#define FLASH_VARIABLES_H

#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_flash.h"


// Максимальные и минимальные значения для переменных
#define PR_TEMP_MAX 99
#define PR_TEMP_MIN 0

#define PR_VOLTAGE_MAX 30
#define PR_VOLTAGE_MIN 0

#define PR_CURRENT_MAX 50
#define PR_CURRENT_MIN 0

/**
 * Настройки.
 *
 * Необходимо выравнивать по 4 байта (32бита)
 * !!! Full size (bytes) must be a multiple of 4 !!!
 */
struct Settings {
	uint8_t protectionTemperature; 	// 0 - 99
	uint8_t protectionVoltage;		// 0 - 30
	uint8_t protectionCurrent;		// 0 - 50
	uint8_t reserved;				//
	// В конце структуры
	uint32_t crc;
};

struct Settings settings;


// Адреса структур в памяти
// Последняя страница (stm32f103C8) 0x0800FF00 – 0x0800FFFF, размер 1Кб
// Последняя страница (stm32f103C6) 0x08007C00 - 0x08007CFF, размер 1Кб
#define SETTINGS_ADDR 0x0800FF00

// Размер в словах
#define SETTINGS_WORDS sizeof(settings) / 4



/** Функция для расчета контролькой суммы (сумма по модулу 256 всех байт) */
inline static uint8_t get_crc(uint8_t const *buf, size_t size) {
	uint8_t crc = *buf;
	while (--size) {
		crc += *(++buf);
	}
	return crc;
}

/** Инициализация */
void FLASH_Init(void) {
	FLASH_HalfCycleAccessCmd(FLASH_HalfCycleAccess_Disable); // Отключаем половинный цикл доступа к памяти, используем полный
	FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable); // Включаем кэширование
	FLASH_SetLatency(FLASH_Latency_2); // Настраиваем задержку с учетом частоты работы МК (для 72MHz = 2)
}

/** Write */
void FLASH_Write(uint32_t *source, uint32_t *dest, uint16_t words) {
	uint32_t *source_addr = source;
	uint32_t *dest_addr = dest;
	for (uint16_t i = 0; i < words; i++) {
		FLASH_ProgramWord((uint32_t)dest_addr, *source_addr);
		source_addr++;
		dest_addr++;
	}
}

/** Read */
void FLASH_Read(uint32_t *source, uint32_t *dest, uint16_t words) {
	volatile uint32_t *source_addr = source;
	volatile uint32_t *dest_addr = dest;
	for (uint16_t i = 0; i < words; i++) {
		*dest_addr = *(volatile uint32_t*)source_addr;
		source_addr++;
		dest_addr++;
	}
}

/** Write settings */
void FLASH_WriteSettings(void) {
	// Считаем crc без учета поля crc
	settings.crc = get_crc((uint8_t *)&settings, sizeof(settings) - 4);

	FLASH_Unlock();
	FLASH_ErasePage(SETTINGS_ADDR);

	/* If the erase operation is completed, disable the PER Bit */
	//CLEAR_BIT(FLASH->CR, FLASH_CR_PER);

	// Сохранили вместе с crc
	FLASH_Write((uint32_t *)&settings, (uint32_t *)SETTINGS_ADDR, SETTINGS_WORDS);
	FLASH_Lock();
}

/** Read settings */
void FLASH_ReadSettings(void) {
	// Читаем для начала в буфер
	struct Settings buf;

	// Читаем из памяти
	FLASH_Read((uint32_t *)SETTINGS_ADDR, (uint32_t *)&buf, SETTINGS_WORDS);

	uint32_t crc = get_crc((uint8_t *)&buf, sizeof(buf) - 4);
	// Проверяем crc
	if (buf.crc == crc) {
		// если все ок то копируем из буфера
		settings = buf;
	} else {
		// Инициализируем переменные
		settings.protectionTemperature = 30;
		settings.protectionVoltage = 14;
		settings.protectionCurrent = 50;
		settings.reserved = 0;

		// Сохраняем
		FLASH_WriteSettings();
	}
}

#endif // FLASH_VARIABLES_H
