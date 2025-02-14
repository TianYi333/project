#ifndef __BSP_MCA33996_H__
#define __BSP_MCA33996_H__

#include "stm32f4xx_hal.h"

#define		MCZ_SPI_PORT					GPIOD
#define		MCZ_SPI_RST_PORT			GPIOC

#define 	MCZ_CS_PIN				GPIO_PIN_3
#define 	MCZ_SCK_PIN				GPIO_PIN_2
#define 	MCZ_RES_PIN				GPIO_PIN_12
#define 	MCZ_MISO_PIN			GPIO_PIN_0
#define 	MCZ_MOSI_PIN			GPIO_PIN_1

#define		MCZ_SET					HAL_GPIO_WritePin(MCZ_SPI_RST_PORT, MCZ_RES_PIN, GPIO_PIN_SET)
#define		MCZ_RESET				HAL_GPIO_WritePin(MCZ_SPI_RST_PORT, MCZ_RES_PIN, GPIO_PIN_RESET)

#define 	MCZ_MOSI_H  		HAL_GPIO_WritePin(MCZ_SPI_PORT, MCZ_MOSI_PIN, GPIO_PIN_SET)  
#define 	MCZ_MOSI_L  		HAL_GPIO_WritePin(MCZ_SPI_PORT, MCZ_MOSI_PIN, GPIO_PIN_RESET) 

#define 	MCZ_SCK_H   		HAL_GPIO_WritePin(MCZ_SPI_PORT, MCZ_SCK_PIN, GPIO_PIN_SET)  
#define 	MCZ_SCK_L   		HAL_GPIO_WritePin(MCZ_SPI_PORT, MCZ_SCK_PIN, GPIO_PIN_RESET)  

#define 	MCZ_CSS_H   		HAL_GPIO_WritePin(MCZ_SPI_PORT, MCZ_CS_PIN, GPIO_PIN_SET)  
#define 	MCZ_CSS_L   		HAL_GPIO_WritePin(MCZ_SPI_PORT, MCZ_CS_PIN, GPIO_PIN_RESET) 

#define		OUT0				0x01//1 << 0		//����
#define		OUT1				0x02//1 << 1		//����
#define		OUT2				0x04//1 << 2    //����	
#define		OUT3				0x08//1 << 3		//����Ƽ�
#define		OUT4				0x10//1 << 4		//��ǰ�����
#define		OUT5				0x20//1 << 5		//��ǰ�����
#define		OUT6				0x40//1 << 6		//��������
#define		OUT7				0x80//1 << 7		//��������
#define		OUT8				0x100//1 << 8		//��໤
#define		OUT9				0x200//1 << 9    //�ղ໤
#define		OUT10				0x400//1 << 10		//��ƽ��
#define		OUT11				0x800//1 << 11   //��ƽ��
#define		OUT12				0x1000//1 << 12
#define		OUT13				0x2000//1 << 13
#define		OUT14				0x4000//1 << 14
#define		OUT15				0x8000//1 << 15

#define		OUT16				0x01 << 16		//����
#define		OUT17				0x01 << 17		//����
#define		OUT18				0x01 << 18    //����	
#define		OUT19				0x01 << 19		//����Ƽ�
#define		OUT20				0x01 << 20		//��ǰ�����
#define		OUT21				0x01 << 21		//��ǰ�����
#define		OUT22				0x01 << 22		//��������
#define		OUT23				0x01 << 23		//��������
#define		OUT24				0x01 << 24		//��໤
#define		OUT25				0x01 << 25    //�ղ໤
#define		OUT26				0x01 << 26		//��ƽ��
#define		OUT27				0x01 << 27   //��ƽ��
#define		OUT28				0x01 << 28
#define		OUT29				0x01 << 29
#define		OUT30				0x01 << 30
#define		OUT31				0x01 << 31
#define		OUT32				0x01 << 32
#define		OUT33				0x01 << 33

/*in and out  ��Һ����Һ*/
#define COLUMN_IN                         OUT0    //����
#define COLUMN_OUT                        OUT1    //����
#define FRONT_FACE_SPRAG_IN               OUT4    //��ǰ����                                                
#define FRONT_FACE_SPRAG_OUT              OUT5   //��ǰ���� 
#define SIDE_FACE_SPRAG_IN                OUT8    //��໤��                                                
#define SIDE_FACE_SPRAG_OUT               OUT9    //�ղ໤��
#define EXTENSIBLE_CANOPY_IN              OUT6    //��������
#define EXTENSIBLE_CANOPY_OUT             OUT7    //��������
#define	SCRAPER_IN                        OUT2    //����
#define	FRAME_MOVE          							OUT3    //�Ƽ�
#define BALANCE_BEAM_IN                   OUT10   //��ƽ����
#define BALANCE_BEAM_OUT                  OUT11    //��ƽ����

#define COLUMN_IN_STOP                    (~OUT0)    //����ֹͣ
#define COLUMN_OUT_STOP                   (~OUT1)    //����ֹͣ
#define FRONT_FACE_SPRAG_IN_STOP          (~OUT4)   //��ǰ����ֹͣ                                                
#define FRONT_FACE_SPRAG_OUT_STOP         (~OUT5)   //��ǰ����ֹͣ
#define SIDE_FACE_SPRAG_IN_STOP           (~OUT8)    //��໤��ֹͣ                                                
#define SIDE_FACE_SPRAG_OUT_STOP          (~OUT9)    //�ղ໤��ֹͣ
#define EXTENSIBLE_CANOPY_IN_STOP         (~OUT6)    //��ǰ̽��ֹͣ
#define EXTENSIBLE_CANOPY_OUT_STOP        (~OUT7)    //��ǰ̽��ֹͣ
#define	SCRAPER_IN_STOP                   (~OUT2)    //����ֹͣ
#define	FRAME_MOVE_STOP          					(~OUT3)    //�Ƽ�ֹͣ
#define BALANCE_BEAM_IN_STOP              (~OUT10)    //��ƽ����ֹͣ
#define BALANCE_BEAM_OUT_STOP             (~OUT11)    //��ƽ����ֹͣ



void MCZ33996_INIT(void);
void OPEN_MCZ33996_OUT(uint32_t out_num);
void MCZ_SOFT_SPI_RW_MODE3( uint32_t write_dat );


#endif
