/**
******************************************************************************
* @file    Project/main.c
* @author  MCD Application Team
* @version V2.1.0
* @date    18-November-2011
* @brief   Main program body
******************************************************************************
* @attention
*
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
* TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
* DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
* FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
* CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"
#include "stm8s_flash.h"
#include "function.h"

/* Private defines -----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

uint8_t Reley_on_off = 0;
uint8_t CB;
uint8_t *But_time;
uint8_t flag_but = 0;

void main(void)
{
  init(); // ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½
  Relay_flag = 0;
  Counter_Power_Btn = 0;
  // èíèöèàëèçàöèÿ ïåðåìåííîé îãðàíè÷åíèÿ
  if (Floor_Limit_state != 0)
  {
    Floor_Limit = Floor_Limit_state;
  }
  else
  {
    FLASH_Unlock(FLASH_MEMTYPE_DATA);
    Floor_Limit_state = 28;
    Floor_Limit = 28;
    FLASH_Lock(FLASH_MEMTYPE_DATA);
  }

  RELAY_OFF;
  for (uint8_t i = 0; i < 10; i++)
  {
    // Äëÿ Óïîíîðà ñòàðòîâàÿ èíäèêàöèÿ âêëþ÷àåò ãîëóáóþ è ñèíþþ
    // Òàêèì îáðàçîì ìîæíî èäåíòèôèöèðîâàòü óñòðîéñòâà
    LED_RED_OFF;
    LED_GREEN_ON;
    LED_BLUE_ON;
    delay_ms(100);
    LED_RED_OFF;
    LED_GREEN_OFF;
    LED_BLUE_ON;
    delay_ms(100);
  }
  lightsOFF();
  delay_ms(1000);
  Mode = STENDBY; // CLASSIC NORMAL!
  for (uint8_t i = 0; i < 20; i++)
  {
    read_ADC();
  }
  Setting_temperature = Calculate_POT_temperature(ADC_POT);
  Air_temperature = Calculate_Temper_6_8(ADC_AIR) - 3;
  Flor_temperature = Calculate_Temper_6_8(ADC_FLOR);
  iCounter_Power_Btn = Setting_temperature;
  Counter_ADC = 5;
  if (BUTTON && (flag_previos_state == 0x53))
  {
    Mode = ERROR_MODE;
  }
  /* Infinite loop */
  for (;;)
  {
    if (!Counter_ADC)
    {
      read_ADC();
      Setting_temperature = Calculate_POT_temperature(ADC_POT);
      Air_temperature = Calculate_Temper_6_8(ADC_AIR) - 3;
      Flor_temperature = Calculate_Temper_6_8(ADC_FLOR);
      Counter_ADC = 3;
    }
    switch (Mode)
    {
    case STENDBY:
      lightsOFF();
      Relay_flag = 0;
      if (BUTTON)
      {
        if (Sensor_err)
        {
          Mode = AIR_MODE;
          // *** [DELETE ERROR INDICATION] ***
          // for(uint8_t i=0; i<6; i++){
          //   LED_RED_ON;
          //   delay_ms(100);
          //   LED_RED_OFF;
          //   delay_ms(100);
          // }
          // LED_GREEN_ON;
          // delay_ms(500);
        }
        else if (Sensor_no_err)
        {
          Mode = AIR_MODE;
          // LED_GREEN_ON;
          // delay_ms(500);
        }
        else if (Sensor_overheat)
        {
          Relay_flag = 0;
          OVERHEAT();
        }
        else
        {
          Mode = AIR_FLOOR_MODE;
          LED_GREEN_ON;
          delay_ms(500);
        }
      }
      break;
    case AIR_MODE:
      if (BUTTON)
      {
        if (Sensor_overheat)
        {
          Relay_flag = 0;
          OVERHEAT();
        }
        else
        {
          if (Air_temperature < Setting_temperature)
          {
            LED_RED_ON;
            LED_GREEN_OFF;
            LED_BLUE_OFF;
            Relay_flag = 1;
            iCounter_Power_Btn = Setting_temperature;
          }
          if (Air_temperature > Setting_temperature)
          {
            LED_RED_OFF;
            LED_GREEN_OFF;
            LED_BLUE_ON;
            Relay_flag = 0;
            iCounter_Power_Btn = Setting_temperature;
          }
        }
      }
      else if (!BUTTON)
      {
        lightsOFF();
        Relay_flag = 0;
        Mode = STENDBY;
      }
      break;
    case AIR_FLOOR_MODE:
      if (BUTTON)
      {
        if (Sensor_err)
        {
          Mode = ERROR_MODE;
          LED_RED_ON;
          LED_GREEN_OFF;
          LED_BLUE_OFF;
          Relay_flag = 0;
        }
        else if (Sensor_overheat)
        {
          Relay_flag = 0;
          OVERHEAT();
        }
        else if (Sensor_no_err)
        {
          // Mode = STENDBY;
          delay_ms(500);
          Mode = AIR_MODE;
        }
        else
        {
          if ((Flor_temperature < Floor_Limit) && (Air_temperature < Setting_temperature))
          {
            LED_RED_ON;
            LED_GREEN_ON;
            LED_BLUE_OFF;
            Relay_flag = 1;
            iCounter_Power_Btn = Setting_temperature;
          }
          if ((Flor_temperature > Floor_Limit) || (Air_temperature > Setting_temperature))
          {
            LED_RED_OFF;
            LED_GREEN_ON;
            LED_BLUE_OFF;
            Relay_flag = 0;
            iCounter_Power_Btn = Setting_temperature;
          }
        }
      }
      else if (!BUTTON)
      {
        lightsOFF();
        Relay_flag = 0;
        Mode = STENDBY;
      }
      break;
    case ERROR_MODE:
      if (BUTTON)
      {
        Relay_flag = 0;
        LED_RED_ON;
        LED_GREEN_OFF;
        LED_BLUE_OFF;
        delay_ms(100);
        LED_RED_OFF;
        delay_ms(100);
        FLASH_Unlock(FLASH_MEMTYPE_DATA);
        flag_previos_state = 0x53;
        Floor_Limit_state = 28;
        FLASH_Lock(FLASH_MEMTYPE_DATA);
      }
      else if (!BUTTON)
      {
        lightsOFF();
        Relay_flag = 0;
        Mode = STENDBY;
        FLASH_Unlock(FLASH_MEMTYPE_DATA);
        flag_previos_state = 0;
        FLASH_Lock(FLASH_MEMTYPE_DATA);
      }
      break;
    }
    SET_function();
    if (Relay_flag)
    {
      delay_ms(250);
      RELAY_ON;
    }
    else
    {
      RELAY_OFF;
    }
  }
}

#ifdef USE_FULL_ASSERT

/**
 * @brief  Reports the name of the source file and the source line number
 *   where the assert_param error has occurred.
 * @param file: pointer to the source file name
 * @param line: assert_param error line source number
 * @retval : None
 */
void assert_failed(u8 *file, u32 line)
{
  /* User can add his own implementation to report the file name and line number,
  ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
