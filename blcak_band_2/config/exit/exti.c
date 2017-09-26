#include "exti.h"
#include "led.h" 

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//外部中断 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/4
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 
uint8_t trainig_flag=0;
uint8_t testing_flag=0;



void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
 if(GPIO_Pin==key1_Pin)
 {
  HAL_Delay(10);
	if (HAL_GPIO_ReadPin(key1_GPIO_Port,key1_Pin)==1) //高电平
	{
		trainig_flag =1; 
		dur=-2000; 
		state=START;
		LED0=0;					    //先点亮红灯
	}
		__HAL_GPIO_EXTI_CLEAR_IT(key1_Pin);
 }
  else if (GPIO_Pin==key2_Pin)
	{
	 HAL_Delay(10);
	 if (HAL_GPIO_ReadPin(key2_GPIO_Port,key2_Pin)==0) //低电平
	 {
	 	testing_flag=1;
		dur=-2000; 
		state=START;
		LED0=1;
	 }
		 __HAL_GPIO_EXTI_CLEAR_IT(key2_Pin);
	}
 
}











