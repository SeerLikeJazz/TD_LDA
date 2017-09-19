#include "sys.h"
#include "usart3.h"
#include "usart.h"
#include "led.h"
#include "delay.h"
#include "emg.h"
#include "exti.h"
#include "key.h"
#include "sram.h"
//#include "config.h"
//#include "hc05.h"
//���ߣ�jeremyxu
u8 predict[1000]={0};
u8 motiondetect=0;
int main(void)
{

  u16 i =0;
//  u16 j =0;
//  u8 key;           //�����ֵ
  
  delay_init(168);      //��ʼ����ʱ����
  FSMC_SRAM_Init();	    //��ʼ��SRAM
  initiate_filter();
  delay_ms(1000);		//�ȴ�����ģ���ϵ��ȶ�
  LED_Init();				  //��ʼ��LED�˿�  
  //KEY_Init();			//������ʼ��
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
  EXTIX_Init();         //�ⲿ�ж�ʹ��
  usart3_init(460800);	//��ʼ������3Ϊ:460800,������. 
  uart_init(115200);	//���ڳ�ʼ��������Ϊ115200
  while(1)
  {
    if(trainig_flag ==1)
	{   
		   
		  //trainig_flag=0;
		if(data_full_flag==1)    
		{   data_full_flag=0;
		//data_full_flag=0; 
			if((count_temp+3)%50>=9)//count_temp=6-46, 56-96.............
			{ 
			  feature_extraction();
			  for(i=0;i<32;i++)
			  {
				feat_matrix[window_count][i]=feat[i];
				
			  }
			  window_count++;			  
			}							
		} 	  
       if(window_count==410&&dur==0)
		
		    LDA_train(feat_matrix);
	}
  if(testing_flag==1)
  {
	  
	  if(data_full_flag==1)    
	  {
		  data_full_flag=0;
	    feature_extraction();
		predict[count%1000]=LDA_predict(feat);	
		  motiondetect=predict[count%1000];
		  if(count>4&&predict[count%1000]==predict[(count-1)%1000]&&
		  predict[(count-1)%1000]==predict[(count-2)%1000]&&
	      predict[(count-2)%1000]==predict[(count-3)%1000]&&
	      predict[(count-3)%1000]==predict[(count-4)%1000]&&
	      predict[(count-4)%1000]==predict[(count-5)%1000])
	     {
		  switch(motiondetect)
		  {
			case 0:
			  USART_SendData(USART1, 'N');
			  break;
			case 1:
			  USART_SendData(USART1, 'P');
			  break;
			case 2:
			  USART_SendData(USART1, 'R');
			  break;
			case 3:
			  USART_SendData(USART1, 'T');
			  break;
			case 4:
			  USART_SendData(USART1, 'B');
			  break;  
			case 5:
			  USART_SendData(USART1, '0');
			  break;
		    default:			
		      break;
		  }
	    }
		  if(count ==409)
		  {   
			  
			  LED0 =0;//��ʾ���ݽ����ˣ��ȴ���һ�ΰ���
			  
		  }
		  
	  }	    	  	
  }
  
  
  }
}



