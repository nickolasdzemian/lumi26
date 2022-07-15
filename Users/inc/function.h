#ifndef __FUNCTION_H
#define __FUNCTION_H
#include "stm8s.h"
// пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ
#define LED_RED_ON GPIO_WriteLow(GPIOC, GPIO_PIN_3)     // пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ
#define LED_RED_OFF GPIO_WriteHigh(GPIOC, GPIO_PIN_3)   // пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ
#define LED_GREEN_ON GPIO_WriteLow(GPIOC, GPIO_PIN_4)   // пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ
#define LED_GREEN_OFF GPIO_WriteHigh(GPIOC, GPIO_PIN_4) // пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ
#define LED_BLUE_ON GPIO_WriteLow(GPIOC, GPIO_PIN_5)    // пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ
#define LED_BLUE_OFF GPIO_WriteHigh(GPIOC, GPIO_PIN_5)  // пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ
#define RELAY_ON GPIO_WriteHigh(GPIOD, GPIO_PIN_2)      // пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅ
#define RELAY_OFF GPIO_WriteLow(GPIOD, GPIO_PIN_2)      // пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅ
#define BUTTON GPIO_ReadInputPin(GPIOD, GPIO_PIN_4)     // пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ

typedef enum
{
  STENDBY = 0,
  AIR_MODE = 1,
  AIR_FLOOR_MODE = 2,
  ERROR_MODE = 3,
} MODE;

// пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ

// пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅ flash

#pragma location = 0x4000
__no_init uint8_t flag_previos_state; // пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ

#pragma location = 0x4004
__no_init uint8_t Floor_Limit_state;

uint16_t Time_counter;

uint8_t ADC_POT;
uint16_t ADC_AIR, ADC_FLOR;
uint8_t Relay_flag;
uint8_t Sensor_err, Sensor_no_err; // Ошибка по КЗ, ошибка по обрыву
uint8_t Sensor_overheat;           // Перегрев
uint8_t Setting_temperature;
uint8_t iCounter_Power_Btn; // Начальное значение счетчика колеса для входа в меню, нужно для сравнения
uint8_t iBlock;             // Значение для блокировки крайних значений в меню
uint8_t Counter_Power_Btn;  // Счетчик колеса для входа в меню, сравнивается с начальным значением выше
int8_t Air_temperature, Flor_temperature;
uint8_t Floor_Limit; // Ограничение по температуре пола *по умолчанию = 28*
MODE Mode;
uint8_t Counter_ADC;

// пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ
void init(void); // пїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ
void read_ADC();
uint8_t Calculate_POT_temperature(uint8_t ADC);
uint8_t Calculate_POT_setting(uint8_t ADC);
int8_t Calculate_Temper_6_8(uint16_t Code);
void delay_ms(uint16_t time);
void SET_function(void); // Функция входа в меню
void OVERHEAT(void);     // Функция обнаружения перегрева
void lightsOFF(void);    // Выключение всех индикаторов
void menuERR(void);      // Ошибка входа в меню
#endif