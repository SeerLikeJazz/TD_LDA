/**
  ******************************************************************************
  * File Name          : USART.c
  * Description        : This file provides code for the configuration
  *                      of the USART instances.
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2017 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usart.h"

#include "gpio.h"
#include "dma.h"

/* USER CODE BEGIN 0 */


uint8_t recv_buff[1000]={0};//DMA接收数组
uint16_t recv_buff_index=0;

uint8_t DMA_recv_complete_flag=0;
uint8_t  data_cnt_state=0;//串口数据当前状态，用来判断起始位，数据位
uint8_t  data_cnt_max=12;
uint16_t data_cnt_check=0;  //校验位
uint8_t  data_cnt_count=0;//数据计数
double data_buff[8];//临时数据缓冲区
double data[600][8];//存储的EMG数据
uint8_t temp_from_recv_buff[12]={0};
int16_t data_index=-1;
uint8_t  data_full_flag=0;
uint8_t  first_flag=0;
uint8_t  kill_flag=1;
uint16_t count_temp=0;

///////滤波器参数//////
uint8_t			  filter_n;
double				data_src[21][8];
double				data_filter[21][8];
double				filter_a[21];
double				filter_b[21];
int8_t        filter_index;
//////////////
/* USER CODE END 0 */

UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart3_rx;

/* USART3 init function */

void MX_USART3_UART_Init(void)
{

  huart3.Instance = USART3;
  huart3.Init.BaudRate = 460800;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(uartHandle->Instance==USART3)
  {
  /* USER CODE BEGIN USART3_MspInit 0 */

  /* USER CODE END USART3_MspInit 0 */
    /* USART3 clock enable */
    __HAL_RCC_USART3_CLK_ENABLE();
  
    /**USART3 GPIO Configuration    
    PB10     ------> USART3_TX
    PB11     ------> USART3_RX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* USART3 DMA Init */
    /* USART3_RX Init */
    hdma_usart3_rx.Instance = DMA1_Stream1;
    hdma_usart3_rx.Init.Channel = DMA_CHANNEL_4;
    hdma_usart3_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart3_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart3_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart3_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart3_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart3_rx.Init.Mode = DMA_NORMAL;
    hdma_usart3_rx.Init.Priority = DMA_PRIORITY_HIGH;
    hdma_usart3_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_usart3_rx) != HAL_OK)
    {
      _Error_Handler(__FILE__, __LINE__);
    }

    __HAL_LINKDMA(uartHandle,hdmarx,hdma_usart3_rx);

    /* USART3 interrupt Init */
    HAL_NVIC_SetPriority(USART3_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(USART3_IRQn);
  /* USER CODE BEGIN USART3_MspInit 1 */

  /* USER CODE END USART3_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART3)
  {
  /* USER CODE BEGIN USART3_MspDeInit 0 */

  /* USER CODE END USART3_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART3_CLK_DISABLE();
  
    /**USART3 GPIO Configuration    
    PB10     ------> USART3_TX
    PB11     ------> USART3_RX 
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_10|GPIO_PIN_11);

    /* USART3 DMA DeInit */
    HAL_DMA_DeInit(uartHandle->hdmarx);

    /* USART3 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART3_IRQn);
  /* USER CODE BEGIN USART3_MspDeInit 1 */

  /* USER CODE END USART3_MspDeInit 1 */
  }
} 

/* USER CODE BEGIN 1 */
//DMA接收完成中断
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
DMA_recv_complete_flag = 1;

}

//滤波器初始化
void initiate_filter(void)
{  uint8_t i,j;
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

//滤波
double Filter(double temp)
{
    uint8_t i;
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

//解码
unsigned char decode(unsigned char cryptograph)
{
	unsigned char tempFir = 0;
	unsigned char tempSec = 0;
	tempFir = cryptograph << 4;
	tempSec = cryptograph >> 4;
	cryptograph = tempFir|tempSec;
	tempFir=~cryptograph&0x55;
	tempSec=cryptograph&0xAA;
	cryptograph=tempFir|tempSec;

	return cryptograph;
}

//把dma收到的数据提取到临时数组data_buff[8]
void deal_with_recv_buff(void)
{
	for(recv_buff_index = 0;recv_buff_index<recv_buff_index_max;recv_buff_index++)
	{
        switch ( data_cnt_state )
		{
			case 0:
				if (recv_buff[recv_buff_index] == 0x00)
					data_cnt_state++;
				break;
			case 1:
				if (recv_buff[recv_buff_index] == 0xff)
				{
				  data_cnt_state++;
				  data_cnt_count = 0;
				  data_cnt_check=0;
				}
				else data_cnt_state=0;
				break;
			case 2:
			  temp_from_recv_buff[data_cnt_count] = decode(recv_buff[recv_buff_index]);
			  data_cnt_check+=temp_from_recv_buff[data_cnt_count];							
				data_cnt_count++;
				if(data_cnt_count==data_cnt_max)
					data_cnt_state++;
				break;
			case 3:
				if(data_cnt_check%256==decode(recv_buff[recv_buff_index]))
				{

					for(uint8_t i=0;i<data_cnt_max;i++)
					{
								if (i%3 == 0)
								{
								data_buff[i % data_cnt_max/3*2]= temp_from_recv_buff[i]*16;//LZX//<移位更高效,过会儿优化一下>
								}
								else if(i%3 == 1)
								{				
								data_buff[i % data_cnt_max/3*2] += (temp_from_recv_buff[i]>>4);
								data_buff[i % data_cnt_max/3*2] = Filter(data_buff[i % data_cnt_max/3*2]);
								data_buff[i % data_cnt_max/3*2 + 1] = ((temp_from_recv_buff[i]&0x0F)*256);
								}
								else
								{
								data_buff[i % data_cnt_max/3*2 + 1] += (temp_from_recv_buff[i]);
								data_buff[i % data_cnt_max/3*2 + 1] = Filter(data_buff[i % data_cnt_max/3*2 + 1]);				
								}
					}
					SetData(data_buff);
				}
				data_cnt_state=0;
				break; 
		
		}	

	}
}

//有按键按下，进入此函数
void SetData(double *p)
{   
	if(dur>0)
	{   
		uint8_t j;
		uint16_t index_temp=(data_index+1);
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

/* USER CODE END 1 */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
