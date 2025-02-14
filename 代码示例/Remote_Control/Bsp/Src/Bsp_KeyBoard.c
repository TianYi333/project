#include "Bsp_KeyBoard.h"

QueueHandle_t Queue_KeyBoard_To_Analyze_Handle;    //用按键后向解析任务发送消息

/*
原理图key值与按键板对应数组：
0x0x开头：数字按键对应数字；
0x11：上，0x12：下，0x13：左，0x14：右；
0x21：成组，0x22：左成组，0x23：右成组；
0x31：确认，0x32：取消，0x33：切换，0x34：取消
*/

//uint8_t enter_key[24] = {0xAA, 0x05, 0x06, 0x01, 0xFF, 0xE1, 
//                         0x02, 0x03, 0x04, 0x07, 0x08 ,0xE2, 
//                         0x11, 0x31, 0x13, 0x14, 0x12, 0x32, 
//                         0x09, 0x21, 0x22, 0x23, 0x00, 0x33};

uint8_t enter_key[24] = {0xAA, 0x07, 0x08, 0x02, 0x01, 0x33, 
                         0x03, 0x04, 0x05, 0x09, 0x00 ,0x31, 
                         0x11, 0x0A, 0x13, 0x14, 0x12, 0x0B, 
                         0x06, 0x21, 0x22, 0x23, 0x34, 0x32};
  
//uint8_t key_time[24];

// 按键板函数
void KeyBoard_Function(void)
{
  vTaskDelay(3000);
  
  while(1)
  {
    //轮训检测按键是否被按下
    if(!KEY1)
    {
      //消抖
      vTaskDelay(50);
      if(!KEY1)
      {
        //触发按键函数
        Enter_Key(1);
      }
    }
    
    if(!KEY2)
    {
      //消抖
      vTaskDelay(50);
      if(!KEY2)
      {
        //触发按键函数
        Enter_Key(2);
      }
    }
    
    if(!KEY3)
    {
      //消抖
      vTaskDelay(50);
      if(!KEY3)
      {
        //触发按键函数
        Enter_Key(3);
      }
    }
    
    if(!KEY4)
    {
      //消抖
      vTaskDelay(50);
      if(!KEY4)
      {
        //触发按键函数
        Enter_Key(4);
      }
    }
    
    if(!KEY5)
    {
      //消抖
      vTaskDelay(50);
      if(!KEY5)
      {
        //触发按键函数
        Enter_Key(5);
      }
    }
    
    if(!KEY6)
    {
      //消抖
      vTaskDelay(50);
      if(!KEY6)
      {
        //触发按键函数
        Enter_Key(6);
      }
    }
    
    if(!KEY7)
    {
      //消抖
      vTaskDelay(50);
      if(!KEY7)
      {
        //触发按键函数
        Enter_Key(7);
      }
    }
    
    if(!KEY8)
    {
      //消抖
      vTaskDelay(50);
      if(!KEY8)
      {
        //触发按键函数
        Enter_Key(8);
      }
    }
    
    if(!KEY9)
    {
      //消抖
      vTaskDelay(50);
      if(!KEY9)
      {
        //触发按键函数
        Enter_Key(9);
      }
    }
    
    if(!KEY10)
    {
      //消抖
      vTaskDelay(50);
      if(!KEY10)
      {
        //触发按键函数
        Enter_Key(10);
      }
    }
    
    if(!KEY11)
    {
      //消抖
      vTaskDelay(50);
      if(!KEY11)
      {
        //触发按键函数
        Enter_Key(11);
      }
    }
    
    if(!KEY12)
    {
      //消抖
      vTaskDelay(50);
      if(!KEY12)
      {
        //触发按键函数
        Enter_Key(12);
      }
    }
    
    if(!KEY13)
    {
      //消抖
      vTaskDelay(50);
      if(!KEY13)
      {
        //触发按键函数
        Enter_Key(13);
      }
    }
    
    if(!KEY14)
    {
      //消抖
      vTaskDelay(50);
      if(!KEY14)
      {
        //触发按键函数
        Enter_Key(14);
      }
    }
    
    if(!KEY15)
    {
      //消抖
      vTaskDelay(50);
      if(!KEY15)
      {
        //触发按键函数
        Enter_Key(15);
      }
    }
    
    if(!KEY16)
    {
      //消抖
      vTaskDelay(50);
      if(!KEY16)
      {
        //触发按键函数
        Enter_Key(16);
      }
    }
    
    if(!KEY17)
    {
      //消抖
      vTaskDelay(50);
      if(!KEY17)
      {
        //触发按键函数
        Enter_Key(17);
      }
    }
    
    if(!KEY18)
    {
      //消抖
      vTaskDelay(50);
      if(!KEY18)
      {
        //触发按键函数
        Enter_Key(18);
      }
    }
    
    if(!KEY19)
    {
      //消抖
      vTaskDelay(50);
      if(!KEY19)
      {
        //触发按键函数
        Enter_Key(19);
      }
    }
    
    if(!KEY20)
    {
      //消抖
      vTaskDelay(50);
      if(!KEY20)
      {
        //触发按键函数
        Enter_Key(20);
      }
    }
    
    if(!KEY21)
    {
      //消抖
      vTaskDelay(50);
      if(!KEY21)
      {
        //触发按键函数
        Enter_Key(21);
      }
    }
    
    if(!KEY22)
    {
      //消抖
      vTaskDelay(50);
      if(!KEY22)
      {
        //触发按键函数
        Enter_Key(22);
      }
    }
    
    if(!KEY23)
    {
      //消抖
      vTaskDelay(50);
      if(!KEY23)
      {
        //触发按键函数
        Enter_Key(23);
      }
    }
  }
}

