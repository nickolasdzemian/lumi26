#include "function.h"
#include "stm8s_tim2.h"

// функция инициализации микроконтроллера
void init(void)
{
  // инициализация CLK
  CLK_SYSCLKConfig(CLK_PRESCALER_HSIDIV8);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER1, ENABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER2, ENABLE);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_ADC, ENABLE);

  // инициализация портов
  GPIO_Init(GPIOC, GPIO_PIN_3, GPIO_MODE_OUT_PP_LOW_SLOW);
  GPIO_Init(GPIOC, GPIO_PIN_4, GPIO_MODE_OUT_PP_LOW_SLOW);
  GPIO_Init(GPIOC, GPIO_PIN_5, GPIO_MODE_OUT_PP_LOW_SLOW);

  GPIO_Init(GPIOD, GPIO_PIN_4, GPIO_MODE_IN_FL_NO_IT);
  GPIO_Init(GPIOD, GPIO_PIN_2, GPIO_MODE_OUT_PP_LOW_SLOW);

  // инициализация таймера
  // настраиваем таймер для генерирования прерываний с частотой 50 Гц
  // 2МГц / 32 / 1250 = 50 Гц
  TIM1_DeInit();
  TIM1_TimeBaseInit(32 - 1, TIM1_COUNTERMODE_UP, 62500 - 1, 0);
  TIM1_ITConfig(TIM1_IT_UPDATE, ENABLE);
  // разрешаем работу таймера
  TIM1_Cmd(ENABLE);

  // TIM2 Peripheral Configuration
  TIM2_DeInit();
  // Time Base configuration
  TIM2_TimeBaseInit(TIM2_PRESCALER_1, 2000);
  // Prescaler configuration
  TIM2_PrescalerConfig(TIM2_PRESCALER_1, TIM2_PSCRELOADMODE_IMMEDIATE);
  TIM2_ITConfig(TIM2_IT_UPDATE, ENABLE);
  // TIM2 counter enable
  // TIM2_Cmd(ENABLE);

  // инициализация АЦП
  /*  Init GPIO for ADC2 */
  GPIO_Init(GPIOD, GPIO_PIN_3, GPIO_MODE_IN_FL_NO_IT);
  GPIO_Init(GPIOD, GPIO_PIN_5, GPIO_MODE_IN_FL_NO_IT);
  GPIO_Init(GPIOD, GPIO_PIN_6, GPIO_MODE_IN_FL_NO_IT);
  enableInterrupts();
}

/********************************************
 *  Задержка в шагом 1 мс                    *
 ********************************************/
void delay_ms(uint16_t time)
{
  Time_counter = time;
  TIM2_Cmd(ENABLE);
  while (Time_counter)
    ;
  TIM2_Cmd(DISABLE);
}
/********************************************
 *  Чтение результата преобразования АЦП     *
 ********************************************/
