#ifndef __USART3_H
#define __USART3_H	 
#include "sys.h"  

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//����3��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2014/3/29
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	   


#define USART3_RX_EN 			1					//0,������;1,����.
extern u8  data_full_flag;
extern double data[600][8];
extern u16 count_temp;
void usart3_init(u32 bound);				//����3��ʼ�� 
void u3_printf(char* fmt, ...);
void SetData(double *p);
void initiate_filter(void);
double Filter(double temp);

//void TIM7_Int_Init(u16 arr,u16 psc);
//void u3_printf(char* fmt, ...);
#endif













