#include "Bsp_Analyze.h"


QueueHandle_t Queue_Step_Stop_Handle;    //ֹͣ������Ϣ

/*��¼��ǰ״̬
  0������״̬��
  1��ѡ�мܺ�״̬��
*/
uint8_t current_state;
//���ڼ�¼����ļܺ�/����
uint8_t enter_num[3];
//���ڼ�¼������ǵڼ�������
uint8_t enter_len;
//������ʾ������
int16_t print_num;
//�ܿؼܺ�
uint8_t control_num;
//��ǰ���涯��
uint8_t choose_action;
//���鷽��
uint8_t m_dir;

uint8_t dis_msg[8];   //��Ļ��ʾ��Ϣ
uint8_t send_msg[12]; //�·���Ϣ
uint8_t recv_msg[8];  //������Ϣ

uint8_t m_msg[8];
uint8_t group_choose; //����ʱ�������λ��

struct group_msg group_para;

// ��������
void Analyze_Function(void)
{
  uint8_t m_key;
  uint8_t i, ifrecv = 0;
  uint16_t m_crc;
  
  //��ʾ����----0
  dis_msg[0] = DIS_MAIN_NUM;
  dis_msg[1] = print_num;
  xQueueSend(Queue_Analyze_To_Display_Handle, dis_msg, 10);
  enter_len = 0;
  
  send_msg[0] = 0x55;
  send_msg[9] = 0xAA;
  
  while(1)
  {
//    HAL_UART_Transmit(&huart3, (uint8_t *)send_msg, strlen(send_msg), 100); 
    xQueueReceive(Queue_KeyBoard_To_Analyze_Handle, &m_key, portMAX_DELAY);
    
//    if((m_key & 0xF0) == 0xE0)
//    {
//      
//      dis_msg[0] = DIS_STOP;
//      xQueueSend(Queue_Analyze_To_Display_Handle, dis_msg, 10);
//      
//      send_msg[0] = 0xE2;
//      send_msg[1] = 0xE2;
//      send_msg[2] = 0xE2;
//      send_msg[3] = 0xE2;
//      send_msg[4] = 0xE2;
//      send_msg[5] = 0xE2;
//      send_msg[6] = 0xE2;
//      send_msg[7] = 0xE2;
//      vTaskDelay(100);
//      while(1)
//      {
//        //��ѭ�����ͼ�ͣ����Ҫ�ϵ�����
//        HAL_UART_Transmit(&huart1, send_msg, 8, 10);
//        vTaskDelay(10);
//      }
//    }
    switch(current_state)
    {
      case STATE_IDEL:                                        //����״̬
        if((m_key & 0xF0) == 0x00)          //���ּ�
        {
          enter_len ++;
      
          if(enter_len > 3)
          {
            //��������
            print_num = 0;
            //��ʾ����----�ص�����0
            dis_msg[0] = DIS_MAIN_NUM;
            dis_msg[1] = print_num;
            xQueueSend(Queue_Analyze_To_Display_Handle, dis_msg, 10);
            enter_len = 0;
          }
          else
          {
            //��¼����
            enter_num[enter_len] = m_key;
            print_num = print_num * 10 + m_key;
            if(print_num > 255)
            {
              //��ʾ����----�ص�����0
              print_num = 0;
              enter_len = 0;
              
              dis_msg[0] = DIS_MAIN_NUM;
              dis_msg[1] = print_num;
              xQueueSend(Queue_Analyze_To_Display_Handle, dis_msg, 10);
              continue;
            }
            //��ʾ����
            dis_msg[0] = DIS_MAIN_NUM;
            dis_msg[1] = print_num;
            xQueueSend(Queue_Analyze_To_Display_Handle, dis_msg, 10);
          }
        }
        else if((m_key & 0xF0) == 0x30)     //���ܼ�
        {
          if(m_key == 0x31)         //ȷ��
          {
            enter_len = 0;
            //��鲢ȷ��print_num�ļܺ�
            if(print_num > 255)
            {
              //�������ܺ�����
              print_num = 0;
              memset(enter_num, 0, 3);
              //����������
              dis_msg[0] = DIS_MAIN_NUM;
              dis_msg[1] = 0;
              xQueueSend(Queue_Analyze_To_Display_Handle, dis_msg, 10);
              break;
            }
            //ѡ��Ŀ��ܺ�
            control_num = print_num;
            //��������
            //��ʾ������
            dis_msg[0] = DIS_CONNECTING;
            xQueueSend(Queue_Analyze_To_Display_Handle, dis_msg, 10);

            //zigbee����
            send_msg[1] = MESSAGE_CONNECT;
            send_msg[2] = print_num;
            
            m_crc = CRC16(send_msg, 10);
            
            send_msg[10] = m_crc >> 8;
            send_msg[11] = m_crc;
            
            xQueueReset(Queue_Zigbee_Receive_Handle);
            
            for(i = 0; i < 3; i++)
            {
              HAL_UART_Transmit(&huart3, (uint8_t *)send_msg, 12, 100); 
            }
            
            ifrecv = 0;
            //���ȴ�5s
            for(i = 0; i < 100; i++)
            {
              if(xQueueReceive(Queue_Zigbee_Receive_Handle, recv_msg, 50) == pdTRUE)   //���ȴ�5s
              {
                //�����ж�
                if(recv_msg[0] == 0xCC && recv_msg[1] == print_num)
                {
                  ifrecv = 1;
                  break;
                }
              }
              //�ɼ����жϼ�ͣ����
              
            }
            if(ifrecv == 0)
            {
              dis_msg[0] = DIS_CONNECTION_FAILED;
              xQueueSend(Queue_Analyze_To_Display_Handle, dis_msg, 10);
              continue;
            }
            //����ѡ�н���״̬
            current_state = STATE_SELECTION;
            //Ĭ��ѡ�е�1������
            choose_action = 0;
            //��ʾ���ƽ���
            memcpy(dis_msg, recv_msg, 8);
            xQueueSend(Queue_Analyze_To_Display_Handle, dis_msg, 10);
          }
          else if(m_key == 0x32)    //ȡ��
          {
            print_num = 0;
            enter_len = 0;
            current_state = STATE_IDEL;
            //��ʾ������
            //����
            dis_msg[0] = DIS_JUMP_MAIN;
            xQueueSend(Queue_Analyze_To_Display_Handle, dis_msg, 10);
            //����Ĭ��0
            dis_msg[0] = DIS_MAIN_NUM;
            dis_msg[1] = print_num;
            xQueueSend(Queue_Analyze_To_Display_Handle, dis_msg, 10);
            
            send_msg[1] = 0xC3;
              
            m_crc = CRC16(send_msg, 10);
            
            send_msg[10] = m_crc >> 8;
            send_msg[11] = m_crc;
            
            HAL_UART_Transmit(&huart3, send_msg, 12, 10);
          }
        }
        else if(m_key == 0x21)           //����
        {
          Key_Group();
        }
        break;
      case STATE_SELECTION:                               //ѡ��״̬
        switch(m_key & 0xF0)
        {
          case 0x00:    //��ݼ�����
            
            if(control_num == 0)
            {
              break;
            }
            dis_msg[0] = DIS_IN_ACTION;
            xQueueSend(Queue_Analyze_To_Display_Handle, dis_msg, 10);
            
            send_msg[1] = SIGNAL_SEND_MESSAGE;
            send_msg[2] = print_num;
            send_msg[3] = selection_menu[m_key].action;
          
            m_crc = CRC16(send_msg, 10);
          
            send_msg[10] = m_crc >> 8;
            send_msg[11] = m_crc;
          
            HAL_UART_Transmit(&huart3, send_msg, 12, 10);
          
            break;
          
          case 0x10:    //�����
            
            switch(m_key)
            {
              case 0x11:    //��----�л���������            
                choose_action = (choose_action + ACTION_MAX - 1) % ACTION_MAX;

                //�������Ϸ�
                dis_msg[0] = DIS_DIR_UP;
                dis_msg[1] = choose_action;
                xQueueSend(Queue_Analyze_To_Display_Handle, dis_msg, 10);
                break;
              
              case 0x12:    //��----�л���������
                choose_action = (choose_action + ACTION_MAX + 1) % ACTION_MAX;

                //�������·�
                dis_msg[0] = DIS_DIR_DOWN;
                dis_msg[1] = choose_action;
                xQueueSend(Queue_Analyze_To_Display_Handle, dis_msg, 10);
                break;
              
              case 0x13:    //��----�л��ܺţ�����
                if(print_num == NUM_MIN)
                {
                  break;
                }
                
                //�ڰ����л���֮ǰ��������ݼ���˵�ѡ����Ч
                control_num = 0;
                
                
                
                //ȡ��ѡ��
                send_msg[1] = 0xC3;
              
                m_crc = CRC16(send_msg, 10);
                
                send_msg[10] = m_crc >> 8;
                send_msg[11] = m_crc;
                
                HAL_UART_Transmit(&huart3, send_msg, 12, 10);
                
                
                
                print_num--;
                
                dis_msg[0] = DIS_DIR_LR;
                dis_msg[1] = print_num;
                xQueueSend(Queue_Analyze_To_Display_Handle, dis_msg, 10);
                break;
              
              case 0x14:    //��----�л��ܺţ��ӣ�
                if(print_num == NUM_MAX)
                {
                  break;
                }
                
                //�ڰ����л���֮ǰ��������ݼ���˵�ѡ����Ч
                control_num = 0;
                
                
                //ȡ��ѡ��
                send_msg[1] = 0xC3;
              
                m_crc = CRC16(send_msg, 10);
                
                send_msg[10] = m_crc >> 8;
                send_msg[11] = m_crc;
                
                HAL_UART_Transmit(&huart3, send_msg, 12, 10);
                
                
                
                
                print_num++;
                
                dis_msg[0] = DIS_DIR_LR;
                dis_msg[1] = print_num;
                xQueueSend(Queue_Analyze_To_Display_Handle, dis_msg, 10);
                break;
              
            }
            
            break;
          
          case 0x20:    //����
            if(m_key == 0x21)
            {
              
              //ȡ��ѡ��
              send_msg[1] = 0xC3;
            
              m_crc = CRC16(send_msg, 10);
              
              send_msg[10] = m_crc >> 8;
              send_msg[11] = m_crc;
              
              HAL_UART_Transmit(&huart3, send_msg, 12, 10);
              vTaskDelay(20);
              
              Key_Group();
            }
            break;
          case 0x30:    //���ܼ�
            if(m_key == 0x32)    //ȡ��
            {
              print_num = 0;
              enter_len = 0;
              current_state = STATE_IDEL;
              //��ʾ������
              //����
              dis_msg[0] = DIS_JUMP_MAIN;
              xQueueSend(Queue_Analyze_To_Display_Handle, dis_msg, 10);
              //����Ĭ��0
              dis_msg[0] = DIS_MAIN_NUM;
              dis_msg[1] = print_num;
              xQueueSend(Queue_Analyze_To_Display_Handle, dis_msg, 10);
              
              send_msg[1] = 0xC3;
              
              m_crc = CRC16(send_msg, 10);
              
              send_msg[10] = m_crc >> 8;
              send_msg[11] = m_crc;
              
              HAL_UART_Transmit(&huart3, send_msg, 12, 10);
            }
            else if(m_key == 0x31)  //ȷ��
            {
              if(control_num == 0)
              {
                break;
              }
              dis_msg[0] = DIS_IN_ACTION;
              xQueueSend(Queue_Analyze_To_Display_Handle, dis_msg, 10);
              
              send_msg[1] = SIGNAL_SEND_MESSAGE;
              send_msg[2] = print_num;
//              send_msg[3] = enter_key[m_key];
              send_msg[3] = selection_menu[choose_action].action;
              
              m_crc = CRC16(send_msg, 10);
              
              send_msg[10] = m_crc >> 8;
              send_msg[11] = m_crc;
              
              HAL_UART_Transmit(&huart3, send_msg, 12, 10);
            }
            else if(m_key == 0x33)    //�л�
            {
                enter_len = 0;
              //��鲢ȷ��print_num�ļܺ�
              if(print_num > 255)
              {
                //�������ܺ�����
                print_num = 0;
                memset(enter_num, 0, 3);
                //����������
                dis_msg[0] = DIS_MAIN_NUM;
                dis_msg[1] = 0;
                xQueueSend(Queue_Analyze_To_Display_Handle, dis_msg, 10);
                break;
              }
              //ѡ��Ŀ��ܺ�
              control_num = print_num;
              //��������
              //��ʾ������
              dis_msg[0] = DIS_CONNECTING;
              xQueueSend(Queue_Analyze_To_Display_Handle, dis_msg, 10);

              //zigbee����
              send_msg[1] = MESSAGE_CONNECT;
              send_msg[2] = print_num;
              
              m_crc = CRC16(send_msg, 10);
              
              send_msg[10] = m_crc >> 8;
              send_msg[11] = m_crc;
              
              xQueueReset(Queue_Zigbee_Receive_Handle);
              
              for(i = 0; i < 3; i++)
              {
                HAL_UART_Transmit(&huart3, (uint8_t *)send_msg, 12, 100); 
              }
              
              ifrecv = 0;
              //���ȴ�5s
              for(i = 0; i < 100; i++)
              {
                if(xQueueReceive(Queue_Zigbee_Receive_Handle, recv_msg, 50) == pdTRUE)   //���ȴ�5s
                {
                  //�����ж�
                  if(recv_msg[0] == 0xCC && recv_msg[1] == print_num)
                  {
                    ifrecv = 1;
                    break;
                  }
                }
                //�ɼ����жϼ�ͣ����
                
              }
              if(ifrecv == 0)
              {
                dis_msg[0] = DIS_CONNECTION_FAILED;
                xQueueSend(Queue_Analyze_To_Display_Handle, dis_msg, 10);
                continue;
              }
              //����ѡ�н���״̬
              current_state = STATE_SELECTION;
              //Ĭ��ѡ�е�13��������ǰ12���а�����
              choose_action = 12;//������12Ϊ��13��
              //��ʾ���ƽ���
              memcpy(dis_msg, recv_msg, 8);
              xQueueSend(Queue_Analyze_To_Display_Handle, dis_msg, 10);
            }
            break;
            
        }
          break;
      case STATE_GROUP:         //����״̬
        switch(m_key & 0xF0)
        {
//          case 0x00:    //��ݼ�����
//          
//          send_msg[1] = print_num;
//          send_msg[2] = 0x01;
//          send_msg[3] = enter_key[m_key];
//          
//          m_crc = CRC16(send_msg, 10);
//            
//          send_msg[10] = m_crc >> 8;
//          send_msg[11] = m_crc;
//          
//          HAL_UART_Transmit(&huart3, send_msg, 12, 10);
//        
//          break;
        
        case 0x10:    //�����
          
          switch(m_key)
          {
            case 0x11:    //��----�л���������            
              choose_action = (choose_action + G_ACTION_MAX - 1) % G_ACTION_MAX;

              //�������Ϸ�
              dis_msg[0] = DIS_DIR_UP;
              dis_msg[1] = choose_action;
              xQueueSend(Queue_Analyze_To_Display_Handle, dis_msg, 10);
              break;
            
            case 0x12:    //��----�л���������
              choose_action = (choose_action + G_ACTION_MAX + 1) % G_ACTION_MAX;

              //�������·�
              dis_msg[0] = DIS_DIR_DOWN;
              dis_msg[1] = choose_action;
              xQueueSend(Queue_Analyze_To_Display_Handle, dis_msg, 10);
              break;
            
//              case 0x13:    //��----�л��ܺţ�����
//                if(print_num == NUM_MIN)
//                {
//                  break;
//                }
//                
//                //�ڰ����л���֮ǰ��������ݼ���˵�ѡ����Ч
//                control_num = 0;
//                print_num--;
//                
//                dis_msg[0] = DIS_DIR_LR;
//                dis_msg[1] = print_num;
//                xQueueSend(Queue_Analyze_To_Display_Handle, dis_msg, 10);
//                break;
//              
//              case 0x14:    //��----�л��ܺţ��ӣ�
//                if(print_num == NUM_MAX)
//                {
//                  break;
//                }
//                
//                //�ڰ����л���֮ǰ��������ݼ���˵�ѡ����Ч
//                control_num = 0;
//                print_num++;
//                
//                dis_msg[0] = DIS_DIR_LR;
//                dis_msg[1] = print_num;
//                xQueueSend(Queue_Analyze_To_Display_Handle, dis_msg, 10);
//                break;
            
          }
          
          break;
        case 0x20:    //���ܼ�
          if(m_key == 0x22)       //�����
          {
            dis_msg[0] = DIS_GROUP_TO_LEFT;
            xQueueSend(Queue_Analyze_To_Display_Handle, dis_msg, 10);
          }
          else if(m_key == 0x23)  //�ҳ���
          {
            dis_msg[0] = DIS_GROUP_TO_RIGHT;
            xQueueSend(Queue_Analyze_To_Display_Handle, dis_msg, 10);
          }
          break;
        case 0x30:    //���ܼ�
          if(m_key == 0x32)    //ȡ��
          {
            print_num = 0;
            enter_len = 0;
            current_state = STATE_IDEL;
            //��ʾ������
            //����
            dis_msg[0] = DIS_JUMP_MAIN;
            xQueueSend(Queue_Analyze_To_Display_Handle, dis_msg, 10);
            //����Ĭ��0
            dis_msg[0] = DIS_MAIN_NUM;
            dis_msg[1] = print_num;
            xQueueSend(Queue_Analyze_To_Display_Handle, dis_msg, 10);
          }
          else if(m_key == 0x31)  //ȷ��
          {
            if(m_dir == 0xEE)
            {
              dis_msg[0] = DIS_NO_DIR;
              xQueueSend(Queue_Analyze_To_Display_Handle, dis_msg, 10);
              continue;
            }
            
            dis_msg[0] = DIS_CLEAN_RED_MESSAGE;
            xQueueSend(Queue_Analyze_To_Display_Handle, dis_msg, 10);
            
                                                              //0x55 ͷ
            send_msg[1] = GROUP_SEND_MESSAGE;                 //0xC5 ��־λ
            send_msg[2] = print_num;                          //�ܺ�
            send_msg[3] = group_menu[choose_action].action;   //�������
            send_msg[4] = group_para.qua[group_choose];       //��������
            send_msg[5] = last_dir;                           //����
            send_msg[6] = group_last_num;
                                                              //[7]~[8]Ϊ��
            m_crc = CRC16(send_msg, 10);
                                                              //0xAA β
            send_msg[10] = m_crc >> 8;                        //CRCH
            send_msg[11] = m_crc;                             //CRCL
              
            HAL_UART_Transmit(&huart3, send_msg, 12, 10);
          }
          break;
        }
      
        break;
    }
  }
}

