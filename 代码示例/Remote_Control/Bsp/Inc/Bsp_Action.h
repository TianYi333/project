#ifndef __BSP_ACTION_H__
#define __BSP_ACTION_H__

#include "main.h"

extern QueueHandle_t Queue_Clean_Red_Handle;    //���������Ϳ�����������������

extern uint8_t ifred;

/* ������ */

void Action_Control_Function(void);		    // �·���������

#endif
