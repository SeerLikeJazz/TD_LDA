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



//��������2s���ϣ�����training  0����������һ�Σ�����testing


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if(Key_Value == 0)
	{
	  key_fall_flag = 1;//���ɰ������±�־
	}
	__HAL_GPIO_EXTI_CLEAR_IT(key2_Pin);
 
}

void HAL_SYSTICK_Callback(void)
{
	if(key_fall_flag==1)//�������������¼�
	{
			if(Key_Value==0)//������������
			 {         
					if(key_holdon_ms <= 2000)  
					 {
							key_holdon_ms++; 
					 } 
					 else //�������µ�2000ms���жϳ���ʱ����������ɳ�����־
					 { 
						 key_holdon_ms = 0; 
						 short_key_flag=0;//��̰�����־
						 key_long_down = 1;//��������־��λ
						 key_fall_flag = 0;//�尴�����±�־
						} 
			 } 
		else //����̧��
		{ 
				 if(key_holdon_ms>50)//����ʱ�����50ms�����ɵ�����־
					{  
						 key_holdon_ms=0;
						 short_key_flag=1;
						 key_long_down =0;
						 key_fall_flag=0;
						
						 keyUpFlag = 1;//����̧�𰴼������ɰ���̧���־

					} 
				 else  //��������ʱ��С��50ms,����
						{    
								 key_holdon_ms=0; 
								 short_key_flag=0;
								 long_key_flag=0;
									key_fall_flag=0;
					 } 
		}

	}
	if(keyUpFlag)//����̧�������������������500ms
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
		LED6=0;					    //�ȵ������
		LED7=1;
	 }
	window_count = 0;//��ʼ������Ȼtraining���������治����
	LED1=1;LED2=1;LED3=1;LED4=1;LED5=1;//trainingȫϨ��
	LED1=0;
//	LED1=0;LED2=0;LED3=0;LED4=0;LED5=0;//trainingȫ��
	uint8_t a=2;//�𶯷���
	HAL_UART_Transmit(&huart3, &a, 1,0xffff);
 }

}




/******************* (C) COPYRIGHT 2015-2020 Econ-Tech *****END OF FILE****/









