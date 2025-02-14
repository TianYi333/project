#include "Bsp_KeyBoard.h"

QueueHandle_t Queue_KeyBoard_To_Analyze_Handle;    //�ð������������������Ϣ

/*
ԭ��ͼkeyֵ�밴�����Ӧ���飺
0x0x��ͷ�����ְ�����Ӧ���֣�
0x11���ϣ�0x12���£�0x13����0x14���ң�
0x21�����飬0x22������飬0x23���ҳ��飻
0x31��ȷ�ϣ�0x32��ȡ����0x33���л���0x34��ȡ��
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

// �����庯��
void KeyBoard_Function(void)
{
  vTaskDelay(3000);
  
  while(1)
  {
    //��ѵ��ⰴ���Ƿ񱻰���
    if(!KEY1)
    {
      //����
      vTaskDelay(50);
      if(!KEY1)
      {
        //������������
        Enter_Key(1);
      }
    }
    
    if(!KEY2)
    {
      //����
      vTaskDelay(50);
      if(!KEY2)
      {
        //������������
        Enter_Key(2);
      }
    }
    
    if(!KEY3)
    {
      //����
      vTaskDelay(50);
      if(!KEY3)
      {
        //������������
        Enter_Key(3);
      }
    }
    
    if(!KEY4)
    {
      //����
      vTaskDelay(50);
      if(!KEY4)
      {
        //������������
        Enter_Key(4);
      }
    }
    
    if(!KEY5)
    {
      //����
      vTaskDelay(50);
      if(!KEY5)
      {
        //������������
        Enter_Key(5);
      }
    }
    
    if(!KEY6)
    {
      //����
      vTaskDelay(50);
      if(!KEY6)
      {
        //������������
        Enter_Key(6);
      }
    }
    
    if(!KEY7)
    {
      //����
      vTaskDelay(50);
      if(!KEY7)
      {
        //������������
        Enter_Key(7);
      }
    }
    
    if(!KEY8)
    {
      //����
      vTaskDelay(50);
      if(!KEY8)
      {
        //������������
        Enter_Key(8);
      }
    }
    
    if(!KEY9)
    {
      //����
      vTaskDelay(50);
      if(!KEY9)
      {
        //������������
        Enter_Key(9);
      }
    }
    
    if(!KEY10)
    {
      //����
      vTaskDelay(50);
      if(!KEY10)
      {
        //������������
        Enter_Key(10);
      }
    }
    
    if(!KEY11)
    {
      //����
      vTaskDelay(50);
      if(!KEY11)
      {
        //������������
        Enter_Key(11);
      }
    }
    
    if(!KEY12)
    {
      //����
      vTaskDelay(50);
      if(!KEY12)
      {
        //������������
        Enter_Key(12);
      }
    }
    
    if(!KEY13)
    {
      //����
      vTaskDelay(50);
      if(!KEY13)
      {
        //������������
        Enter_Key(13);
      }
    }
    
    if(!KEY14)
    {
      //����
      vTaskDelay(50);
      if(!KEY14)
      {
        //������������
        Enter_Key(14);
      }
    }
    
    if(!KEY15)
    {
      //����
      vTaskDelay(50);
      if(!KEY15)
      {
        //������������
        Enter_Key(15);
      }
    }
    
    if(!KEY16)
    {
      //����
      vTaskDelay(50);
      if(!KEY16)
      {
        //������������
        Enter_Key(16);
      }
    }
    
    if(!KEY17)
    {
      //����
      vTaskDelay(50);
      if(!KEY17)
      {
        //������������
        Enter_Key(17);
      }
    }
    
    if(!KEY18)
    {
      //����
      vTaskDelay(50);
      if(!KEY18)
      {
        //������������
        Enter_Key(18);
      }
    }
    
    if(!KEY19)
    {
      //����
      vTaskDelay(50);
      if(!KEY19)
      {
        //������������
        Enter_Key(19);
      }
    }
    
    if(!KEY20)
    {
      //����
      vTaskDelay(50);
      if(!KEY20)
      {
        //������������
        Enter_Key(20);
      }
    }
    
    if(!KEY21)
    {
      //����
      vTaskDelay(50);
      if(!KEY21)
      {
        //������������
        Enter_Key(21);
      }
    }
    
    if(!KEY22)
    {
      //����
      vTaskDelay(50);
      if(!KEY22)
      {
        //������������
        Enter_Key(22);
      }
    }
    
    if(!KEY23)
    {
      //����
      vTaskDelay(50);
      if(!KEY23)
      {
        //������������
        Enter_Key(23);
      }
    }
  }
}

//�Ƿ�ȴ�̧��
void Wait_lift(uint8_t key)
{
  if(current_state == STATE_IDEL || current_state == STATE_GROUP)
  {
    //����״̬�ͳ���״̬��Ҫ�ȴ�̧��
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
    //ѡ����ܺź󣬶�������Ϊ������Ч
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

//��������
void Enter_Key(uint8_t key)
{
  
  //�Ƿ�ȴ�̧��
  Wait_lift(key);
        
  //����������ͱ������İ������
  xQueueSend(Queue_KeyBoard_To_Analyze_Handle, (void *)&enter_key[key], 10);
}

#if(0)
void Stop_Function(void)
{
  uint8_t send_msg[8];
  uint8_t succeed = 0;
  
  //����������������
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
  
  Display_Str(FIRST_LINE_X, FIRST_LINE_y, "��ͣ", DEFAULT_COLOR_TEXT, 32, 0);
  
  while(1)
  {
    //��ѭ�����ͼ�ͣ����Ҫ�ϵ�����
    HAL_UART_Transmit(&huart3, send_msg, 8, 10);
    vTaskDelay(10);
  }
}

#endif
