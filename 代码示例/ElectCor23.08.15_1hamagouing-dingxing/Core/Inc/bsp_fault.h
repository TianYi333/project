#ifndef __BSP_FAULT_H
#define __BSP_FAULT_H

#include "main.h"
#include "sensor.h"
#include "set.h"
#include "main_logic.h"
#include "bsp_ncv7356.h"
#include "controlded.h"

#define   TPYE_NEAR_TEST       0x20      //CAN通讯标志位----临架检测

extern uint8_t near_flag[2];    //0：左，1：右
extern uint16_t near_pre[2];    //0：左，1：右

void Fault_Near_Function(void);
void Fault_Pre_Function(void);

#endif

