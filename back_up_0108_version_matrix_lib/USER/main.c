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
//作者：jeremyxu
u8 predict[1000]={0};
u8 motiondetect=0;
int main(void)
{

  u16 i =0;
//  u16 j =0;
//  u8 key;           //保存键值
  
  delay_init(168);      //初始化延时函数
  FSMC_SRAM_Init();	    //初始化SRAM
  initiate_filter();
  delay_ms(1000);		//等待蓝牙模块上电稳定
  LED_Init();				  //初始化LED端口  
  //KEY_Init();			//按键初始化
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
  EXTIX_Init();         //外部中断使能
  usart3_init(460800);	//初始化串口3为:460800,波特率. 
  uart_init(115200);	//串口初始化波特率为115200
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
			  
			  LED0 =0;//提示数据结束了，等待下一次按键
			  
		  }
		  
	  }	    	  	
  }
  
  
  }
}



