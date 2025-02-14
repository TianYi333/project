#include "bsp_mcz33996.h"

void SPI_MCZ_Delay(void)
{
	for(int i=0;i<10;i++);
}

/**
 * @brief 模拟SPI（模式3 CPOL = 1, CPHA = 1）
 * @param	要发送的数据
 * @retval void
 */ 
void MCZ_SOFT_SPI_RW_MODE3(uint32_t write_dat)
{
	uint8_t i;
	MCZ_CSS_L;
	for( i = 0; i < 24; i++ )
	{
		MCZ_SCK_L; 
		SPI_MCZ_Delay();
		 if(write_dat & 0x800000)
				 MCZ_MOSI_H;
		 else                    
				 MCZ_MOSI_L; 
		 SPI_MCZ_Delay();
		 write_dat <<= 1;
		 MCZ_SCK_H; 
		 SPI_MCZ_Delay();
	}
	MCZ_CSS_H;
}

/**
 * @brief MCZ33996初始化
 * @param	void
 * @retval void
 */ 
void MCZ33996_INIT()
{
	MCZ_CSS_L;
	MCZ_RESET;
	MCZ_SET;
	MCZ_CSS_H;
}

void OPEN_MCZ33996_OUT(uint32_t out_num)
{
	
	MCZ_SOFT_SPI_RW_MODE3(out_num<<1);
	
}
