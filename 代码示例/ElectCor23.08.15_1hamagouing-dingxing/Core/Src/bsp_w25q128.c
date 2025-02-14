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
  * @brief  ��ʼ��Flash
  * @param  ��
  * @retval ��
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
  * @brief  ����FLASH����
  * @param  SectorAddr��Ҫ������������ַ
  * @retval ��
  */
void SPI_FLASH_SectorErase(uint32_t SectorAddr)
{
  /* ����FLASHдʹ������ */
  SPI_FLASH_WriteEnable();
  SPI_FLASH_WaitForWriteEnd();
  /* �������� */
  /* ѡ��FLASH: CS�͵�ƽ */
  W25Q128_CSS_L;
  /* ������������ָ��*/
  W25Q128_SOFT_SPI_RW_MODE3(W25X_SectorErase);
  /*���Ͳ���������ַ�ĸ�λ*/
  W25Q128_SOFT_SPI_RW_MODE3((SectorAddr & 0xFF0000) >> 16);
  /* ���Ͳ���������ַ����λ */
  W25Q128_SOFT_SPI_RW_MODE3((SectorAddr & 0xFF00) >> 8);
  /* ���Ͳ���������ַ�ĵ�λ */
  W25Q128_SOFT_SPI_RW_MODE3(SectorAddr & 0xFF);
  /* ֹͣ�ź� FLASH: CS �ߵ�ƽ */
  W25Q128_CSS_H;
  /* �ȴ��������*/
  SPI_FLASH_WaitForWriteEnd();
}


 /**
  * @brief  ����FLASH��������Ƭ����
  * @param  ��
  * @retval ��
  */
void SPI_FLASH_BulkErase(void)
{
  /* ����FLASHдʹ������ */
  SPI_FLASH_WriteEnable();

  /* ���� Erase */
  /* ѡ��FLASH: CS�͵�ƽ */
  W25Q128_CSS_L;
  /* �����������ָ��*/
  W25Q128_SOFT_SPI_RW_MODE3(W25X_ChipErase);
  /* ֹͣ�ź� FLASH: CS �ߵ�ƽ */
  W25Q128_CSS_H;

  /* �ȴ��������*/
  SPI_FLASH_WaitForWriteEnd();
}

 /**
  * @brief  ��FLASH��ҳд�����ݣ����ñ�����д������ǰ��Ҫ�Ȳ�������
  * @param	pBuffer��Ҫд�����ݵ�ָ��
  * @param WriteAddr��д���ַ
  * @param  NumByteToWrite��д�����ݳ��ȣ�����С�ڵ���SPI_FLASH_PerWritePageSize
  * @retval ��
  */
void SPI_FLASH_PageWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
  /* ����FLASHдʹ������ */
  SPI_FLASH_WriteEnable();

  /* ѡ��FLASH: CS�͵�ƽ */
  W25Q128_CSS_L;
  /* дҳдָ��*/
  W25Q128_SOFT_SPI_RW_MODE3(W25X_PageProgram);
  /*����д��ַ�ĸ�λ*/
  W25Q128_SOFT_SPI_RW_MODE3((WriteAddr & 0xFF0000) >> 16);
  /*����д��ַ����λ*/
  W25Q128_SOFT_SPI_RW_MODE3((WriteAddr & 0xFF00) >> 8);
  /*����д��ַ�ĵ�λ*/
  W25Q128_SOFT_SPI_RW_MODE3(WriteAddr & 0xFF);

  if(NumByteToWrite > SPI_FLASH_PerWritePageSize)
  {
     NumByteToWrite = SPI_FLASH_PerWritePageSize;
     //FLASH_ERROR("SPI_FLASH_PageWrite too large!");
  }

  /* д������*/
  while (NumByteToWrite--)
  {
    /* ���͵�ǰҪд����ֽ����� */
    W25Q128_SOFT_SPI_RW_MODE3(*pBuffer);
    /* ָ����һ�ֽ����� */
    pBuffer++;
  }

  /* ֹͣ�ź� FLASH: CS �ߵ�ƽ */
  W25Q128_CSS_H;

  /* �ȴ�д�����*/
  SPI_FLASH_WaitForWriteEnd();
}


 /**
  * @brief  ��FLASHд�����ݣ����ñ�����д������ǰ��Ҫ�Ȳ�������
  * @param	pBuffer��Ҫд�����ݵ�ָ��
  * @param  WriteAddr��д���ַ
  * @param  NumByteToWrite��д�����ݳ���
  * @retval ��
  */
