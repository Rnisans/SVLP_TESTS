#include "RCC_CLOCK.h"

// Clock initialization function
void ClockInit(void) {
    // Enable HSE (High Speed External oscillator)
    RCC_HSEConfig(RCC_HSE_ON);
    
    // Wait for HSE to be ready
    if (RCC_WaitForHSEStartUp() == SUCCESS) {
        // Configure Flash latency for 72 MHz
        FLASH_SetLatency(FLASH_Latency_2);
        
        // Configure PLL
        RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
        
        // Enable PLL
        RCC_PLLCmd(ENABLE);
        
        // Wait for PLL to be ready
        while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
        
        // Select PLL as system clock source
        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
        
        // Wait for PLL to be used as system clock source
        while (RCC_GetSYSCLKSource() != 0x08);
    }
    
    // Configure AHB, APB1, and APB2 prescalers
    RCC_HCLKConfig(RCC_SYSCLK_Div1);
    RCC_PCLK1Config(RCC_HCLK_Div2);
    RCC_PCLK2Config(RCC_HCLK_Div1);
}

// SysTick initialization
void SysTick_Init(void) {
    // Configure SysTick to generate 1ms interrupts
    if (SysTick_Config(SYSTEM_CLOCK_FREQ / SYSTICK_FREQ)) {
        // Error handling - SysTick configuration failed
        while (1);
    }
}

// SysTick interrupt handler
void SysTick_Handler(void) {
    // This function is called every 1ms
    // Add any periodic tasks here if needed
}