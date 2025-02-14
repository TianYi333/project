#include "Bsp_Action.h"

QueueHandle_t Queue_Clean_Red_Handle;    //解析任务发送控制命令至动作任务

uint8_t prepare_send[8] = {1, 2, 3, 4, 5, 6, 7, 8};                         //预备要发送的信息

uint8_t ifred;

void Action_Control_Function(void)		           // 下发动作
{
  uint8_t clean_msg[8];
  uint8_t time = 100;
  
  while(1)
  {
    
    if(xQueueReceive(Queue_Clean_Red_Handle, prepare_send, 100) == pdFALSE)
    {
      if(time < 100)
      {
        time++;
      }
      
      if(time == 80)
      {
        clean_msg[0] = DIS_CLEAN_RED_MESSAGE;
        xQueueSend(Queue_Analyze_To_Display_Handle, clean_msg, 10);
      }
    }
    else
    {
      time = 80 - prepare_send[0] * 10;
    }
    
////    
////    switch(prepare_send[1] & 0xF0)
////    {
////      case 0x00:
////        while(1)
////        {
////          
////        }
////      break;
////      
////      case 0x10:
////        
////      break;
////    }
  }
}