void SPI_FLASH_BufferWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
  uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;
	
	/*mod�������࣬��writeAddr��SPI_FLASH_PageSize��������������AddrֵΪ0*/
  Addr = WriteAddr % SPI_FLASH_PageSize;
	
	/*��count������ֵ���պÿ��Զ��뵽ҳ��ַ*/
  count = SPI_FLASH_PageSize - Addr;
	/*�����Ҫд��������ҳ*/
  NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
	/*mod�������࣬�����ʣ�಻��һҳ���ֽ���*/
  NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;

	 /* Addr=0,��WriteAddr �պð�ҳ���� aligned  */
  if (Addr == 0) 
  {
		/* NumByteToWrite < SPI_FLASH_PageSize */
    if (NumOfPage == 0) 
    {
      SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
    }
    else /* NumByteToWrite > SPI_FLASH_PageSize */
    {
			/*�Ȱ�����ҳ��д��*/
      while (NumOfPage--)
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
        WriteAddr +=  SPI_FLASH_PageSize;
        pBuffer += SPI_FLASH_PageSize;
      }
			/*���ж���Ĳ���һҳ�����ݣ�����д��*/
      SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
    }
  }
	/* ����ַ�� SPI_FLASH_PageSize ������  */
  else 
  {
		/* NumByteToWrite < SPI_FLASH_PageSize */
    if (NumOfPage == 0) 
    {
			/*��ǰҳʣ���count��λ�ñ�NumOfSingleС��д����*/
      if (NumOfSingle > count) 
      {
        temp = NumOfSingle - count;
				
				/*��д����ǰҳ*/
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
        WriteAddr +=  count;
        pBuffer += count;
				
				/*��дʣ�������*/
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, temp);
      }
      else /*��ǰҳʣ���count��λ����д��NumOfSingle������*/
      {				
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
      }
    }
    else /* NumByteToWrite > SPI_FLASH_PageSize */
    {
			/*��ַ����������count�ֿ������������������*/
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
      NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;

      SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
      WriteAddr +=  count;
      pBuffer += count;
			
			/*������ҳ��д��*/
      while (NumOfPage--)
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
        WriteAddr +=  SPI_FLASH_PageSize;
        pBuffer += SPI_FLASH_PageSize;
      }
			/*���ж���Ĳ���һҳ�����ݣ�����д��*/
      if (NumOfSingle != 0)
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
      }
    }
  }
}

 /**
  * @brief  ��ȡFLASH����
  * @param 	pBuffer���洢�������ݵ�ָ��
  * @param   ReadAddr����ȡ��ַ
  * @param   NumByteToRead����ȡ���ݳ���
  * @retval ��
  */