void read_ADC(void)
{
  // чтение уставки с потенциометра
  /* De-Init ADC peripheral*/
  ADC1_DeInit();
  /* Init ADC2 peripheral */
  ADC1_Init(ADC1_CONVERSIONMODE_SINGLE, ADC1_CHANNEL_4, ADC1_PRESSEL_FCPU_D18, //|  ADC1_CHANNEL_5 | ADC1_CHANNEL_6
            ADC1_EXTTRIG_TIM, DISABLE, ADC1_ALIGN_RIGHT, ADC1_SCHMITTTRIG_CHANNEL4,
            DISABLE);
  ADC1_Cmd(ENABLE);
  while (ADC1_GetFlagStatus(ADC1_FLAG_EOC) == 0)
    ;
  // Получение значения из АЦП
  ADC_POT = ADC1_GetConversionValue() >> 2;
  ADC1_Cmd(DISABLE);

  // чтение уставки с датчика воздуха
  /* De-Init ADC peripheral*/
  ADC1_DeInit();
  /* Init ADC2 peripheral */
  ADC1_Init(ADC1_CONVERSIONMODE_SINGLE, ADC1_CHANNEL_5, ADC1_PRESSEL_FCPU_D18, //|  ADC1_CHANNEL_5 | ADC1_CHANNEL_6
            ADC1_EXTTRIG_TIM, DISABLE, ADC1_ALIGN_RIGHT, ADC1_SCHMITTTRIG_CHANNEL5,
            DISABLE);
  ADC1_Cmd(ENABLE);
  while (ADC1_GetFlagStatus(ADC1_FLAG_EOC) == 0)
    ;
  // Получение значения из АЦП
  ADC_AIR = ADC1_GetConversionValue();
  ADC1_Cmd(DISABLE);

  // чтение уставки с датчика пола
  /* De-Init ADC peripheral*/
  ADC1_DeInit();
  /* Init ADC2 peripheral */
  ADC1_Init(ADC1_CONVERSIONMODE_SINGLE, ADC1_CHANNEL_6, ADC1_PRESSEL_FCPU_D18, //|  ADC1_CHANNEL_5 | ADC1_CHANNEL_6
            ADC1_EXTTRIG_TIM, DISABLE, ADC1_ALIGN_RIGHT, ADC1_SCHMITTTRIG_CHANNEL6,
            DISABLE);
  ADC1_Cmd(ENABLE);
  while (ADC1_GetFlagStatus(ADC1_FLAG_EOC) == 0)
    ;
  // Получение значения из АЦП
  ADC_FLOR = ADC1_GetConversionValue();
  ADC1_Cmd(DISABLE);
}

/********************************************
 *  функция обсчета температуры              *
 ********************************************/
int8_t Calculate_Temper_6_8(uint16_t Code)
{ // Для датчика 6,8 кОм
  int8_t Temper[37] = {-55, -50, -45, -40, -35, -30, -25, -20, -15, -10, -5, 0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 85, 90, 95, 100, 105, 110, 115, 120, 125};
  float Code_ADC[37] = {975.55, 955.59, 929.41, 896.02, 854.73, 805.59, 748.93, 686.46, 620.00, 552.34, 485.79, 422.81, 364.84, 313.08, 267.72, 228.80, 195.76, 168.05, 144.96, 125.84, 110.03, 96.972, 86.219, 77.312, 69.962, 63.863, 58.817, 54.586, 51.061, 48.105, 45.619, 43.521, 41.757, 40.255, 38.968, 37.864, 36.905};
  int8_t Res;
  unsigned char i, kr;
  float k, m;
  //  if(Code<172) kr=20;
  //  if(Code>3976)
  kr = 0;
  for (i = 0; i < 37; i++)
  {
    if (Code >= Code_ADC[i])
    {
      kr = i + 1;
      break;
    }
  }
  k = (float)(Temper[kr] - Temper[kr - 1]) / (Code_ADC[kr] - Code_ADC[kr - 1]);
  m = (float)((float)Temper[kr] - k * (float)Code_ADC[kr]);
  Res = (int8_t)(k * (float)Code + m);
  Sensor_err = 0;
  Sensor_overheat = 0;
  Sensor_no_err = 0;
  if (Code < 35)
  {
    Sensor_err = 1;
    Res = -127;
  }
  else if ((Code < 96) && (Code > 34))
  {
    Sensor_overheat = 1;
  }
  else if (Code > 990)
  {
    Sensor_err = 0;
    Sensor_no_err = 1;
    Res = -127; //,,,
  }
  return Res;
};

/***********************************************
 * Функция пересчета температуры с потенциометра*
 ***********************************************/
uint8_t Calculate_POT_temperature(uint8_t ADC)
{
  uint8_t Temp[29] = {5, 10, 15, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45};
  uint8_t Code_ADC[29] = {242, 234, 225, 216, 209, 200, 193, 184, 176, 168, 160, 151, 142, 133, 124, 116, 107, 98, 89, 81, 73, 65, 56, 49, 40, 32, 24, 16, 7};
  uint8_t Res;
  uint8_t i, kr;
  uint8_t k, m;
  for (i = 0; i < 29; i++)
  {
    if (ADC >= Code_ADC[i])
    {
      kr = i;
      break;
    }
  }
  k = (Temp[kr] - Temp[kr - 1]) / (Code_ADC[kr] - Code_ADC[kr - 1]);
  m = Temp[kr] - k * Code_ADC[kr];
  Res = (uint8_t)(k * ADC + m);
  return Res;
};

