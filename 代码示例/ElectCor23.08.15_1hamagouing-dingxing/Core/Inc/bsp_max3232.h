#ifndef __BSP_MAX3232_H__
#define __BSP_MAX3232_H__

#include "stm32f4xx_hal.h"

extern UART_HandleTypeDef huart7;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart6;

#define RS232_1	huart7
#define RS232_2	huart1
#define RS232_3	huart6

#endif
