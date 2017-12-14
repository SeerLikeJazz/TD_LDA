#include "exti.h"
#include "usart.h"


#define Key_Value  HAL_GPIO_ReadPin(key2_GPIO_Port,key2_Pin)							  
////////////////////////////////////////////////////////////////////////////////// 
uint8_t trainig_flag=0;
uint8_t testing_flag=0;

uint8_t key_fall_flag =0;
uint8_t keyUpFlag=0;
uint16_t key_holdon_ms=0;
uint8_t short_key_flag=0;
uint8_t key_long_down=0;
uint8_t long_key_flag=0;
uint16_t keyupCnt=0;



//连续按下2s以上，进入training  0：正常按下一次，进入testing


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if(Key_Value == 0)
	{
	  key_fall_flag = 1;//生成按键按下标志
	}
	__HAL_GPIO_EXTI_CLEAR_IT(key2_Pin);
 
}

void HAL_SYSTICK_Callback(void)
{
	if(key_fall_flag==1)//发生按键按下事件
	{
			if(Key_Value==0)//按键持续按下
			 {         
					if(key_holdon_ms <= 2000)  
					 {
							key_holdon_ms++; 
					 } 
					 else //按键按下到2000ms就判断长按时间成立，生成长按标志
					 { 
						 key_holdon_ms = 0; 
						 short_key_flag=0;//清短按键标志
						 key_long_down = 1;//长按键标志置位
						 key_fall_flag = 0;//清按键按下标志
						} 
			 } 
		else //按键抬起
		{ 
				 if(key_holdon_ms>50)//按键时间大于50ms，生成单击标志
					{  
						 key_holdon_ms=0;
						 short_key_flag=1;
						 key_long_down =0;
						 key_fall_flag=0;
						
						 keyUpFlag = 1;//单击抬起按键后，生成按键抬起标志

					} 
				 else  //按键持续时间小于50ms,忽略
						{    
								 key_holdon_ms=0; 
								 short_key_flag=0;
								 long_key_flag=0;
									key_fall_flag=0;
					 } 
		}

	}
	if(keyUpFlag)//单击抬起后，启动计数，计数到500ms
		 keyupCnt++;
  if(keyupCnt>500)
	{ 
		 keyupCnt = 0;
		 keyUpFlag = 0;
	}
}

void Key_Func(void)
{
 if(short_key_flag)//testing
 {
  short_key_flag = 0;
	 {
		testing_flag=1;
		trainig_flag=0;
		dur=-2000; 
		state=START;
		LED6=1;	
		LED7=1;
	 }
 }
 if(key_long_down)//training
 {
  key_long_down = 0;
	 {
		trainig_flag =1; 
		testing_flag=0;
		dur=-2000; 
		state=START;
		LED6=0;					    //先点亮红灯
		LED7=1;
	 }
	window_count = 0;//初始化，不然training主程序里面不更新
	LED1=1;LED2=1;LED3=1;LED4=1;LED5=1;//training全熄灭
	LED1=0;
//	LED1=0;LED2=0;LED3=0;LED4=0;LED5=0;//training全亮
	uint8_t a=2;//震动反馈
	HAL_UART_Transmit(&huart3, &a, 1,0xffff);
 }

}




/******************* (C) COPYRIGHT 2015-2020 Econ-Tech *****END OF FILE****/









