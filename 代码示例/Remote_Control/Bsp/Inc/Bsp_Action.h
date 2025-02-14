#ifndef __BSP_ACTION_H__
#define __BSP_ACTION_H__

#include "main.h"

extern QueueHandle_t Queue_Clean_Red_Handle;    //解析任务发送控制命令至动作任务

extern uint8_t ifred;

/* 任务函数 */

void Action_Control_Function(void);		    // 下发动作任务

#endif
