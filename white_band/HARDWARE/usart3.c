#include "usart3.h"
#include "exti.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"	
#include "config.h"
#include "led.h" 
#ifdef USART3_RX_EN   								//如果使能了接收   	  
u8  data_cnt_state=0;//串口数据当前状态，用来判断起始位，数据位
u8  data_cnt_max=16;
u16 data_cnt_check=0;  //校验位
u8  data_cnt_count=0;//数据计数
double data_buff[8];//临时数据缓冲区
double data[600][8];//存储的EMG数据
s16 data_index=-1;
u8  data_full_flag=0;
u8  first_flag=0;
u8  kill_flag=1;
u16 count_temp=0;
/////////////
u8					filter_n;
double				data_src[21][8];
double				data_filter[21][8];
double				filter_a[21];
double				filter_b[21];
s8                  filter_index;
//////////////
void initiate_filter(void)
{  u8 i,j;
   filter_index=-1;
   for(i=0;i<21;i++)
   {
   	for(j=0;j<8;j++)
   	{
   		data_src[i][j]=0;
   		data_filter[i][j]=0;
   	}
   }
   for(i=0;i<21;i++)
   {
   	filter_a[i]=0;
   	filter_b[i]=0;
   }
   filter_n=20;        //1000Hz采样率下陷波器参数
   filter_a[0]=1.000000000000000;
   filter_a[20]=-0.809784033195007;
   filter_b[0]=0.904892016597504;
   filter_b[20]=-0.904892016597504;
}
double Filter(double temp)
{
        u8 i;
		filter_index=(filter_index+1)%8;    //LB// filter_index=-1;
		for(i=0;i<filter_n;i++)         //LB// filter_n=20;
		{
			data_src[filter_n-i][filter_index]=data_src[filter_n-i-1][filter_index];
			data_filter[filter_n-i][filter_index]=data_filter[filter_n-i-1][filter_index];
		}
		data_src[0][filter_index]=temp;
		data_filter[0][filter_index]=0;
		for(i=0;i<filter_n+1;i++)
			data_filter[0][filter_index]+=filter_b[i]*data_src[i][filter_index];
		for(i=1;i<filter_n+1;i++)
			data_filter[0][filter_index]-=filter_a[i]*data_filter[i][filter_index];
		return data_filter[0][filter_index];
	
}
void SetData(double *p)
{   
	if(dur>0)
	{   
		u8 j;
		u16 index_temp=(data_index+1);
		dur--;
		data_index++;
		for (j=0;j<DATA_CHANNEL;j++)
		{
		data[index_temp][j]=*p;//LZX//<把data_buff[8]临时数据缓冲区的数据存到data[600][8]>
		p++;		
		}  
		if(dur<50000&&dur%5000==0)//LZX//<训练和休息，line99,100,>
		{
		  LED0=!LED0;
		}
		if(first_flag==1)//first_flag使得增量窗执行在第一个TIME_INTERVAL后
		{  
			if((index_temp+1)%TIME_INCREMENT==0)
			{  
			count_temp++;     //保证每次执行特征提取函数时都是当前窗的数据
			data_full_flag=1;//数据满个TIME_INTERVAL300个，即一个滑动窗，以便主程序开始特征提取
			if(index_temp==599)
				data_index=-1;//将后续的数据从头开始存储
			}	   
		}
		if((index_temp>=TIME_INTERVAL-1)&((index_temp+1)%TIME_INCREMENT==0)&(kill_flag==1))//kill_flag的作用是只执行一次if后的语句，第一个窗
		{ 
		first_flag=1;kill_flag=0;
		data_full_flag=1;//数据满300个，即一个滑动窗，以便主程序开始特征提取
		}	 
		if(dur==0&&testing_flag==1)
       dur=DATA_LENGTH;		 
	}
	if(dur<0)//LZX//<只执行一次>
	{
	   dur++;
		if(dur==0&&state==START )
			dur=DATA_LENGTH;	
	}	
}

void USART3_IRQHandler(void)
{
	u8 res;	    
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)//接收到数据
	{	 
        
	    res =USART_ReceiveData(USART3);	
        switch ( data_cnt_state )
		{
			case 0:
				if (res == 0x55)
					data_cnt_state++;
				break;
			case 1:
				if (res == 0xAA)
				{
				  data_cnt_state++;
				  data_cnt_count = 0;
				  data_cnt_check=0;
				}
				else data_cnt_state=0;
				break;
			case 2:
				data_cnt_check+=res;
			    if (data_cnt_count%2 ==0)
					data_buff[data_cnt_count %(DATA_CHANNEL*2)/2]= res*256;//LZX//<移位更高效>
				else
				{
				  data_buff[data_cnt_count %(DATA_CHANNEL*2)/2]+= res;
			      if(data_buff[data_cnt_count%(DATA_CHANNEL*2)/2]>=65534/2)   
						data_buff[data_cnt_count%(DATA_CHANNEL*2)/2]-=65536;//将负数的补码转换成原码
				        data_buff[data_cnt_count %(DATA_CHANNEL*2)/2]-=1862;//LZX//<为什么要减>
				        data_buff[data_cnt_count %(DATA_CHANNEL*2)/2]=Filter(data_buff[data_cnt_count %(DATA_CHANNEL*2)/2]);//LZX//<滤波>
				}
				data_cnt_count++;
				if(data_cnt_count==data_cnt_max)
					data_cnt_state++;
				break;
			case 3:
				if(data_cnt_check%256==res)
				{
				     SetData(data_buff);
				}
				data_cnt_state=0;
				break; 
		
		}	 
   
	}										 
}  
#endif	

//初始化IO 串口3
//bound:波特率	  
void usart3_init(u32 bound)
{  
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
 
	USART_DeInit(USART3);  //复位串口3
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE); //使能GPIOB时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);//使能USART3时钟
	
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_10; //GPIOB11和GPIOB10初始化
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOB,&GPIO_InitStructure); //初始化GPIOB11，和GPIOB10
	
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3); //GPIOB11复用为USART3
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3); //GPIOB10复用为USART3	  
	
	USART_InitStructure.USART_BaudRate = bound;//波特率一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART3, &USART_InitStructure); //初始化串口3
	
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启中断  
		
	USART_Cmd(USART3, ENABLE);                    //使能串口 
	
 
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;//抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	

}


 
 



