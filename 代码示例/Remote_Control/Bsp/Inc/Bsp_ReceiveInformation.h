#ifndef __BSP_RECEIVEINFORMATION_H__
#define __BSP_RECEIVEINFORMATION_H__

#include "main.h"

extern QueueHandle_t Queue_Zigbee_Receive_Handle;    //zigbee���ն���

extern BaseType_t xHigherPriorityTaskWoken;

void Zigbee_Receive_Function(void);

#endif
