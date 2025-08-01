#ifndef _TEMPERATURE_18B20_H
#define _TEMPERATURE_18B20_H

#include "one_wire/tm_stm32f4_ds18b20.h"

// Структура для работы OneWire
TM_OneWire_t OneWire;
// Ид датчика
uint8_t device[8];

// Текущая температура
float temperature = 0;

/**
 * Инициализация датчика температуры
 */
void temperatureInit() {
	// Инициализация датчика температуры
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	TM_OneWire_Init(&OneWire, GPIOB, GPIO_Pin_11);
	TM_OneWire_First(&OneWire);
	TM_OneWire_GetFullROM(&OneWire, device);
	//TM_DS18B20_SetResolution(&OneWire, device,  TM_DS18B20_Resolution_12bits);
	TM_DS18B20_Start(&OneWire, device);
}

/**
 * Обновить температуру
 */
void updateTemp() {
	uint8_t r = 0;
	int8_t cnt = 0;

	//прерывания запрещать нельзя! но обмен с термосенсором сбивается если пройдет прерывание
	while ((r== 0) && (cnt < 10)) {
		r = TM_DS18B20_Read(&OneWire, device, &temperature);
		cnt ++;
	}

	TM_DS18B20_Start(&OneWire, device);

	// если не измерили с 10-й попытки - вернем перегрев
	if (r == 0 || temperature == 85) {// У термометра DS18B20 значение температуры по умолчанию - 85 градусов. Т.е. когда он включается в памяти стоит значение  85
		temperature = 250;
	}
}

/**
 * Получить текущую температуру
 */
inline float getTemp() {
	return temperature;
}

#endif // _TEMPERATURE_18B20_H
