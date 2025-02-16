#include "bsp_w25q128.h"

static __IO uint32_t  SPITimeout = SPIT_LONG_TIMEOUT;   

void SPI_Delay(uint8_t time)
{
	for(int i=0;i<time;i++);
}

/* CPOL = 1, CPHA = 1, MSB first */
uint8_t W25Q128_SOFT_SPI_RW_MODE3( uint8_t write_dat )
{
    uint8_t i, read_dat;
		SPI_Delay(2);
    for( i = 0; i < 8; i++ )
		{
			W25Q128_SCK_L; 
			SPI_Delay(2);
      if( write_dat & 0x80 )
				W25Q128_MOSI_H;  
      else                    
        W25Q128_MOSI_L; 
      write_dat <<= 1;
			SPI_Delay(2);
      W25Q128_SCK_H; 
			SPI_Delay(2);
      read_dat <<= 1;  
      if(W25Q128_MISO_READ)
				read_dat++;
    }
    return read_dat;
}

 /**
  * @brief  初始化Flash
  * @param  无
  * @retval 无
  */
void SPI_FLASH_Init()
{
	W25Q128_CSS_H;
	W25Q128_SCK_H;
	W25Q128_MOSI_H;
//	W25Q128_CSS_L;
//	W25Q128_SOFT_SPI_RW_MODE3(0x66);
//	W25Q128_CSS_H;
//	
//	HAL_Delay(10);
//	
//	W25Q128_CSS_L;
//	W25Q128_SOFT_SPI_RW_MODE3(0x99);
//	W25Q128_CSS_H;
//	
//	HAL_Delay(10);
}


 /**
  * @brief  擦除FLASH扇区
  * @param  SectorAddr：要擦除的扇区地址
  * @retval 无
  */
void SPI_FLASH_SectorErase(uint32_t SectorAddr)
{
  /* 发送FLASH写使能命令 */
  SPI_FLASH_WriteEnable();
  SPI_FLASH_WaitForWriteEnd();
  /* 擦除扇区 */
  /* 选择FLASH: CS低电平 */
  W25Q128_CSS_L;
  /* 发送扇区擦除指令*/
  W25Q128_SOFT_SPI_RW_MODE3(W25X_SectorErase);
  /*发送擦除扇区地址的高位*/
  W25Q128_SOFT_SPI_RW_MODE3((SectorAddr & 0xFF0000) >> 16);
  /* 发送擦除扇区地址的中位 */
  W25Q128_SOFT_SPI_RW_MODE3((SectorAddr & 0xFF00) >> 8);
  /* 发送擦除扇区地址的低位 */
  W25Q128_SOFT_SPI_RW_MODE3(SectorAddr & 0xFF);
  /* 停止信号 FLASH: CS 高电平 */
  W25Q128_CSS_H;
  /* 等待擦除完毕*/
  SPI_FLASH_WaitForWriteEnd();
}


 /**
  * @brief  擦除FLASH扇区，整片擦除
  * @param  无
  * @retval 无
  */
void SPI_FLASH_BulkErase(void)
{
  /* 发送FLASH写使能命令 */
  SPI_FLASH_WriteEnable();

  /* 整块 Erase */
  /* 选择FLASH: CS低电平 */
  W25Q128_CSS_L;
  /* 发送整块擦除指令*/
  W25Q128_SOFT_SPI_RW_MODE3(W25X_ChipErase);
  /* 停止信号 FLASH: CS 高电平 */
  W25Q128_CSS_H;

  /* 等待擦除完毕*/
  SPI_FLASH_WaitForWriteEnd();
}

 /**
  * @brief  对FLASH按页写入数据，调用本函数写入数据前需要先擦除扇区
  * @param	pBuffer，要写入数据的指针
  * @param WriteAddr，写入地址
  * @param  NumByteToWrite，写入数据长度，必须小于等于SPI_FLASH_PerWritePageSize
  * @retval 无
  */
void SPI_FLASH_PageWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
  /* 发送FLASH写使能命令 */
  SPI_FLASH_WriteEnable();

  /* 选择FLASH: CS低电平 */
  W25Q128_CSS_L;
  /* 写页写指令*/
  W25Q128_SOFT_SPI_RW_MODE3(W25X_PageProgram);
  /*发送写地址的高位*/
  W25Q128_SOFT_SPI_RW_MODE3((WriteAddr & 0xFF0000) >> 16);
  /*发送写地址的中位*/
  W25Q128_SOFT_SPI_RW_MODE3((WriteAddr & 0xFF00) >> 8);
  /*发送写地址的低位*/
  W25Q128_SOFT_SPI_RW_MODE3(WriteAddr & 0xFF);

  if(NumByteToWrite > SPI_FLASH_PerWritePageSize)
  {
     NumByteToWrite = SPI_FLASH_PerWritePageSize;
     //FLASH_ERROR("SPI_FLASH_PageWrite too large!");
  }

  /* 写入数据*/
  while (NumByteToWrite--)
  {
    /* 发送当前要写入的字节数据 */
    W25Q128_SOFT_SPI_RW_MODE3(*pBuffer);
    /* 指向下一字节数据 */
    pBuffer++;
  }

  /* 停止信号 FLASH: CS 高电平 */
  W25Q128_CSS_H;

  /* 等待写入完毕*/
  SPI_FLASH_WaitForWriteEnd();
}


 /**
  * @brief  对FLASH写入数据，调用本函数写入数据前需要先擦除扇区
  * @param	pBuffer，要写入数据的指针
  * @param  WriteAddr，写入地址
  * @param  NumByteToWrite，写入数据长度
  * @retval 无
  */
