#ifndef __BSP_HD7279_H__
#define __BSP_HD7279_H__

#include "stm32f4xx_hal.h"
#include "BSP_LEDS.h"

#define		KEY_I2C_PORT	GPIOG

#define		KEY_I2C_CS		GPIO_PIN_13
#define		KEY_I2C_SCK		GPIO_PIN_14
#define		KEY_I2C_SDA		GPIO_PIN_15

#define 	IIC_SDA_1			HAL_GPIO_WritePin(KEY_I2C_PORT, KEY_I2C_SDA, GPIO_PIN_SET)  
#define 	IIC_SDA_0			HAL_GPIO_WritePin(KEY_I2C_PORT, KEY_I2C_SDA, GPIO_PIN_RESET)  

#define 	IIC_SCL_1			HAL_GPIO_WritePin(KEY_I2C_PORT, KEY_I2C_SCK, GPIO_PIN_SET)  
#define 	IIC_SCL_0			HAL_GPIO_WritePin(KEY_I2C_PORT, KEY_I2C_SCK, GPIO_PIN_RESET)  

#define 	HD7279_CS_H						HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET)
#define 	HD7279_CS_L						HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET)


#define LED_SINGAL          0x00

#define LED_GROUPRIGHT         0x01

#define LED_GROUPLEFT        0x02

#define LED_SETTING_NUM         0x03

#define LED_NORMOL        0x04

#define LED_BLINK        0x05
#define LED_SETING       0x06


#define KEY1_VALUE	0x74
#define KEY2_VALUE	0x76
#define KEY3_VALUE	0x75
#define KEY4_VALUE	0x72
#define KEY5_VALUE	0x6c
#define KEY6_VALUE	0x6e
#define KEY7_VALUE	0x6d
#define KEY8_VALUE	0x6a

#define KEY9_VALUE	0x64
#define KEY10_VALUE	0x66
#define KEY11_VALUE	0x65
#define KEY12_VALUE	0x62
#define KEY13_VALUE	0x7c
#define KEY14_VALUE	0x5c
#define KEY15_VALUE	0x5a
#define KEY16_VALUE	0x7a

#define KEY17_VALUE	0x5f
#define KEY18_VALUE	0x5e
#define KEY19_VALUE	0x5d
#define KEY20_VALUE	0x5b
#define KEY21_VALUE	0x7f
#define KEY22_VALUE	0x7e
#define KEY23_VALUE	0x7d
#define KEY24_VALUE	0x63
										

#define KEY26_VALUE	0x73
#define KEY27_VALUE	0x7b
#define KEY28_VALUE	0x6b

#define KEY30_VALUE	0x6f
#define KEY31_VALUE	0x67
#define KEY32_VALUE	0x77

#define LED3_VALUE4	    0x04
#define LED4_VALUE4  	0x40
#define LED7_VALUE4	    0x01
#define LED8_VALUE4	    0x20
#define LED11_VALUE4	0x02
#define LED12_VALUE4	0x10
#define LED15_VALUE4	0x08
#define LED16_VALUE4	0x80

#define LED1_VALUE5	    0x40
#define LED2_VALUE5	    0x04
#define LED5_VALUE5	    0x20
#define LED6_VALUE5	    0x01
#define LED9_VALUE5	    0x10
#define LED10_VALUE5	0x02
#define LED13_VALUE5	0x80
#define LED14_VALUE5	0x08


#define LED19_VALUE7	0x02
#define LED20_VALUE7	0x08
#define LED22_VALUE7	0x01
#define LED23_VALUE7	0x04
#define LED24_VALUE7	0x10
#define LED26_VALUE7	0x40
#define LED27_VALUE7	0x80
#define LED28_VALUE7	0x20



#define LED17_VALUE6	0x08
#define LED18_VALUE6	0x02
#define LED21_VALUE6	0x80
#define LED30_VALUE6	0x20
#define LED31_VALUE6	0x10
#define LED32_VALUE6	0x40
#define NORMAL_VALUE6	0x01
#define BLINK_VALUE6	0x04




#define READ_SDA  HAL_GPIO_ReadPin(GPIOG,GPIO_PIN_15)  

void IIC_Init(void);                //��ʼ��IIC��IO��				 
void IIC_Start(void);				//����IIC��ʼ�ź�
void IIC_Stop(void);	  			//����IICֹͣ�ź�
void IIC_Send_Byte(uint8_t txd);			//IIC����һ���ֽ�
uint8_t IIC_Read_Byte(uint8_t ack);//IIC��ȡһ���ֽ�
uint8_t IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void IIC_Ack(void);					//IIC����ACK�ź�
void IIC_NAck(void);	
void CH455_init(void);
//�������ʾ
void CH455_Display(uint8_t digital,uint8_t num);		
/* CH450�ĳ��������룬���������I2C���ݣ���ô��8λӦ������1λ */
void CH450_Write(unsigned short cmd);	//д����

uint8_t CH455_Key_Read(void);
#define CH450_SYSOFF    0x0400					// �ر���ʾ���رռ���
#define CH450_SYSON1    0x0401					// ������ʾ
#define CH450_SYSON2    0x0403					// ������ʾ������
#define CH450_GET_KEY	0x0700					// ��ȡ����,���ذ�������

#define CH450_DIG2      0x1200		            // �����λ2��ʾ,�����8λ����
#define CH450_DIG3      0x1300		            // �����λ3��ʾ,�����8λ����
#define CH450_DIG4      0x1400		            // �����λ4��ʾ,�����8λ����
#define CH450_DIG5      0x1500					// �����λ5��ʾ,�����8λ����
#define CH450_DIG6      0x1600					// �����λ6��ʾ,�����8λ����
#define CH450_DIG7      0x1700		            // �����λ7��ʾ,�����8λ����

// ��Ч��������
#define		CH450_KEY_MIN		0x42
#define		CH450_KEY_MAX		0x7F

// CH450�ӿڶ���
#define		CH450_I2C_ADDR1		0x40			// CH450�ĵ�ַ
#define		CH450_I2C_MASK		0x3E			// CH450�ĸ��ֽ���������

extern const unsigned char BCD_decode_tab[0x10];
void	CH450_buf_index( unsigned char index, unsigned char dat );
void	CH450_buf_write( unsigned short cmd ) ;
void LED_Management_Fanction(uint8_t LED_Light);
#endif
