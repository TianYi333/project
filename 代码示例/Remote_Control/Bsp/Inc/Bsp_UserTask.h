#ifndef __BSP_USERTASK_H__
#define __BSP_USERTASK_H__

#include "main.h"


/* ������ */

extern TaskHandle_t Key_Board_Handle;         //������
extern TaskHandle_t Display_Handle;           //��Ļ��ʾ
extern TaskHandle_t Analyze_Handle;           //����
extern TaskHandle_t Action_Handle;            //����
extern TaskHandle_t Zigbee_Receive_Handle;    //zigbee����


/* ������ */

void CreatTask(void);           //�����������������ã�

void AppTaskCreate(void);		    // ��������ִ�У�

#endif
