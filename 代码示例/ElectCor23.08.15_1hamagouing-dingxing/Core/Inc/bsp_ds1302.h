#ifndef __BSP_DS1302_H__
#define __BSP_DS1302_H__

#include "stm32f4xx_hal.h"

#define		DS1302_DAT_PORT 				GPIOD
#define		DS1302_CLK_RST_PORT 		GPIOC

#define		DS1302_I2C_DAT 	GPIO_PIN_0
#define		DS1302_I2C_CLK 	GPIO_PIN_12
#define		DS1302_I2C_RST 	GPIO_PIN_11

#define 	DS1302_I2C_DAT_H			HAL_GPIO_WritePin(DS1302_DAT_PORT, DS1302_I2C_DAT, GPIO_PIN_SET)  
#define 	DS1302_I2C_DAT_L			HAL_GPIO_WritePin(DS1302_DAT_PORT, DS1302_I2C_DAT, GPIO_PIN_RESET)  

#define 	DS1302_I2C_CLK_H			HAL_GPIO_WritePin(DS1302_CLK_RST_PORT, DS1302_I2C_CLK, GPIO_PIN_SET)  
#define 	DS1302_I2C_CLK_L			HAL_GPIO_WritePin(DS1302_CLK_RST_PORT, DS1302_I2C_CLK, GPIO_PIN_RESET)  

#define		DS1302_RST_H					HAL_GPIO_WritePin(DS1302_CLK_RST_PORT,DS1302_I2C_RST,GPIO_PIN_SET)
#define		DS1302_RST_L					HAL_GPIO_WritePin(DS1302_CLK_RST_PORT,DS1302_I2C_RST,GPIO_PIN_RESET)

/* д���ݼĴ�����ַ */
#define ds1302_sec_write			0x80		//�����ݵ�ַ
#define ds1302_min_write			0x82	//�����ݵ�ַ
#define ds1302_hr_write				0x84		//ʱ���ݵ�ַ
#define ds1302_date_write			0x86		//�����ݵ�ַ
#define ds1302_month_write			0x88		//�����ݵ�ַ
#define ds1302_day_write				0x8a		//�������ݵ�ַ
#define ds1302_year_write				0x8c		//�����ݵ�ַ

/* �����ݼĴ�����ַ */
#define ds1302_sec_read			0x81		//�����ݵ�ַ
#define ds1302_min_read				0x83		//�����ݵ�ַ
#define ds1302_hr_read				0x85		//ʱ���ݵ�ַ
#define ds1302_date_read			0x87		//�����ݵ�ַ
#define ds1302_month_read			0x89		//�����ݵ�ַ
#define ds1302_day_read				0x8b		//�������ݵ�ַ
#define ds1302_year_read				0x8d		//�����ݵ�ַ


#define ds1302_control_add		0x8f		//�������ݵ�ַ
#define ds1302_charger_add		0x90
#define ds1302_clkburst_add		0xbe

void DS1302_Write_Time(uint8_t *time_buf);
void DS1302_Read_Time(uint8_t *time_buf);

#endif