void Key_Group(void)
{
  uint8_t i, ifrecv;
  uint16_t m_crc;
  //��ʾ������
  dis_msg[0] = DIS_CONNECTING;
  xQueueSend(Queue_Analyze_To_Display_Handle, dis_msg, 10);
  
//  //zigbee����
//  send_msg[1] = GROUP_CONNECT;
//  send_msg[2] = print_num;
//  
//  m_crc = CRC16(send_msg, 10);
//              
//  send_msg[10] = m_crc >> 8;
//  send_msg[11] = m_crc;
//  
//  xQueueReset(Queue_Zigbee_Receive_Handle);
//  
//  for(i = 0; i < 3; i++)
//  {
//    HAL_UART_Transmit(&huart3, (uint8_t *)send_msg, 12, 100); 
//  }
//            
//  ifrecv = 0;
//  //���ȴ�5s
//  for(i = 0; i < 100; i++)
//  {
//    if(xQueueReceive(Queue_Zigbee_Receive_Handle, recv_msg, 50) == pdTRUE)   //���ȴ�5s
//    {
//      //�����ж�
//      if(1) //������
//      {
//            //������
//        group_para.dir = 2;
//        group_para.qua = 10;
//        
//        ifrecv = 1;
//        break;
//      }
//      if(recv_msg[0] == 0xCA)
//      {
//        group_para.dir = 1;
////        memcpy(group_para.qua, &recv_msg[2], 6);
//        group_para.qua[0] = 3;
//        group_para.qua[1] = 5;
//        group_para.qua[2] = 5;
//        group_para.qua[3] = 5;
//        group_para.qua[4] = 5;
//        group_para.qua[5] = 5;
//        group_choose = 0;
//        ifrecv = 1;
//        last_group_dir = 0;
//        break;
//      }
//    }
//    //�ɼ����жϼ�ͣ����
//    
//  }
//  if(ifrecv == 0)
//  {
//    dis_msg[0] = DIS_CONNECTION_FAILED;
//    xQueueSend(Queue_Analyze_To_Display_Handle, dis_msg, 10);
//    return;
//  }
  
  //Ĭ������
  group_para.dir = 2;
  
  //����������ΪĬ��ֵ���Ƽ�Ϊ3������Ϊ5
  group_para.qua[0] = 3;
  group_para.qua[1] = 5;
  group_para.qua[2] = 5;
  group_para.qua[3] = 5;
  group_para.qua[4] = 5;
  group_para.qua[5] = 5;
  group_choose = 0;
  last_group_dir = 0;
  
  m_dir = 0xEE;     //Ĭ��δѡ����
  
  current_state = STATE_GROUP;
  choose_action = 0; //Ĭ��ѡ�е�һ��
  dis_msg[0] = DIS_JUMP_GROUP;
  dis_msg[1] = group_para.dir;
  dis_msg[2] = group_para.qua[group_choose];
  xQueueSend(Queue_Analyze_To_Display_Handle, dis_msg, 10);
}

