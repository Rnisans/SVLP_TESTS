#ifndef RCC_CLOCK_H
#define RCC_CLOCK_H

#include "stm32f10x.h"

// Function prototypes
void ClockInit(void);
void SysTick_Init(void);

// Clock configuration
#define SYSTEM_CLOCK_FREQ    72000000  // 72 MHz
#define SYSTICK_FREQ         1000      // 1 kHz

#endif // RCC_CLOCK_H