//是否等待抬起
void Wait_lift(uint8_t key)
{
  if(current_state == STATE_IDEL || current_state == STATE_GROUP)
  {
    //空闲状态和成组状态需要等待抬起
    switch(key)
    {
      case 1:
        while(!KEY1)
        {
          vTaskDelay(10);
        }
        break;
        
      case 2:
        while(!KEY2)
        {
          vTaskDelay(10);
        }
        break;
        
      case 3:
        while(!KEY3)
        {
          vTaskDelay(10);
        }
        break;
        
      case 4:
        while(!KEY4)
        {
          vTaskDelay(10);
        }
        break;
        
      case 5:
        while(!KEY5)
        {
          vTaskDelay(10);
        }
        break;
        
      case 6:
        while(!KEY6)
        {
          vTaskDelay(10);
        }
        break;
        
      case 7:
        while(!KEY7)
        {
          vTaskDelay(10);
        }
        break;
        
      case 8:
        while(!KEY8)
        {
          vTaskDelay(10);
        }
        break;
        
      case 9:
        while(!KEY9)
        {
          vTaskDelay(10);
        }
        break;
        
      case 10:
        while(!KEY10)
        {
          vTaskDelay(10);
        }
        break;
        
      case 11:
        while(!KEY11)
        {
          vTaskDelay(10);
        }
        break;
        
      case 12:
        while(!KEY12)
        {
          vTaskDelay(10);
        }
        break;
        
      case 13:
        while(!KEY13)
        {
          vTaskDelay(10);
        }
        break;
        
      case 14:
        while(!KEY14)
        {
          vTaskDelay(10);
        }
        break;
        
      case 15:
        while(!KEY15)
        {
          vTaskDelay(10);
        }
        break;
        
      case 16:
        while(!KEY16)
        {
          vTaskDelay(10);
        }
        break;
        
      case 17:
        while(!KEY17)
        {
          vTaskDelay(10);
        }
        break;
        
      case 18:
        while(!KEY18)
        {
          vTaskDelay(10);
        }
        break;
        
      case 19:
        while(!KEY19)
        {
          vTaskDelay(10);
        }
        break;
        
      case 20:
        while(!KEY20)
        {
          vTaskDelay(10);
        }
        break;
        
      case 21:
        while(!KEY21)
        {
          vTaskDelay(10);
        }
        break;
        
      case 22:
        while(!KEY22)
        {
          vTaskDelay(10);
        }
        break;
        
      case 23:
        while(!KEY23)
        {
          vTaskDelay(10);
        }
        break;
    }
  }
  else if(current_state == STATE_SELECTION)
  {
    //选择完架号后，动作命令为长按有效
    switch(key)
    {        
      case 5:
        while(!KEY5)
        {
          vTaskDelay(10);
        }
        break;
        
//      case 11:
//        while(!KEY11)
//        {
//          vTaskDelay(10);
//        }
//        break;
        
      case 12:
        while(!KEY12)
        {
          vTaskDelay(10);
        }
        break;
        
//      case 13:
//        while(!KEY13)
//        {
//          vTaskDelay(10);
//        }
//        break;
        
      case 14:
        while(!KEY14)
        {
          vTaskDelay(10);
        }
        break;
        
      case 15:
        while(!KEY15)
        {
          vTaskDelay(10);
        }
        break;
        
      case 16:
        while(!KEY16)
        {
          vTaskDelay(10);
        }
        break;
        
//      case 17:
//        while(!KEY17)
//        {
//          vTaskDelay(10);
//        }
//        break;
        
      case 19:
        while(!KEY19)
        {
          vTaskDelay(10);
        }
        break;
        
      case 20:
        while(!KEY20)
        {
          vTaskDelay(10);
        }
        break;
        
      case 21:
        while(!KEY21)
        {
          vTaskDelay(10);
        }
        break;
        
      case 22:
        while(!KEY22)
        {
          vTaskDelay(10);
        }
        break;
        
      case 23:
        while(!KEY23)
        {
          vTaskDelay(10);
        }
        break;
        
      default:
        
        break;
    }
  }
}

//按键输入
void Enter_Key(uint8_t key)
{
  
  //是否等待抬起
  Wait_lift(key);
        
  //向解析任务发送被触发的按键编号
  xQueueSend(Queue_KeyBoard_To_Analyze_Handle, (void *)&enter_key[key], 10);
}

#if(0)
void Stop_Function(void)
{
  uint8_t send_msg[8];
  uint8_t succeed = 0;
  
  //挂起其他所有任务
  vTaskSuspend(Key_Board_Handle);
  vTaskSuspend(Display_Handle);
  vTaskSuspend(Analyze_Handle);
  vTaskSuspend(Action_Handle);
  vTaskSuspend(Zigbee_Receive_Handle);

  send_msg[0] = 0xE2;
  send_msg[1] = 0xE2;
  send_msg[2] = 0xE2;
  send_msg[3] = 0xE2;
  send_msg[4] = 0xE2;
  send_msg[5] = 0xE2;
  send_msg[6] = 0xE2;
  send_msg[7] = 0xE2;
  
  Display_Str(FIRST_LINE_X, FIRST_LINE_y, "急停", DEFAULT_COLOR_TEXT, 32, 0);
  
  while(1)
  {
    //死循环发送急停，需要断电重启
    HAL_UART_Transmit(&huart3, send_msg, 8, 10);
    vTaskDelay(10);
  }
}

#endif