void SET_function(void)
{
  if ((!BUTTON) && (Mode == STENDBY))
  {
    if (Setting_temperature != iCounter_Power_Btn)
    {
      switch (Setting_temperature)
      {
      case 45:
        if ((Sensor_err == 1) || (Sensor_no_err == 1))
        {
          menuERR();
          break;
        }
        else
        {
          delay_ms(3000);
          iBlock = 45;
          Counter_Power_Btn = 1;
          iCounter_Power_Btn = Setting_temperature;
          break;
        };
        break;
      case 5:
        if ((Sensor_err == 1) || (Sensor_no_err == 1))
        {
          menuERR();
          break;
        }
        else
        {
          delay_ms(3000);
          iBlock = 5;
          Counter_Power_Btn = 1;
          iCounter_Power_Btn = Setting_temperature;
          break;
        };
        break;
      }
    }
    if (Counter_Power_Btn == 1)
    {
      delay_ms(1000);
      if ((Setting_temperature == 5) || (Setting_temperature == 45))
      {
        // Подтверждение входа в меню
        for (uint8_t i = 0; i < 5; i++)
        {
          lightsOFF();
          delay_ms(100);
          LED_RED_ON;
          LED_GREEN_ON;
          LED_BLUE_OFF;
          delay_ms(100);
        }
        // Вход в меню и настройка
        delay_ms(500);
        LED_RED_OFF;
        LED_GREEN_ON;
        LED_BLUE_OFF;
        // Выход по нажатию кнопки Питание
        for (uint16_t i = 0; i < 1400; i++)
        {
          if (BUTTON)
          {
            Counter_Power_Btn = 0;
            break;
          }
          read_ADC();
          if(iBlock != Calculate_POT_temperature(ADC_POT))
          {
            iBlock = 0;
          }
          delay_ms(10);
        }
        for (uint8_t i = 0; i < 5; i++)
        {
          lightsOFF();
          delay_ms(100);
          LED_RED_OFF;
          LED_GREEN_ON;
          LED_BLUE_OFF;
          delay_ms(100);
        }
        lightsOFF();

        if (iBlock == 0)
        {
          read_ADC();
          Counter_ADC = 3;
          Floor_Limit = Calculate_POT_temperature(ADC_POT);
          FLASH_Unlock(FLASH_MEMTYPE_DATA);
          Floor_Limit_state = Floor_Limit;
          Floor_Limit = Floor_Limit;
          FLASH_Lock(FLASH_MEMTYPE_DATA);
        }

        Counter_Power_Btn = 0;
        iCounter_Power_Btn = Setting_temperature;
        Setting_temperature = Setting_temperature;
      }
      else
      {
        Counter_Power_Btn = 0;
      }
    }
  }
};

void OVERHEAT(void)
{
  LED_GREEN_OFF;
  LED_BLUE_ON;
  LED_RED_OFF;
  delay_ms(200);
  LED_GREEN_ON;
  LED_BLUE_OFF;
  LED_RED_ON;
  delay_ms(200);
  LED_GREEN_OFF;
  LED_BLUE_OFF;
  LED_RED_ON;
  delay_ms(400);
  lightsOFF();
  delay_ms(100);
};

void lightsOFF(void)
{
  LED_RED_OFF;
  LED_GREEN_OFF;
  LED_BLUE_OFF;
};

void menuERR(void)
{
  delay_ms(3000);
  for (uint8_t i = 0; i < 5; i++)
  {
    LED_RED_ON;
    delay_ms(100);
    LED_RED_OFF;
    delay_ms(100);
  }
  Counter_Power_Btn = 0;
  iCounter_Power_Btn = Setting_temperature;
}
