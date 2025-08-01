
extern "C" {
	#include "stm32f10x.h"
	#include "RCC_CLOCK.h"
	#include "USB-VCP.h"
	#include "string.h"
	#include "stdio.h"
	#include "Temperature18B20.h"
	#include "stm32f10x_tim.h"
	#include "delay/delay.h"
}

#include "svlp_common_lib/svlp_lib/headers/svlp_writer.h"
#include "socket/socket_1.h"

// Custom atof function for string to float conversion
float custom_atof(const char *str) {
    float result = 0.0f;
    float fraction = 0.1f;
    int sign = 1;

    // Skip leading spaces
    while (*str == ' ') str++;

    // Handle sign
    if (*str == '-') {
        sign = -1;
        str++;
    } else if (*str == '+') {
        str++;
    }

    // Parse integer part
    while (*str >= '0' && *str <= '9') {
        result = result * 10.0f + (*str - '0');
        str++;
    }

    // Parse fractional part
    if (*str == '.') {
        str++;

        while (*str >= '0' && *str <= '9') {
            result += (*str - '0') * fraction;
            fraction *= 0.1f;
            str++;
        }
    }
    return sign * result;
}

// Global variables
float targetTemperature = 0; // Target temperature
char temp_str[20];
uint32_t cmp_len = 0;
float SendTemp = 0;

// PID controller parameters
const float Kp = 12.0f;
const float Ki = 0.4f;
const float Kd = 4.0f;

// Integral limits
const float IntMax = 50;
const float IntMin = -50;

// PID variables
float errorPrevious = 0; // Previous error
float Integral = 0; // Accumulated integral

// PID controller function
int PID() {
	Read_VCP((uint8_t*)temp_str, &cmp_len);
	targetTemperature = custom_atof(temp_str);

	float error = targetTemperature - temperature; // Temperature error
	float P = Kp * error; // Proportional component
	Integral += Ki * error;

	// Limit integral component
	if (Integral > IntMax) {
		Integral = IntMax;
	}
	if (Integral < IntMin) {
		Integral = IntMin;
	}
	float I = Integral; // Integral component

	float derivative = error - errorPrevious;
	float D = Kd * derivative; // Derivative component

	int Power = P + I + D;
	if (Power > 100) {
		Power = 100;
	}
	if (Power < 0) {
		Power = 0;
	}
	errorPrevious = error;
	TIM3->CCR3 = 100 - Power;
	return Power;
}

// Function for sending messages via USB VCP
void SendMsg(const char *text) {
	if(text == NULL) return;
	int length = -1;
	const char *str = text;
	while (*str) {
		if ((*str & 0xC0) != 0x80) {
			length++;
		}
		str++;
	}
	Write_VCP((uint8_t*)text, length);
}

// Timer initialization for PWM control
void TIMEInit(void) {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitTypeDef heater;
	heater.GPIO_Pin = GPIO_Pin_0;
	heater.GPIO_Speed = GPIO_Speed_2MHz;
	heater.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &heater);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	TIM_TimeBaseInitTypeDef timer;
	TIM_OCInitTypeDef timerPWM;

	TIM_TimeBaseStructInit(&timer);
	timer.TIM_Prescaler = 7200 - 1;	// 72MHz / 7200 = 10kHz
	timer.TIM_Period = 100;	// 10kHz / 100 = 100Hz
	timer.TIM_CounterMode = TIM_CounterMode_Down;
	TIM_TimeBaseInit(TIM3, &timer);

	TIM_OCStructInit(&timerPWM);
	timerPWM.TIM_OCMode = TIM_OCMode_PWM1;
	timerPWM.TIM_OutputState = TIM_OutputState_Enable;
	timerPWM.TIM_Pulse = 0;
	timerPWM.TIM_OCPolarity = TIM_OCPolarity_Low;

	TIM_OC3Init(TIM3, &timerPWM);
	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);

	TIM_ARRPreloadConfig(TIM3, ENABLE);
	TIM_Cmd(TIM3, ENABLE);
}

int main(void) {
    // Initialize all peripherals
    ClockInit();
    USBInit();
    TIMEInit();
    SysTick_Init();
    temperatureInit();

    // Initialize socket and writer
    Socket socket;
    socket.open();
    svlp::SVLP_Writer writer(socket, true);

    // Main loop
    while(1) {
        USB_MANAGEMENT();
        updateTemp();
        temperature = getTemp();
        
        char message[20];
        memset(message, 0, 20);
        snprintf(message, sizeof(message), "%.2f,%d\r\n", temperature, PID());
		SendMsg(message);
		delay_ms(1000);
    }
}
