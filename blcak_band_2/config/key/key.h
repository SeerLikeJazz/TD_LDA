#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//����������������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/3
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	 

/*����ķ�ʽ��ͨ��ֱ�Ӳ����⺯����ʽ��ȡIO*/

#define KEY1 		HAL_GPIO_ReadPin(GPIOE,GPIO_Pin_3)	//PE3 

#define WK_UP 	HAL_GPIO_ReadPin(GPIOA,GPIO_Pin_0)	//PA0





#define KEY1_PRES	2

#define WKUP_PRES   4

extern uint8_t KEY_Scan(uint8_t mode);

#endif

