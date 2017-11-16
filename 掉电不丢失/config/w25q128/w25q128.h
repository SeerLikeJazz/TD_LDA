#ifndef __W25Q128_H
#define __W25Q128_H

/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "spi.h"

/* 私有宏定义 ----------------------------------------------------------------*/
#define SPI_FLASH_PageSize              256
#define SPI_FLASH_PerWritePageSize      256
#define W25X_WriteEnable		            0x06 
#define W25X_WriteDisable		            0x04 
#define W25X_ReadStatusReg		          0x05 
#define W25X_WriteStatusReg		          0x01 
#define W25X_ReadData			              0x03 
#define W25X_FastReadData		            0x0B 
#define W25X_FastReadDual		            0x3B 
#define W25X_PageProgram		            0x02 
#define W25X_BlockErase			            0xD8 
#define W25X_SectorErase		            0x20 
#define W25X_ChipErase			            0xC7 
#define W25X_PowerDown			            0xB9 
#define W25X_ReleasePowerDown	          0xAB 
#define W25X_DeviceID			              0xAB 
#define W25X_ManufactDeviceID   	      0x90 
#define W25X_JedecDeviceID		          0x9F 

#define WIP_Flag                        0x01  /* Write In Progress (WIP) flag */

#define Dummy_Byte                      0xFF

/* 类型定义 ------------------------------------------------------------------*/
/* 宏定义 --------------------------------------------------------------------*/

//#define  SPI_FLASH_ID                       0xEF3015     //W25X16   2MB
//#define  SPI_FLASH_ID                       0xEF4015	   //W25Q16   4MB
//#define  SPI_FLASH_ID                       0XEF4017     //W25Q64   8MB
#define  SPI_FLASH_ID                       0XEF4018     //W25Q128  16MB Econ开发默认使用

#define FLASH_SPI_CS_ENABLE()                      HAL_GPIO_WritePin(W25Q128_CS_GPIO_Port, W25Q128_CS_Pin, GPIO_PIN_RESET)
#define FLASH_SPI_CS_DISABLE()                     HAL_GPIO_WritePin(W25Q128_CS_GPIO_Port, W25Q128_CS_Pin, GPIO_PIN_SET)

#define  FLASH_WriteAddress     0x00000               //Flash写入地址 
#define  FLASH_ReadAddress      FLASH_WriteAddress
#define  FLASH_SectorToErase    FLASH_WriteAddress


/* 扩展变量 ------------------------------------------------------------------*/

/* 函数声明 ------------------------------------------------------------------*/

void SPI_FLASH_SectorErase(uint32_t SectorAddr);
void SPI_FLASH_BulkErase(void);
void SPI_FLASH_PageWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void SPI_FLASH_BufferWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void SPI_FLASH_BufferRead(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);
uint32_t SPI_FLASH_ReadID(void);
uint32_t SPI_FLASH_ReadDeviceID(void);
void SPI_FLASH_StartReadSequence(uint32_t ReadAddr);
void SPI_Flash_PowerDown(void);
void SPI_Flash_WAKEUP(void);

uint8_t SPI_FLASH_ReadByte(void);
uint8_t SPI_FLASH_SendByte(uint8_t byte);
void SPI_FLASH_WriteEnable(void);
void SPI_FLASH_WaitForWriteEnd(void);

#endif  /* __W25Q128_H */

/******************* (C) COPYRIGHT 2015-2020 硬石嵌入式开发团队 *****END OF FILE****/