void SPI_FLASH_BufferWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
  uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;
	
	/*mod运算求余，若writeAddr是SPI_FLASH_PageSize整数倍，运算结果Addr值为0*/
  Addr = WriteAddr % SPI_FLASH_PageSize;
	
	/*差count个数据值，刚好可以对齐到页地址*/
  count = SPI_FLASH_PageSize - Addr;
	/*计算出要写多少整数页*/
  NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
	/*mod运算求余，计算出剩余不满一页的字节数*/
  NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;

	 /* Addr=0,则WriteAddr 刚好按页对齐 aligned  */
  if (Addr == 0) 
  {
		/* NumByteToWrite < SPI_FLASH_PageSize */
    if (NumOfPage == 0) 
    {
      SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
    }
    else /* NumByteToWrite > SPI_FLASH_PageSize */
    {
			/*先把整数页都写了*/
      while (NumOfPage--)
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
        WriteAddr +=  SPI_FLASH_PageSize;
        pBuffer += SPI_FLASH_PageSize;
      }
			/*若有多余的不满一页的数据，把它写完*/
      SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
    }
  }
	/* 若地址与 SPI_FLASH_PageSize 不对齐  */
  else 
  {
		/* NumByteToWrite < SPI_FLASH_PageSize */
    if (NumOfPage == 0) 
    {
			/*当前页剩余的count个位置比NumOfSingle小，写不完*/
      if (NumOfSingle > count) 
      {
        temp = NumOfSingle - count;
				
				/*先写满当前页*/
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
        WriteAddr +=  count;
        pBuffer += count;
				
				/*再写剩余的数据*/
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, temp);
      }
      else /*当前页剩余的count个位置能写完NumOfSingle个数据*/
      {				
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
      }
    }
    else /* NumByteToWrite > SPI_FLASH_PageSize */
    {
			/*地址不对齐多出的count分开处理，不加入这个运算*/
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
      NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;

      SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
      WriteAddr +=  count;
      pBuffer += count;
			
			/*把整数页都写了*/
      while (NumOfPage--)
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
        WriteAddr +=  SPI_FLASH_PageSize;
        pBuffer += SPI_FLASH_PageSize;
      }
			/*若有多余的不满一页的数据，把它写完*/
      if (NumOfSingle != 0)
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
      }
    }
  }
}

 /**
  * @brief  读取FLASH数据
  * @param 	pBuffer，存储读出数据的指针
  * @param   ReadAddr，读取地址
  * @param   NumByteToRead，读取数据长度
  * @retval 无
  */
