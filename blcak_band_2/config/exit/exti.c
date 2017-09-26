#include "exti.h"
#include "led.h" 

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//�ⲿ�ж� ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/4
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 
uint8_t trainig_flag=0;
uint8_t testing_flag=0;



void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
 if(GPIO_Pin==key1_Pin)
 {
  HAL_Delay(10);
	if (HAL_GPIO_ReadPin(key1_GPIO_Port,key1_Pin)==1) //�ߵ�ƽ
	{
		trainig_flag =1; 
		dur=-2000; 
		state=START;
		LED0=0;					    //�ȵ������
	}
		__HAL_GPIO_EXTI_CLEAR_IT(key1_Pin);
 }
  else if (GPIO_Pin==key2_Pin)
	{
	 HAL_Delay(10);
	 if (HAL_GPIO_ReadPin(key2_GPIO_Port,key2_Pin)==0) //�͵�ƽ
	 {
	 	testing_flag=1;
		dur=-2000; 
		state=START;
		LED0=1;
	 }
		 __HAL_GPIO_EXTI_CLEAR_IT(key2_Pin);
	}
 
}











