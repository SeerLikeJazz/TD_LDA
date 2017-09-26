#ifndef __USART3_H
#define __USART3_H	 
#include "sys.h"  

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//串口3驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2014/3/29
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	   


#define USART3_RX_EN 			1					//0,不接收;1,接收.
extern u8  data_full_flag;
extern double data[600][8];
extern u16 count_temp;
void usart3_init(u32 bound);				//串口3初始化 
void u3_printf(char* fmt, ...);
void SetData(double *p);
void initiate_filter(void);
double Filter(double temp);

//void TIM7_Int_Init(u16 arr,u16 psc);
//void u3_printf(char* fmt, ...);
#endif