void SPI_FLASH_BufferRead(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{
  /* 选择FLASH: CS低电平 */
  W25Q128_CSS_L;

  /* 发送 读 指令 */
  W25Q128_SOFT_SPI_RW_MODE3(W25X_ReadData);

  /* 发送 读 地址高位 */
  W25Q128_SOFT_SPI_RW_MODE3((ReadAddr & 0xFF0000) >> 16);
  /* 发送 读 地址中位 */
  W25Q128_SOFT_SPI_RW_MODE3((ReadAddr& 0xFF00) >> 8);
  /* 发送 读 地址低位 */
  W25Q128_SOFT_SPI_RW_MODE3(ReadAddr & 0xFF);
  
	/* 读取数据 */
  while (NumByteToRead--)
  {
    /* 读取一个字节*/
    *pBuffer = W25Q128_SOFT_SPI_RW_MODE3(Dummy_Byte);
    /* 指向下一个字节缓冲区 */
    pBuffer++;
  }

  /* 停止信号 FLASH: CS 高电平 */
  W25Q128_CSS_H;
}


 /**
  * @brief  读取FLASH ID
  * @param 	无
  * @retval FLASH ID
  */
uint32_t SPI_FLASH_ReadID(void)
{
  uint32_t Temp = 0;
	uint8_t Temp0 = 0, Temp1 = 0, Temp2 = 0;

  /* 开始通讯：CS低电平 */
  W25Q128_CSS_L;

  /* 发送JEDEC指令，读取ID */
  W25Q128_SOFT_SPI_RW_MODE3(W25X_JedecDeviceID);
	
  /* 读取一个字节数据 */
  Temp0 = W25Q128_SOFT_SPI_RW_MODE3(Dummy_Byte);

  /* 读取一个字节数据 */
  Temp1 = W25Q128_SOFT_SPI_RW_MODE3(Dummy_Byte);

  /* 读取一个字节数据 */
  Temp2 = W25Q128_SOFT_SPI_RW_MODE3(Dummy_Byte);

  /* 停止通讯：CS高电平 */
  W25Q128_CSS_H;

	/*把数据组合起来，作为函数的返回值*/
  Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;

  return Temp;
}

 /**
  * @brief  读取FLASH Device ID
  * @param 	无
  * @retval FLASH Device ID
  */
uint8_t SPI_FLASH_ReadDeviceID(void)
{
  uint8_t Temp = 0;

  /* Select the FLASH: Chip Select low */
  W25Q128_CSS_L;

  /* Send "RDID " instruction */
  W25Q128_SOFT_SPI_RW_MODE3(W25X_DeviceID);
  W25Q128_SOFT_SPI_RW_MODE3(Dummy_Byte);
  W25Q128_SOFT_SPI_RW_MODE3(Dummy_Byte);
  W25Q128_SOFT_SPI_RW_MODE3(Dummy_Byte);
  
  /* Read a byte from the FLASH */
  Temp = W25Q128_SOFT_SPI_RW_MODE3(Dummy_Byte);

  /* Deselect the FLASH: Chip Select high */
  W25Q128_CSS_H;

  return Temp;
}

/*******************************************************************************
* Function Name  : SPI_FLASH_StartReadSequence
* Description    : Initiates a read data byte (READ) sequence from the Flash.
*                  This is done by driving the /CS line low to select the device,
*                  then the READ instruction is transmitted followed by 3 bytes
*                  address. This function exit and keep the /CS line low, so the
*                  Flash still being selected. With this technique the whole
*                  content of the Flash is read with a single READ instruction.
* Input          : - ReadAddr : FLASH's internal address to read from.
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_StartReadSequence(uint32_t ReadAddr)
{
  /* Select the FLASH: Chip Select low */
  W25Q128_CSS_L;

  /* Send "Read from Memory " instruction */
  W25Q128_SOFT_SPI_RW_MODE3(W25X_ReadData);

  /* Send the 24-bit address of the address to read from -----------------------*/
  /* Send ReadAddr high nibble address byte */
  W25Q128_SOFT_SPI_RW_MODE3((ReadAddr & 0xFF0000) >> 16);
  /* Send ReadAddr medium nibble address byte */
  W25Q128_SOFT_SPI_RW_MODE3((ReadAddr& 0xFF00) >> 8);
  /* Send ReadAddr low nibble address byte */
  W25Q128_SOFT_SPI_RW_MODE3(ReadAddr & 0xFF);
}

 /**
  * @brief  使用SPI读取一个字节的数据
  * @param  无
  * @retval 返回接收到的数据
  */
uint8_t SPI_FLASH_ReadByte(void)
{
  return (W25Q128_SOFT_SPI_RW_MODE3(Dummy_Byte));
}

 /**
  * @brief  向FLASH发送 写使能 命令
  * @param  none
  * @retval none
  */
void SPI_FLASH_WriteEnable(void)
{
  /* 通讯开始：CS低 */
  W25Q128_CSS_L;

  /* 发送写使能命令*/
  W25Q128_SOFT_SPI_RW_MODE3(W25X_WriteEnable);

  /*通讯结束：CS高 */
  W25Q128_CSS_H;
}

 /**
  * @brief  等待WIP(BUSY)标志被置0，即等待到FLASH内部数据写入完毕
  * @param  none
  * @retval none
  */
void SPI_FLASH_WaitForWriteEnd(void)
{
  uint8_t FLASH_Status = 0;

  /* 选择 FLASH: CS 低 */
  W25Q128_CSS_L;

  /* 发送 读状态寄存器 命令 */
  W25Q128_SOFT_SPI_RW_MODE3(W25X_ReadStatusReg);

  SPITimeout = SPIT_FLAG_TIMEOUT;
  /* 若FLASH忙碌，则等待 */
  do
  {
    /* 读取FLASH芯片的状态寄存器 */
    FLASH_Status = W25Q128_SOFT_SPI_RW_MODE3(Dummy_Byte);	 
  }
  while ((FLASH_Status & WIP_Flag) == SET); /* 正在写入标志 */

  /* 停止信号  FLASH: CS 高 */
  W25Q128_CSS_H;
}


//进入掉电模式
void SPI_Flash_PowerDown(void)   
{ 
  /* 选择 FLASH: CS 低 */
  W25Q128_CSS_L;

  /* 发送 掉电 命令 */
  W25Q128_SOFT_SPI_RW_MODE3(W25X_PowerDown);

  /* 停止信号  FLASH: CS 高 */
  W25Q128_CSS_H;
}   

//唤醒
void SPI_Flash_WAKEUP(void)   
{
  /*选择 FLASH: CS 低 */
  W25Q128_CSS_L;

  /* 发上 上电 命令 */
  W25Q128_SOFT_SPI_RW_MODE3(W25X_ReleasePowerDown);

  /* 停止信号 FLASH: CS 高 */
  W25Q128_CSS_H;                   //等待TRES1
}

