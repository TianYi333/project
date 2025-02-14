#include "w5500_spi.h"
#include "wizchip_conf.h"
#include "stdio.h"

extern SPI_HandleTypeDef hspi3;
 
uint8_t SPIReadWrite(uint8_t TxData)
{
  uint8_t RxData;
  HAL_SPI_TransmitReceive(&hspi3, (uint8_t *)&TxData, &RxData, 1, 100);
  return RxData;
}

void  wizchip_select(void)
{
	HAL_GPIO_WritePin(WK_SPI_SCS_GPIO_Port, WK_SPI_SCS_Pin, GPIO_PIN_RESET);
}

void  wizchip_deselect(void)
{
	HAL_GPIO_WritePin(WK_SPI_SCS_GPIO_Port, WK_SPI_SCS_Pin, GPIO_PIN_SET);
}

uint8_t wizchip_read(void)
{
	uint8_t rb;
	rb=SPIReadWrite(0x00);
	return rb;
}

void  wizchip_write(uint8_t wb)
{
	SPIReadWrite(wb);
}

void wizchip_readburst(uint8_t* pBuf, uint16_t len)
{
	for(uint16_t i=0;i<len;i++)
	{
		*pBuf=SPIReadWrite(0x00);
		pBuf++;
	}
}

void  wizchip_writeburst(uint8_t* pBuf, uint16_t len)
{
	for(uint16_t i=0;i<len;i++)
	{
		SPIReadWrite(*pBuf);
		pBuf++;
	}
}

void W5500IOInit(void)
{
  //使用STM32Cube生成的IO引脚初始化
	  MX_SPI3_Init();
//	  MX_GPIO_Init();
}

void w5500_enter_critical(void)
{
	__disable_irq();
}

void w5500_exit_critical(void)
{
	__enable_irq();
}

void W5500Init()
{
	uint8_t tmp;
	uint8_t memsize[2][8] = { { 2, 2, 2, 2, 2, 2, 2, 2 }, { 2, 2, 2, 2, 2, 2, 2, 2 } };

	W5500IOInit();

	HAL_GPIO_WritePin(WK_SPI_SCS_GPIO_Port, WK_SPI_SCS_Pin, GPIO_PIN_SET);//CS high by default

	//Send a pulse on reset pin
	HAL_GPIO_WritePin(WK__RESET_GPIO_Port, WK__RESET_Pin, GPIO_PIN_RESET);
	tmp = 0xFF;
	while(tmp--);
	HAL_GPIO_WritePin(WK__RESET_GPIO_Port, WK__RESET_Pin, GPIO_PIN_SET);

	reg_wizchip_cs_cbfunc(wizchip_select, wizchip_deselect);
	reg_wizchip_spi_cbfunc(wizchip_read, wizchip_write);
	reg_wizchip_spiburst_cbfunc(wizchip_readburst, wizchip_writeburst);
	reg_wizchip_cris_cbfunc(w5500_enter_critical, w5500_exit_critical);

	/* WIZChip Initialize*/
	if (ctlwizchip(CW_INIT_WIZCHIP, (void*) memsize) == -1) {
		printf("WIZCHIP Initialized Failed.\r\n");
		while (1);
	}
	printf("WIZCHIP Initialization Success.\r\n");

	//Check if SPI is working
	printf("Auto-detecting Ethernet Chip on SPI Port...");
	if(WIZCHIP_READ(VERSIONR)==0x04)
	{
		printf("W5500 Found on SPI\r\n");
	}
	else//将串行同步时钟的空闲状态设置为高电平！！！！！！！！
	{
		printf("W5500 NOT Found on SPI\r\n");
		printf("Double Check SPI Cables\r\n");
		while(1);
	}
}

void PHYStatusCheck(void)
{
	uint8_t tmp;

	do
	{
		printf("\r\nChecking Ethernet Cable Presence ...");
		ctlwizchip(CW_GET_PHYLINK, (void*) &tmp);

		if(tmp == PHY_LINK_OFF)
		{
			printf("NO Cable Connected!");
			osDelay(1500);
		}
	}while(tmp == PHY_LINK_OFF);

	printf("Good! Cable got connected!");

}

void PrintPHYConf(void)
{
	wiz_PhyConf phyconf;

	ctlwizchip(CW_GET_PHYCONF, (void*) &phyconf);

	if(phyconf.by==PHY_CONFBY_HW)
	{
		printf("\r\nPHY Configured by Hardware Pins");
	}
	else
	{
		printf("\r\nPHY Configured by Registers");
	}

	if(phyconf.mode==PHY_MODE_AUTONEGO)
	{
		printf("\r\nAutonegotiation Enabled");
	}
	else
	{
		printf("\r\nAutonegotiation NOT Enabled");
	}

	if(phyconf.duplex==PHY_DUPLEX_FULL)
	{
		printf("\r\nDuplex Mode: Full");
	}
	else
	{
		printf("\r\nDuplex Mode: Half");
	}

	if(phyconf.speed==PHY_SPEED_10)
	{
		printf("\r\nSpeed: 10Mbps");
	}
	else
	{
		printf("\r\nSpeed: 100Mbps");
	}
}





#if (defined(__GNUC__) && !defined(__CC_ARM))
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#define GETCHAR_PROTOTYPE int __io_getchar(void)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#define GETCHAR_PROTOTYPE int fgetc(FILE *f)
#endif /* __GNUC__ */

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART2 and Loop until the end of transmission */
  while (HAL_OK != HAL_UART_Transmit(&huart2, (uint8_t *) &ch, 1, 30000))
  {
    ;
  }
  return ch;
}

/**
  * @brief  Retargets the C library scanf function to the USART.
  * @param  None
  * @retval None
  */
GETCHAR_PROTOTYPE
{
  /* Place your implementation of fgetc here */
  /* e.g. read a character on USART and loop until the end of read */
  uint8_t ch = 0;
  while (HAL_OK != HAL_UART_Receive(&huart2, (uint8_t *)&ch, 1, 30000))
  {
    ;
  }
  return ch;
}
