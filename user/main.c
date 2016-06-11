/**
  ******************************************************************************
  * @file    Project/STM32F37x_StdPeriph_Templates/main.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    20-September-2012
  * @brief   Main program body
  ******************************************************************************
 
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "ads1298.h"
#include "usart.h"
#include "yei3space.h"
#include "stm32f37x.h"
#include "schedule.h"
#include "motor.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
//    int i;
    u8 deviceID;
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    
    systick_init();
    NVIC_SetPriority(SysTick_IRQn, 0x0);
    USART2_Init(921600);
    
    ads1298_init();
    //motor_init();
//    resetADS1298();
//    GPIO_ResetBits(GPIOE,GPIO_Pin_8);
//    if (shakeHands()==0x92)
//        printf("ADS1298 started successfully\n"); 
//    else
//        printf("ADS1298 started failed\n"); 
    
    while (1)
    {
        //motorshake(0xff,100,100,10);
        //delayMs(1000);
        //USART2_SendByte(0x88);
        //delayMs(100);
    }    
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
