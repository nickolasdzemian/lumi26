#ifndef __FUNCTION_H
#define __FUNCTION_H
#include "stm8s.h"
// ����������� ������
#define LED_RED_ON GPIO_WriteLow(GPIOC, GPIO_PIN_3)     // ��������� �������� ����������
#define LED_RED_OFF GPIO_WriteHigh(GPIOC, GPIO_PIN_3)   // ���������� �������� ����������
#define LED_GREEN_ON GPIO_WriteLow(GPIOC, GPIO_PIN_4)   // ��������� �������� ����������
#define LED_GREEN_OFF GPIO_WriteHigh(GPIOC, GPIO_PIN_4) // ���������� �������� ����������
#define LED_BLUE_ON GPIO_WriteLow(GPIOC, GPIO_PIN_5)    // ��������� �������� ����������
#define LED_BLUE_OFF GPIO_WriteHigh(GPIOC, GPIO_PIN_5)  // ���������� �������� ����������
#define RELAY_ON GPIO_WriteHigh(GPIOD, GPIO_PIN_2)      // ��������� ����
#define RELAY_OFF GPIO_WriteLow(GPIOD, GPIO_PIN_2)      // ���������� ����
#define BUTTON GPIO_ReadInputPin(GPIOD, GPIO_PIN_4)     // ������ ��������� ���� ������

typedef enum
{
  STENDBY = 0,
  AIR_MODE = 1,
  AIR_FLOOR_MODE = 2,
  ERROR_MODE = 3,
} MODE;

// ����������� ���������� ����������

// ������ ��� �������� �� flash

#pragma location = 0x4000
__no_init uint8_t flag_previos_state; // ��������� ������� �� ���������� �������

#pragma location = 0x4004
__no_init uint8_t Floor_Limit_state;

uint16_t Time_counter;

uint8_t ADC_POT;
uint16_t ADC_AIR, ADC_FLOR;
uint8_t Relay_flag;
uint8_t Sensor_err, Sensor_no_err; // ������ �� ��, ������ �� ������
uint8_t Sensor_overheat;           // ��������
uint8_t Setting_temperature;
uint8_t iCounter_Power_Btn; // ��������� �������� �������� ������ ��� ����� � ����, ����� ��� ���������
uint8_t iBlock;             // �������� ��� ���������� ������� �������� � ����
uint8_t Counter_Power_Btn;  // ������� ������ ��� ����� � ����, ������������ � ��������� ��������� ����
int8_t Air_temperature, Flor_temperature;
uint8_t Floor_Limit; // ����������� �� ����������� ���� *�� ��������� = 28*
MODE Mode;
uint8_t Counter_ADC;

// ����������� �������
void init(void); // ������� ������������� ����������������
void read_ADC();
uint8_t Calculate_POT_temperature(uint8_t ADC);
uint8_t Calculate_POT_setting(uint8_t ADC);
int8_t Calculate_Temper_6_8(uint16_t Code);
void delay_ms(uint16_t time);
void SET_function(void); // ������� ����� � ����
void OVERHEAT(void);     // ������� ����������� ���������
void lightsOFF(void);    // ���������� ���� �����������
void menuERR(void);      // ������ ����� � ����
#endif