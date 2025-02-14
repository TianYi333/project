#ifndef __BSP_OLED19264_H__
#define __BSP_OLED19264_H__

#include "stm32f4xx_hal.h"
#include "bsp_w25q128.h"

#define		OLED_I2C_PORT 		GPIOG
#define		OLED_I2C_RES_PORT GPIOD

#define		OLED_I2C_SDA 	GPIO_PIN_10
#define		OLED_I2C_SCK 	GPIO_PIN_11
#define		OLED_I2C_RES 	GPIO_PIN_7
#define		OLED_I2C_CS	 	GPIO_PIN_9

#define 	OLED_I2C_SDA_H			HAL_GPIO_WritePin(OLED_I2C_PORT, OLED_I2C_SDA, GPIO_PIN_SET)
#define 	OLED_I2C_SDA_L			HAL_GPIO_WritePin(OLED_I2C_PORT, OLED_I2C_SDA, GPIO_PIN_RESET)

#define 	OLED_I2C_SCL_H			HAL_GPIO_WritePin(OLED_I2C_PORT, OLED_I2C_SCK, GPIO_PIN_SET)
#define 	OLED_I2C_SCL_L			HAL_GPIO_WritePin(OLED_I2C_PORT, OLED_I2C_SCK, GPIO_PIN_RESET)

#define		OLED_RESET_START		HAL_GPIO_WritePin(OLED_I2C_RES_PORT,OLED_I2C_RES,GPIO_PIN_RESET)
#define		OLED_RESET_END			HAL_GPIO_WritePin(OLED_I2C_RES_PORT,OLED_I2C_RES,GPIO_PIN_SET)

static void Delay(void);


static uint8_t I2C_SDA_READ(void);
void I2C_Start(void);
void I2C_Stop(void);
void I2C_SendByte(uint8_t _ucByte);
uint8_t I2C_WaitAck(void);
void I2C_Ack(void);
void I2C_NAck(void);
uint8_t I2C_ReadByte(void);


void OLED19264_WRITE_COMMAND(uint8_t com);
void OLED19264_WRITE_DATA(uint8_t data);

void OLED19264_INIT(void);
void OLED19264_RESET(void);
void Clear_Screen(void);

void display_graphic_16x16_cn(uint8_t page,uint8_t column,uint8_t *dp);
void display_graphic_8x16_en(uint8_t page,uint8_t column,uint8_t *dp);

void Clear_Block(uint8_t page_start,uint8_t column_start,uint8_t page_end,uint8_t column_end);
void Display_String(uint8_t page,uint8_t column,char *p);
void Display_String_Reverse(uint8_t page,uint8_t column,char *p);
void Display_Num(uint8_t page,uint8_t column,uint8_t Number);
void Display_Singal_Num(uint8_t page,uint8_t column,char Number);
extern const unsigned char angel1[32];
extern const unsigned char angel2[32];

extern void display_graphic_128x64(unsigned char page,unsigned char column,unsigned char *dp);

extern void OLED_Delay(uint32_t time);

#endif
