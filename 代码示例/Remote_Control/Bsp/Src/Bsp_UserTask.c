#include "Bsp_UserTask.h"

/* 任务句柄 */

TaskHandle_t Key_Board_Handle = NULL;       //按键板
TaskHandle_t Display_Handle = NULL;         //屏幕显示
TaskHandle_t Analyze_Handle = NULL;         //解析
TaskHandle_t Action_Handle = NULL;          //动作
TaskHandle_t Zigbee_Receive_Handle = NULL;  //zigbee接收
TaskHandle_t Uart_Transmit_Handle = NULL;   //串口发送
TaskHandle_t Batery_Handle = NULL;          //电池电量检测

void CreatTask(void)
{
  xTaskCreate((TaskFunction_t )AppTaskCreate,     /* 任务入口函数 */
                (const char*    )"AppTaskCreate", /* 任务名字 */
                (uint16_t       )512,             /* 任务栈大小 */
                (void*          )NULL,            /* 任务入口函数参数 */
                (UBaseType_t    )1,               /* 任务的优先级 */
                (TaskHandle_t*  )NULL);           /* 任务控制块指针 */
}
uint16_t ifsu;
void AppTaskCreate(void)
{
  taskENTER_CRITICAL();           //进入临界区
  
  Queue_KeyBoard_To_Analyze_Handle = xQueueCreate(4, 1);
  Queue_Clean_Red_Handle = xQueueCreate(8, 8);
  Queue_Step_Stop_Handle = xQueueCreate(1, 1);
  Queue_Analyze_To_Display_Handle = xQueueCreate(8, 8);
  Queue_Zigbee_Receive_Handle = xQueueCreate(8, 8);
  
  xTaskCreate((TaskFunction_t )KeyBoard_Function,   /* 任务入口函数 */
              (const char*    )"KeyBoard",          /* 任务名字 */
              (uint16_t       )64,                  /* 任务栈大小 */
              (void*          )NULL,                /* 任务入口函数参数 */
              (UBaseType_t    )6,                   /* 任务的优先级 */
              (TaskHandle_t*  )&Key_Board_Handle);  /* 任务控制块指针 */
                
  xTaskCreate((TaskFunction_t )Display_Function,    /* 任务入口函数 */
              (const char*    )"Display",           /* 任务名字 */
              (uint16_t       )256,                 /* 任务栈大小 */
              (void*          )NULL,                /* 任务入口函数参数 */
              (UBaseType_t    )6,                   /* 任务的优先级 */
              (TaskHandle_t*  )&Display_Handle);    /* 任务控制块指针 */
                
  xTaskCreate((TaskFunction_t )Analyze_Function,    /* 任务入口函数 */
              (const char*    )"Analyze",           /* 任务名字 */
              (uint16_t       )64,                  /* 任务栈大小 */
              (void*          )NULL,                /* 任务入口函数参数 */
              (UBaseType_t    )6,                   /* 任务的优先级 */
              (TaskHandle_t*  )&Analyze_Handle);    /* 任务控制块指针 */
              
  xTaskCreate((TaskFunction_t )Action_Control_Function,   /* 任务入口函数 */
              (const char*    )"Action",                  /* 任务名字 */
              (uint16_t       )64,                        /* 任务栈大小 */
              (void*          )NULL,                      /* 任务入口函数参数 */
              (UBaseType_t    )6,                         /* 任务的优先级 */
              (TaskHandle_t*  )&Action_Handle);           /* 任务控制块指针 */
              
  xTaskCreate((TaskFunction_t )Zigbee_Receive_Function,   /* 任务入口函数 */
              (const char*    )"Zigbee",                  /* 任务名字 */
              (uint16_t       )64,                        /* 任务栈大小 */
              (void*          )NULL,                      /* 任务入口函数参数 */
              (UBaseType_t    )6,                         /* 任务的优先级 */
              (TaskHandle_t*  )&Zigbee_Receive_Handle);   /* 任务控制块指针 */
              
  xTaskCreate((TaskFunction_t )Uart_Transmit_Function,    /* 任务入口函数 */
              (const char*    )"Uart",                    /* 任务名字 */
              (uint16_t       )64,                        /* 任务栈大小 */
              (void*          )NULL,                      /* 任务入口函数参数 */
              (UBaseType_t    )6,                         /* 任务的优先级 */
              (TaskHandle_t*  )&Uart_Transmit_Handle);    /* 任务控制块指针 */
 ifsu = xTaskCreate((TaskFunction_t )Batery_Function,           /* 任务入口函数 */
              (const char*    )"Batery",                  /* 任务名字 */
              (uint16_t       )16,                        /* 任务栈大小 */
              (void*          )NULL,                      /* 任务入口函数参数 */
              (UBaseType_t    )6,                         /* 任务的优先级 */
              (TaskHandle_t*  )&Batery_Handle);           /* 任务控制块指针 */
  
  vTaskDelete(NULL);              //删除自己
  taskEXIT_CRITICAL();            //退出临界区
}