void SPI_FLASH_BufferRead(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{
  /* ѡ��FLASH: CS�͵�ƽ */
  W25Q128_CSS_L;

  /* ���� �� ָ�� */
  W25Q128_SOFT_SPI_RW_MODE3(W25X_ReadData);

  /* ���� �� ��ַ��λ */
  W25Q128_SOFT_SPI_RW_MODE3((ReadAddr & 0xFF0000) >> 16);
  /* ���� �� ��ַ��λ */
  W25Q128_SOFT_SPI_RW_MODE3((ReadAddr& 0xFF00) >> 8);
  /* ���� �� ��ַ��λ */
  W25Q128_SOFT_SPI_RW_MODE3(ReadAddr & 0xFF);
  
	/* ��ȡ���� */
  while (NumByteToRead--)
  {
    /* ��ȡһ���ֽ�*/
    *pBuffer = W25Q128_SOFT_SPI_RW_MODE3(Dummy_Byte);
    /* ָ����һ���ֽڻ����� */
    pBuffer++;
  }

  /* ֹͣ�ź� FLASH: CS �ߵ�ƽ */
  W25Q128_CSS_H;
}


 /**
  * @brief  ��ȡFLASH ID
  * @param 	��
  * @retval FLASH ID
  */
uint32_t SPI_FLASH_ReadID(void)
{
  uint32_t Temp = 0;
	uint8_t Temp0 = 0, Temp1 = 0, Temp2 = 0;

  /* ��ʼͨѶ��CS�͵�ƽ */
  W25Q128_CSS_L;

  /* ����JEDECָ���ȡID */
  W25Q128_SOFT_SPI_RW_MODE3(W25X_JedecDeviceID);
	
  /* ��ȡһ���ֽ����� */
  Temp0 = W25Q128_SOFT_SPI_RW_MODE3(Dummy_Byte);

  /* ��ȡһ���ֽ����� */
  Temp1 = W25Q128_SOFT_SPI_RW_MODE3(Dummy_Byte);

  /* ��ȡһ���ֽ����� */
  Temp2 = W25Q128_SOFT_SPI_RW_MODE3(Dummy_Byte);

  /* ֹͣͨѶ��CS�ߵ�ƽ */
  W25Q128_CSS_H;

	/*�����������������Ϊ�����ķ���ֵ*/
  Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;

  return Temp;
}

 /**
  * @brief  ��ȡFLASH Device ID
  * @param 	��
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
  * @brief  ʹ��SPI��ȡһ���ֽڵ�����
  * @param  ��
  * @retval ���ؽ��յ�������
  */
uint8_t SPI_FLASH_ReadByte(void)
{
  return (W25Q128_SOFT_SPI_RW_MODE3(Dummy_Byte));
}

 /**
  * @brief  ��FLASH���� дʹ�� ����
  * @param  none
  * @retval none
  */
void SPI_FLASH_WriteEnable(void)
{
  /* ͨѶ��ʼ��CS�� */
  W25Q128_CSS_L;

  /* ����дʹ������*/
  W25Q128_SOFT_SPI_RW_MODE3(W25X_WriteEnable);

  /*ͨѶ������CS�� */
  W25Q128_CSS_H;
}

 /**
  * @brief  �ȴ�WIP(BUSY)��־����0�����ȴ���FLASH�ڲ�����д�����
  * @param  none
  * @retval none
  */
void SPI_FLASH_WaitForWriteEnd(void)
{
  uint8_t FLASH_Status = 0;

  /* ѡ�� FLASH: CS �� */
  W25Q128_CSS_L;

  /* ���� ��״̬�Ĵ��� ���� */
  W25Q128_SOFT_SPI_RW_MODE3(W25X_ReadStatusReg);

  SPITimeout = SPIT_FLAG_TIMEOUT;
  /* ��FLASHæµ����ȴ� */
  do
  {
    /* ��ȡFLASHоƬ��״̬�Ĵ��� */
    FLASH_Status = W25Q128_SOFT_SPI_RW_MODE3(Dummy_Byte);	 
  }
  while ((FLASH_Status & WIP_Flag) == SET); /* ����д���־ */

  /* ֹͣ�ź�  FLASH: CS �� */
  W25Q128_CSS_H;
}


//�������ģʽ
void SPI_Flash_PowerDown(void)   
{ 
  /* ѡ�� FLASH: CS �� */
  W25Q128_CSS_L;

  /* ���� ���� ���� */
  W25Q128_SOFT_SPI_RW_MODE3(W25X_PowerDown);

  /* ֹͣ�ź�  FLASH: CS �� */
  W25Q128_CSS_H;
}   

//����
void SPI_Flash_WAKEUP(void)   
{
  /*ѡ�� FLASH: CS �� */
  W25Q128_CSS_L;

  /* ���� �ϵ� ���� */
  W25Q128_SOFT_SPI_RW_MODE3(W25X_ReleasePowerDown);

  /* ֹͣ�ź� FLASH: CS �� */
  W25Q128_CSS_H;                   //�ȴ�TRES1
}

