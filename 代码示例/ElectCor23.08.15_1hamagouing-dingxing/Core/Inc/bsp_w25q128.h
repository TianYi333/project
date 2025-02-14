#ifndef __BSP_W25Q128_H__
#define __BSP_W25Q128_H__

#include "stm32f4xx_hal.h"

#define		W25Q128_SPI_PORT			GPIOA

#define 	W25Q128_CS_PIN				GPIO_PIN_4
#define 	W25Q128_CLK_PIN				GPIO_PIN_5
#define 	W25Q128_MISO_PIN			GPIO_PIN_6
#define 	W25Q128_MOSI_PIN			GPIO_PIN_7


#define 	W25Q128_MOSI_H  		HAL_GPIO_WritePin(W25Q128_SPI_PORT, W25Q128_MOSI_PIN, GPIO_PIN_SET)  
#define 	W25Q128_MOSI_L  		HAL_GPIO_WritePin(W25Q128_SPI_PORT, W25Q128_MOSI_PIN, GPIO_PIN_RESET)  

#define 	W25Q128_SCK_H   		HAL_GPIO_WritePin(W25Q128_SPI_PORT, W25Q128_CLK_PIN, GPIO_PIN_SET)  
#define 	W25Q128_SCK_L   		HAL_GPIO_WritePin(W25Q128_SPI_PORT, W25Q128_CLK_PIN, GPIO_PIN_RESET)  

#define 	W25Q128_CSS_H   		HAL_GPIO_WritePin(W25Q128_SPI_PORT, W25Q128_CS_PIN, GPIO_PIN_SET)  
#define 	W25Q128_CSS_L   		HAL_GPIO_WritePin(W25Q128_SPI_PORT, W25Q128_CS_PIN, GPIO_PIN_RESET) 

#define 	W25Q128_MISO_READ   HAL_GPIO_ReadPin(W25Q128_SPI_PORT, W25Q128_MISO_PIN)


/***************W25Q128相关定义*********************/
#define sFLASH_ID                 0XEF4018    //W25Q128

#define SPI_FLASH_PageSize              256
#define SPI_FLASH_PerWritePageSize      256

#define W25X_WriteEnable		      0x06 
#define W25X_WriteDisable		      0x04 
#define W25X_ReadStatusReg		    0x05 
#define W25X_WriteStatusReg		    0x01 
#define W25X_ReadData			        0x03 
#define W25X_FastReadData		      0x0B 
#define W25X_FastReadDual		      0x3B 
#define W25X_PageProgram		      0x02 
#define W25X_BlockErase			      0xD8 
#define W25X_SectorErase		      0x20 
#define W25X_ChipErase			      0xC7 
#define W25X_PowerDown			      0xB9 
#define W25X_ReleasePowerDown	    0xAB 
#define W25X_DeviceID			        0xAB 
#define W25X_ManufactDeviceID   	0x90 
#define W25X_JedecDeviceID		    0x9F

#define WIP_Flag                  0x01  /* Write In Progress (WIP) flag */
#define Dummy_Byte                0xFF

/*等待超时时间*/
#define SPIT_FLAG_TIMEOUT         ((uint32_t)0x1000)
#define SPIT_LONG_TIMEOUT         ((uint32_t)(10 * SPIT_FLAG_TIMEOUT))

uint8_t W25Q128_SOFT_SPI_RW_MODE3( uint8_t write_dat );

/* SPI FLASH初始化函数 */
void SPI_FLASH_Init(void);

/* SPI FLASH基础操作函数 */
void SPI_FLASH_SectorErase(uint32_t SectorAddr);
void SPI_FLASH_BulkErase(void);
void SPI_FLASH_PageWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void SPI_FLASH_BufferWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void SPI_FLASH_BufferRead(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);
uint32_t SPI_FLASH_ReadID(void);
uint8_t SPI_FLASH_ReadDeviceID(void);
void SPI_FLASH_StartReadSequence(uint32_t ReadAddr);
void SPI_Flash_PowerDown(void);
void SPI_Flash_WAKEUP(void);
uint8_t SPI_FLASH_ReadByte(void);
uint8_t SPI_FLASH_SendByte(uint8_t byte);
void SPI_FLASH_WriteEnable(void);
void SPI_FLASH_WaitForWriteEnd(void);


void SPI_Delay(uint8_t time);
#endif
