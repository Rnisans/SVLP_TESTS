
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
#include "headers/svlp_writer.h"
#include "socket/socket_1.h"


// Р В¤РЎС“Р Р…Р С”РЎвЂ Р С‘РЎРЏ Р С—Р ВµРЎР‚Р ВµР Р†Р С•Р Т‘Р В° Р С‘Р В· РЎРѓРЎвЂљРЎР‚Р С•Р С”Р С‘ Р Р† float
float custom_atof(const char *str) {
    float result = 0.0f;
    float fraction = 0.1f;
    int sign = 1;

    // Р СџРЎР‚Р С•Р С—РЎС“РЎРѓР С”Р В°Р ВµР С� Р С—РЎР‚Р С•Р В±Р ВµР В»РЎвЂ№
    while (*str == ' ') str++;

    // Р С›Р В±РЎР‚Р В°Р В±Р В°РЎвЂљРЎвЂ№Р Р†Р В°Р ВµР С� Р В·Р Р…Р В°Р С”
    if (*str == '-') {
        sign = -1;
        str++;
    } else if (*str == '+') {
        str++;
    }

    // Р С›Р В±РЎР‚Р В°Р В±Р В°РЎвЂљРЎвЂ№Р Р†Р В°Р ВµР С� РЎвЂ Р ВµР В»РЎС“РЎР‹ РЎвЂЎР В°РЎРѓРЎвЂљРЎРЉ
    while (*str >= '0' && *str <= '9') {
        result = result * 10.0f + (*str - '0');
        str++;
    }

    // Р С›Р В±РЎР‚Р В°Р В±Р В°РЎвЂљРЎвЂ№Р Р†Р В°Р ВµР С� Р Т‘РЎР‚Р С•Р В±Р Р…РЎС“РЎР‹ РЎвЂЎР В°РЎРѓРЎвЂљРЎРЉ
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

float targetTemperature = 0; // Р В¦Р ВµР В»Р ВµР Р†Р В°РЎРЏ РЎвЂљР ВµР С�Р С—Р ВµРЎР‚Р В°РЎвЂљРЎС“РЎР‚Р В°
char temp_str[20];
uint32_t cmp_len = 0;
float SendTemp = 0;

// PID-Р С”Р С•РЎРЊРЎвЂћРЎвЂћР С‘РЎвЂ Р С‘Р ВµР Р…РЎвЂљРЎвЂ№
const float Kp = 12.0f;
const float Ki = 0.4f;
const float Kd = 4.0f;

// Р С›Р С–РЎР‚Р В°Р Р…Р С‘РЎвЂЎР ВµР Р…Р С‘Р Вµ Р С‘Р Р…РЎвЂљР ВµР С–РЎР‚Р В°Р В»РЎРЉР Р…Р С•Р в„– РЎвЂЎР В°РЎРѓРЎвЂљР С‘
const float IntMax = 50;
const float IntMin = -50;

// Р С›РЎв‚¬Р С‘Р В±Р С”Р С‘
float errorPrevious = 0; // Р СџРЎР‚Р ВµР Т‘РЎвЂ№Р Т‘РЎС“РЎвЂ°Р В°РЎРЏ Р С•РЎв‚¬Р С‘Р В±Р С”Р В°
float Integral = 0; // Р СњР В°Р С”Р С•Р С—Р В»Р ВµР Р…Р Р…РЎвЂ№Р в„– Р С‘Р Р…РЎвЂљР ВµР С–РЎР‚Р В°Р В»

// PID-РЎР‚Р ВµР С–РЎС“Р В»РЎРЏРЎвЂљР С•РЎР‚
int PID(){
	Read_VCP((uint8_t*)temp_str, &cmp_len);
	targetTemperature = custom_atof(temp_str);

	float error = targetTemperature - temperature; // Р С›РЎв‚¬Р С‘Р В±Р С”Р В° РЎвЂљР ВµР С”РЎС“РЎвЂ°Р ВµР в„– РЎвЂљР ВµР С�Р С—Р ВµРЎР‚Р В°РЎвЂљРЎС“РЎР‚РЎвЂ№
	float P = Kp * error;// Р С™Р С•Р С�Р С—Р С•Р Р…Р ВµР Р…РЎвЂљ P
	Integral += Ki * error;

	//Р С›Р С–РЎР‚Р В°Р Р…Р С‘РЎвЂЎР ВµР Р…Р С‘Р Вµ Р С‘Р Р…РЎвЂљР ВµР С–РЎР‚Р В°Р В»РЎРЉР Р…Р С•Р в„– РЎвЂЎР В°РЎРѓРЎвЂљР С‘
	if (Integral > IntMax){
		Integral = IntMax;
	}
	if (Integral < IntMin){
		Integral = IntMin;
	}
	float I = Integral;// Р С™Р С•Р С�Р С—Р Р…Р ВµР Р…РЎвЂљ I

	float derivative = error - errorPrevious;
	float D = Kd * derivative;// Р С™Р С•Р С�Р С—Р С•Р Р…Р ВµР Р…РЎвЂљ D

	int Power = P + I + D;
	if (Power > 100){
		Power = 100;
	}
	if (Power < 0){
		Power = 0;
	}
	errorPrevious = error;
	TIM3->CCR3 = 100 - Power;
	return Power;
}

// Р В¤РЎС“Р Р…Р С”РЎвЂ Р С‘РЎРЏ Р Т‘Р В»РЎРЏ Р С•РЎвЂљР С—РЎР‚Р В°Р Р†Р С”Р С‘ РЎРѓР С•Р С•Р В±РЎвЂ°Р ВµР Р…Р С‘Р в„– РЎвЂЎР ВµРЎР‚Р ВµР В· USB VCP
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

// Р пїЅР Р…Р С‘РЎвЂ Р С‘Р В°Р В»Р С‘Р В·Р В°РЎвЂ Р С‘РЎРЏ Р РЃР пїЅР Сљ Р Т‘Р В»РЎРЏ РЎС“Р С—РЎР‚Р В°Р Р†Р В»Р ВµР Р…Р С‘РЎРЏ Р Р…Р В°Р С–РЎР‚Р ВµР Р†Р В°РЎвЂљР ВµР В»Р ВµР С�
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
	timer.TIM_Prescaler = 7200 - 1;	//72MHz / 7200 = 10kHz
	timer.TIM_Period = 100;	//10kHz / 100 = 100Hz
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
    // Р СњР В°РЎРѓРЎвЂљРЎР‚Р С•Р в„–Р С”Р В° Р С—Р В°РЎР‚Р В°Р С�Р ВµРЎвЂљРЎР‚Р С•Р Р† USB Р С—Р С• РЎС“Р С�Р С•Р В»РЎвЂЎР В°Р Р…Р С‘РЎР‹
    LC_DATA_RATE = 115200;
    LC_STOP_BITS = 0;
    LC_PARITY_TYPE = 0;
    LC_N_BITS_DATA = 8;

    // Р С›РЎвЂљР С”Р В»РЎР‹РЎвЂЎР ВµР Р…Р С‘Р Вµ Р Р†РЎРѓР ВµРЎвЂ¦ Р С—РЎР‚Р ВµРЎР‚РЎвЂ№Р Р†Р В°Р Р…Р С‘Р в„–
    NVIC->ICER[0] = 0xFFFFFFFF;

    ClockInit();
    USBInit();
    TIMEInit();
    delay_init();
    temperatureInit();

    Socket socket;
    socket.open();
    //svlp::SVLP_Parser parser(socket);
    svlp::SVLP_Writer writer(socket, true);

    while(1) {
        USB_MANAGEMENT();
        updateTemp();
        temperature = getTemp();
        char message[20];
        memset(message, 0, 20);
        snprintf(message, sizeof(message), "%.2f,%d\r\n", temperature, PID());
		SendMsg(message);
		Delay_ms_USB(1000);
    }
}
