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

#define		OUT0				0x01//1 << 0		//ÉýÖù
#define		OUT1				0x02//1 << 1		//½µÖù
#define		OUT2				0x04//1 << 2    //ÍÆÁï	
#define		OUT3				0x08//1 << 3		//À­Áï¡¢ÒÆ¼Ü
#define		OUT4				0x10//1 << 4		//ÉìÇ°»¤°ï°å
#define		OUT5				0x20//1 << 5		//ÊÕÇ°»¤°ï°å
#define		OUT6				0x40//1 << 6		//ÉìÉìËõÁº
#define		OUT7				0x80//1 << 7		//ÊÕÉìËõÁº
#define		OUT8				0x100//1 << 8		//Éì²à»¤
#define		OUT9				0x200//1 << 9    //ÊÕ²à»¤
#define		OUT10				0x400//1 << 10		//ÉìÆ½ºâ
#define		OUT11				0x800//1 << 11   //ÊÕÆ½ºâ
#define		OUT12				0x1000//1 << 12
#define		OUT13				0x2000//1 << 13
#define		OUT14				0x4000//1 << 14
#define		OUT15				0x8000//1 << 15

#define		OUT16				0x01 << 16		//ÉýÖù
#define		OUT17				0x01 << 17		//½µÖù
#define		OUT18				0x01 << 18    //ÍÆÁï	
#define		OUT19				0x01 << 19		//À­Áï¡¢ÒÆ¼Ü
#define		OUT20				0x01 << 20		//ÉìÇ°»¤°ï°å
#define		OUT21				0x01 << 21		//ÊÕÇ°»¤°ï°å
#define		OUT22				0x01 << 22		//ÉìÉìËõÁº
#define		OUT23				0x01 << 23		//ÊÕÉìËõÁº
#define		OUT24				0x01 << 24		//Éì²à»¤
#define		OUT25				0x01 << 25    //ÊÕ²à»¤
#define		OUT26				0x01 << 26		//ÉìÆ½ºâ
#define		OUT27				0x01 << 27   //ÊÕÆ½ºâ
#define		OUT28				0x01 << 28
#define		OUT29				0x01 << 29
#define		OUT30				0x01 << 30
#define		OUT31				0x01 << 31
#define		OUT32				0x01 << 32
#define		OUT33				0x01 << 33

/*in and out  ¹©ÒººÍÅÅÒº*/
#define COLUMN_IN                         OUT0    //ÉýÖù
#define COLUMN_OUT                        OUT1    //½µÖù
#define FRONT_FACE_SPRAG_IN               OUT4    //ÉìÇ°»¤°ï                                                
#define FRONT_FACE_SPRAG_OUT              OUT5   //ÊÕÇ°»¤°ï 
#define SIDE_FACE_SPRAG_IN                OUT8    //Éì²à»¤°ï                                                
#define SIDE_FACE_SPRAG_OUT               OUT9    //ÊÕ²à»¤°ï
#define EXTENSIBLE_CANOPY_IN              OUT6    //ÉìÉìËõÁº
#define EXTENSIBLE_CANOPY_OUT             OUT7    //ÊÕÉìËõÁº
#define	SCRAPER_IN                        OUT2    //ÍÆÁï
#define	FRAME_MOVE          							OUT3    //ÒÆ¼Ü
#define BALANCE_BEAM_IN                   OUT10   //ÉìÆ½ºâÁº
#define BALANCE_BEAM_OUT                  OUT11    //ÊÕÆ½ºâÁº

#define COLUMN_IN_STOP                    (~OUT0)    //ÉýÖùÍ£Ö¹
#define COLUMN_OUT_STOP                   (~OUT1)    //½µÖùÍ£Ö¹
#define FRONT_FACE_SPRAG_IN_STOP          (~OUT4)   //ÉìÇ°»¤°ïÍ£Ö¹                                                
#define FRONT_FACE_SPRAG_OUT_STOP         (~OUT5)   //ÊÕÇ°»¤°ïÍ£Ö¹
#define SIDE_FACE_SPRAG_IN_STOP           (~OUT8)    //Éì²à»¤°ïÍ£Ö¹                                                
#define SIDE_FACE_SPRAG_OUT_STOP          (~OUT9)    //ÊÕ²à»¤°ïÍ£Ö¹
#define EXTENSIBLE_CANOPY_IN_STOP         (~OUT6)    //ÉìÇ°Ì½ÁºÍ£Ö¹
#define EXTENSIBLE_CANOPY_OUT_STOP        (~OUT7)    //ÊÕÇ°Ì½ÁºÍ£Ö¹
#define	SCRAPER_IN_STOP                   (~OUT2)    //ÍÆÁïÍ£Ö¹
#define	FRAME_MOVE_STOP          					(~OUT3)    //ÒÆ¼ÜÍ£Ö¹
#define BALANCE_BEAM_IN_STOP              (~OUT10)    //ÉìÆ½ºâÁºÍ£Ö¹
#define BALANCE_BEAM_OUT_STOP             (~OUT11)    //ÊÕÆ½ºâÁºÍ£Ö¹



void MCZ33996_INIT(void);
void OPEN_MCZ33996_OUT(uint32_t out_num);
void MCZ_SOFT_SPI_RW_MODE3( uint32_t write_dat );


#endif
