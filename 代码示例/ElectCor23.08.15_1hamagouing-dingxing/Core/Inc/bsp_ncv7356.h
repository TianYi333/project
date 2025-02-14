#ifndef __BSP_NCV7356_H__
#define __BSP_NCV7356_H__

#include "stm32f4xx_hal.h"

#define LEFT_CAN 		hcan3
#define RIGHT_CAN 	hcan1
#define BUS_CAN			hcan2

extern CAN_HandleTypeDef hcan1;
extern CAN_HandleTypeDef hcan2;
extern CAN_HandleTypeDef hcan3;
extern uint8_t Flag_ncv7356_cansendeorr;
void CAN_Config(void);

uint8_t CANx_SendNormalData(CAN_HandleTypeDef *hcanX, uint16_t ID, uint8_t *pData, uint16_t Len);
//Default_Para_Con.Setting_Para
#endif
