/**
  ******************************************************************************
  * File Name          : SPI.c
  * Description        : This file provides code for the configuration
  *                      of the SPI instances.
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
#include "spi.h"

#include "gpio.h"

/* USER CODE BEGIN 0 */
__IO uint32_t DeviceID = 0;
__IO uint32_t FlashID = 0;

uint8_t SAVE_LDA_TEMP[800]={0};
uint8_t dtest[800]={0};
float32_t extract_LDA_TEMP[192]={0};

uint8_t SAVE_Bias_Mat[24]={0};
uint8_t etest[24]={0};
float32_t extract_Bias_Mat[6]={0};
/* USER CODE END 0 */

SPI_HandleTypeDef hspi1;

/* SPI1 init function */
void MX_SPI1_Init(void)
{

  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

void HAL_SPI_MspInit(SPI_HandleTypeDef* spiHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(spiHandle->Instance==SPI1)
  {
  /* USER CODE BEGIN SPI1_MspInit 0 */

  /* USER CODE END SPI1_MspInit 0 */
    /* SPI1 clock enable */
    __HAL_RCC_SPI1_CLK_ENABLE();
  
    /**SPI1 GPIO Configuration    
    PA5     ------> SPI1_SCK
    PA6     ------> SPI1_MISO
    PA7     ------> SPI1_MOSI 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN SPI1_MspInit 1 */

  /* USER CODE END SPI1_MspInit 1 */
  }
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* spiHandle)
{

  if(spiHandle->Instance==SPI1)
  {
  /* USER CODE BEGIN SPI1_MspDeInit 0 */

  /* USER CODE END SPI1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_SPI1_CLK_DISABLE();
  
    /**SPI1 GPIO Configuration    
    PA5     ------> SPI1_SCK
    PA6     ------> SPI1_MISO
    PA7     ------> SPI1_MOSI 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7);

  /* USER CODE BEGIN SPI1_MspDeInit 1 */

  /* USER CODE END SPI1_MspDeInit 1 */
  }
} 

/* USER CODE BEGIN 1 */
void Extract_from_flash(void)
{
	SPI_FLASH_SendByte(0xff);
	DeviceID = SPI_FLASH_ReadDeviceID();
	FlashID = SPI_FLASH_ReadID();
	printf("%x,%x\r\n",DeviceID,FlashID);
	
	/* Check the SPI Flash ID */
	if (FlashID == SPI_FLASH_ID)  /* #define  sFLASH_ID  0XEF4018 */
	{							
		/* 擦除SPI的扇区以写入 */
//		SPI_FLASH_SectorErase(FLASH_SectorToErase);	 	 						
		/* 将发送缓冲区的数据写到flash中 */ 	
//		SPI_FLASH_BufferWrite(SAVE_LDA_TEMP, FLASH_WriteAddress, 768);					
		/* 将刚刚写入的数据读出来放到接收缓冲区中 */
		SPI_FLASH_BufferRead(dtest, FLASH_ReadAddress, 768);
		
//		SPI_FLASH_BufferWrite(SAVE_Bias_Mat, 0xf00, 24);					
		/* 将刚刚写入的数据读出来放到接收缓冲区中 */
		SPI_FLASH_BufferRead(etest, 0xf00, 24);						
	}
	else
	{    
		printf("读取模型失败!\n");
	}
	char *pa=dtest;
	char *pb=etest;					
	for(int temp_i=0;temp_i<192;temp_i++)
		{
		 extract_LDA_TEMP[temp_i]=*((float *)pa+temp_i);
		 printf("%f\r\n",extract_LDA_TEMP[temp_i]);
		}
	//extract_LDA_TEMP[192]转化为LDA_TEMP[32][6];	
	for(int i=0,count=0;i<32;i++)
	{
		for(int j=0;j<6;j++)
		{
	       LDA_TEMP[i][j]=extract_LDA_TEMP[count++];
		}
	}	
		
	for(int temp_i=0;temp_i<6;temp_i++)
		{
		 Bias_Mat[temp_i]=*((float *)pb+temp_i);
		 printf("%f\r\n",Bias_Mat[temp_i]);
		}
}

void Save_to_flash(void)
{
/********************开始存数据********************************************************/
 char *pc=(char*)LDA_TEMP;
 char *pd=(char*)Bias_Mat;
 for(int temp_i=0;temp_i<(32*4*6);temp_i++) 
	{
			SAVE_LDA_TEMP[temp_i]=*(pc+temp_i);
	}
 for(int temp_i=0;temp_i<(6*4);temp_i++) 
	{
			SAVE_Bias_Mat[temp_i]=*(pd+temp_i);
	}
	/* Check the SPI Flash ID */
	if (FlashID == SPI_FLASH_ID)  /* #define  sFLASH_ID  0XEF4018 */
	{							
		/* 擦除SPI的扇区以写入 */
		SPI_FLASH_SectorErase(FLASH_SectorToErase);	 	 						
		/* 将发送缓冲区的数据写到flash中 */ 	
		SPI_FLASH_BufferWrite(SAVE_LDA_TEMP, FLASH_WriteAddress, 768);					
		/* 将刚刚写入的数据读出来放到接收缓冲区中 */
//						SPI_FLASH_BufferRead(dtest, FLASH_ReadAddress, 768);
		/* 将发送缓冲区的数据写到flash中 */ 							
		SPI_FLASH_BufferWrite(SAVE_Bias_Mat, 0xf00, 24);					
		/* 将刚刚写入的数据读出来放到接收缓冲区中 */
//						SPI_FLASH_BufferRead(etest, 0xf00, 24);						
	}
	else
	{    
		printf("获取不到 W25Q128 ID!\n");
	}
//					pc=dtest;
//          pd=etest;					
//					for(int temp_i=0;temp_i<192;temp_i++)
//						{
//						 extract_LDA_TEMP[temp_i]=*((float *)pc+temp_i);
//						 printf("%f\r\n",extract_LDA_TEMP[temp_i]);
//						}
//					for(int temp_i=0;temp_i<6;temp_i++)
//						{
//						 extract_Bias_Mat[temp_i]=*((float *)pd+temp_i);
//						 printf("%f\r\n",extract_Bias_Mat[temp_i]);
//						}
 /********************结束存数据********************************************************/
}

/* USER CODE END 1 */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
