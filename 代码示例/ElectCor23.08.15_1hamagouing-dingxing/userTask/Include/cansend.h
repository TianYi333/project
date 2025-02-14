#ifndef __CANSEND_H__
#define __CANSEND_H__
#include "stm32f4xx_hal.h"
#include "parameter_operation.h"

typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;

#define   COMMUN_ID    0x10
//#define   SINGAL_PITCH_ON     0x01

/**********************************************数组定义*******************************************************/
extern uint8_t array_Cansend_commandR[8];//传递命令变量  创建人蒋博 创建日期2023/02/02

void  Cansendfaction(void);
	

#endif


