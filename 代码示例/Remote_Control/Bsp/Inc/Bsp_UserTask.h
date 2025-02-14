#ifndef __BSP_USERTASK_H__
#define __BSP_USERTASK_H__

#include "main.h"


/* 任务句柄 */

extern TaskHandle_t Key_Board_Handle;         //按键板
extern TaskHandle_t Display_Handle;           //屏幕显示
extern TaskHandle_t Analyze_Handle;           //解析
extern TaskHandle_t Action_Handle;            //动作
extern TaskHandle_t Zigbee_Receive_Handle;    //zigbee接收


/* 任务函数 */

void CreatTask(void);           //创建任务（主函数调用）

void AppTaskCreate(void);		    // 创建任务（执行）

#endif
