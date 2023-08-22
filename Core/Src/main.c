/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "include.h"
#include "main.h"
#include "dma.h"
#include "gpio.h"


void SystemClock_Config(void);



/*==================================================================================================
Name:
Function:
Input:
Output:
==================================================================================================*/
extern uIB2_Response Responsedata;
extern sIB2D_InitStruct IB2D_InitStruct;
extern uIB2_RequiredResources APP_RequiredResources;
extern uint8_t  ChannelUpdateFlg;
signed short Channeldata1[SES_MAX_CHANNELS_LENGTH];
signed short APP_ChannelsUs[SES_MAX_CHANNELS_LENGTH];

/*==================================================================================================
Name:
Function:
Input:
Output:
==================================================================================================*/
int main(void)
{ 
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_DMA_Init();;
    SystemConfig();
    Dev_Ibus_Init();
    SYS_StartWatchdog();
    HAL_Delay(200);
    while (1)
    {
        if( ChannelUpdateFlg &&  (!APP_RequiredResources.Types.ChannelsTypes) )
        {
            SES_UnpackChannels(IB2D_InitStruct.pChannelsPacked, Channeldata1,  SES_MAX_CHANNELS_LENGTH, IB2D_InitStruct.pChannelsType );
            ChannelUpdateFlg = 0;
            
            for (uint32_t i=0;i<SES_MAX_CHANNELS_LENGTH;i++)
            {
                int32_t ChannelValue;

                if (Channeldata1[i]!=SES_KEEP_FAILSAFE_CHANNEL && Channeldata1[i]!=SES_STOP_FAILSAFE_CHANNEL)
                {
                    ChannelValue=((Channeldata1[i]+49152)*8000+(1<<15))>>16;
                    
                    if (ChannelValue<APP_CHANNEL_US_VALUE_MIN*4)
                        ChannelValue=APP_CHANNEL_US_VALUE_MIN*4;
                    if (ChannelValue>APP_CHANNEL_US_VALUE_MAX*4)
                        ChannelValue=APP_CHANNEL_US_VALUE_MAX*4;
                    APP_ChannelsUs[i]=ChannelValue/4;   //APP_ChannelsUsµÄ·¶Î§¾ÍÊÇ750-1500-2250
                }
            }
        }     
    }
}

/*==================================================================================================
Name:
Function:
Input:
Output:
==================================================================================================*/
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
  RCC_OscInitStruct.PLL.PLLN = 8;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
