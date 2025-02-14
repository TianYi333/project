#include "Bsp_Analyze.h"


QueueHandle_t Queue_Step_Stop_Handle;    //停止单动消息

/*记录当前状态
  0：空闲状态；
  1：选中架号状态；
*/
uint8_t current_state;
//用于记录输入的架号/动作
uint8_t enter_num[3];
//用于记录输入的是第几个数字
uint8_t enter_len;
//用于显示的数字
int16_t print_num;
//受控架号
uint8_t control_num;
//当前界面动作
uint8_t choose_action;
//成组方向
uint8_t m_dir;

uint8_t dis_msg[8];   //屏幕显示信息
uint8_t send_msg[12]; //下发信息
uint8_t recv_msg[8];  //接收信息

uint8_t m_msg[8];
uint8_t group_choose; //成组时光标所在位置

struct group_msg group_para;

// 解析函数
void Analyze_Function(void)
{
  uint8_t m_key;
  uint8_t i, ifrecv = 0;
  uint16_t m_crc;
  
  //显示数字----0
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
//        //死循环发送急停，需要断电重启
//        HAL_UART_Transmit(&huart1, send_msg, 8, 10);
//        vTaskDelay(10);
//      }
//    }
    switch(current_state)
    {
      case STATE_IDEL:                                        //空闲状态
        if((m_key & 0xF0) == 0x00)          //数字键
        {
          enter_len ++;
      
          if(enter_len > 3)
          {
            //超出长度
            print_num = 0;
            //显示数字----回到数字0
            dis_msg[0] = DIS_MAIN_NUM;
            dis_msg[1] = print_num;
            xQueueSend(Queue_Analyze_To_Display_Handle, dis_msg, 10);
            enter_len = 0;
          }
          else
          {
            //记录数字
            enter_num[enter_len] = m_key;
            print_num = print_num * 10 + m_key;
            if(print_num > 255)
            {
              //显示数字----回到数字0
              print_num = 0;
              enter_len = 0;
              
              dis_msg[0] = DIS_MAIN_NUM;
              dis_msg[1] = print_num;
              xQueueSend(Queue_Analyze_To_Display_Handle, dis_msg, 10);
              continue;
            }
            //显示数字
            dis_msg[0] = DIS_MAIN_NUM;
            dis_msg[1] = print_num;
            xQueueSend(Queue_Analyze_To_Display_Handle, dis_msg, 10);
          }
        }
        else if((m_key & 0xF0) == 0x30)     //功能键
        {
          if(m_key == 0x31)         //确认
          {
            enter_len = 0;
            //检查并确认print_num的架号
            if(print_num > 255)
            {
              //超出最大架号限制
              print_num = 0;
              memset(enter_num, 0, 3);
              //主界面清零
              dis_msg[0] = DIS_MAIN_NUM;
              dis_msg[1] = 0;
              xQueueSend(Queue_Analyze_To_Display_Handle, dis_msg, 10);
              break;
            }
            //选中目标架号
            control_num = print_num;
            //尝试链接
            //显示连接中
            dis_msg[0] = DIS_CONNECTING;
            xQueueSend(Queue_Analyze_To_Display_Handle, dis_msg, 10);

            //zigbee发送
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
            //最多等待5s
            for(i = 0; i < 100; i++)
            {
              if(xQueueReceive(Queue_Zigbee_Receive_Handle, recv_msg, 50) == pdTRUE)   //最多等待5s
              {
                //连接判断
                if(recv_msg[0] == 0xCC && recv_msg[1] == print_num)
                {
                  ifrecv = 1;
                  break;
                }
              }
              //可加入判断急停代码
              
            }
            if(ifrecv == 0)
            {
              dis_msg[0] = DIS_CONNECTION_FAILED;
              xQueueSend(Queue_Analyze_To_Display_Handle, dis_msg, 10);
              continue;
            }
            //进入选中界面状态
            current_state = STATE_SELECTION;
            //默认选中第1个动作
            choose_action = 0;
            //显示控制界面
            memcpy(dis_msg, recv_msg, 8);
            xQueueSend(Queue_Analyze_To_Display_Handle, dis_msg, 10);
          }
          else if(m_key == 0x32)    //取消
          {
            print_num = 0;
            enter_len = 0;
            current_state = STATE_IDEL;
            //显示主界面
            //清屏
            dis_msg[0] = DIS_JUMP_MAIN;
            xQueueSend(Queue_Analyze_To_Display_Handle, dis_msg, 10);
            //数字默认0
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
        else if(m_key == 0x21)           //成组
        {
          Key_Group();
        }
        break;
      case STATE_SELECTION:                               //选中状态
        switch(m_key & 0xF0)
        {
          case 0x00:    //快捷键单动
            
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
          
          case 0x10:    //方向键
            
            switch(m_key)
            {
              case 0x11:    //上----切换单动动作            
                choose_action = (choose_action + ACTION_MAX - 1) % ACTION_MAX;

                //界面向上翻
                dis_msg[0] = DIS_DIR_UP;
                dis_msg[1] = choose_action;
                xQueueSend(Queue_Analyze_To_Display_Handle, dis_msg, 10);
                break;
              
              case 0x12:    //下----切换单动动作
                choose_action = (choose_action + ACTION_MAX + 1) % ACTION_MAX;

                //界面向下翻
                dis_msg[0] = DIS_DIR_DOWN;
                dis_msg[1] = choose_action;
                xQueueSend(Queue_Analyze_To_Display_Handle, dis_msg, 10);
                break;
              
              case 0x13:    //左----切换架号（减）
                if(print_num == NUM_MIN)
                {
                  break;
                }
                
                //在按下切换键之前，动作快捷键与菜单选中无效
                control_num = 0;
                
                
                
                //取消选中
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
              
              case 0x14:    //右----切换架号（加）
                if(print_num == NUM_MAX)
                {
                  break;
                }
                
                //在按下切换键之前，动作快捷键与菜单选中无效
                control_num = 0;
                
                
                //取消选中
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
          
          case 0x20:    //成组
            if(m_key == 0x21)
            {
              
              //取消选中
              send_msg[1] = 0xC3;
            
              m_crc = CRC16(send_msg, 10);
              
              send_msg[10] = m_crc >> 8;
              send_msg[11] = m_crc;
              
              HAL_UART_Transmit(&huart3, send_msg, 12, 10);
              vTaskDelay(20);
              
              Key_Group();
            }
            break;
          case 0x30:    //功能键
            if(m_key == 0x32)    //取消
            {
              print_num = 0;
              enter_len = 0;
              current_state = STATE_IDEL;
              //显示主界面
              //清屏
              dis_msg[0] = DIS_JUMP_MAIN;
              xQueueSend(Queue_Analyze_To_Display_Handle, dis_msg, 10);
              //数字默认0
              dis_msg[0] = DIS_MAIN_NUM;
              dis_msg[1] = print_num;
              xQueueSend(Queue_Analyze_To_Display_Handle, dis_msg, 10);
              
              send_msg[1] = 0xC3;
              
              m_crc = CRC16(send_msg, 10);
              
              send_msg[10] = m_crc >> 8;
              send_msg[11] = m_crc;
              
              HAL_UART_Transmit(&huart3, send_msg, 12, 10);
            }
            else if(m_key == 0x31)  //确认
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
            else if(m_key == 0x33)    //切换
            {
                enter_len = 0;
              //检查并确认print_num的架号
              if(print_num > 255)
              {
                //超出最大架号限制
                print_num = 0;
                memset(enter_num, 0, 3);
                //主界面清零
                dis_msg[0] = DIS_MAIN_NUM;
                dis_msg[1] = 0;
                xQueueSend(Queue_Analyze_To_Display_Handle, dis_msg, 10);
                break;
              }
              //选中目标架号
              control_num = print_num;
              //尝试链接
              //显示连接中
              dis_msg[0] = DIS_CONNECTING;
              xQueueSend(Queue_Analyze_To_Display_Handle, dis_msg, 10);

              //zigbee发送
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
              //最多等待5s
              for(i = 0; i < 100; i++)
              {
                if(xQueueReceive(Queue_Zigbee_Receive_Handle, recv_msg, 50) == pdTRUE)   //最多等待5s
                {
                  //连接判断
                  if(recv_msg[0] == 0xCC && recv_msg[1] == print_num)
                  {
                    ifrecv = 1;
                    break;
                  }
                }
                //可加入判断急停代码
                
              }
              if(ifrecv == 0)
              {
                dis_msg[0] = DIS_CONNECTION_FAILED;
                xQueueSend(Queue_Analyze_To_Display_Handle, dis_msg, 10);
                continue;
              }
              //进入选中界面状态
              current_state = STATE_SELECTION;
              //默认选中第13个动作（前12个有按键）
              choose_action = 12;//动作号12为第13个
              //显示控制界面
              memcpy(dis_msg, recv_msg, 8);
              xQueueSend(Queue_Analyze_To_Display_Handle, dis_msg, 10);
            }
            break;
            
        }
          break;
      case STATE_GROUP:         //成组状态
        switch(m_key & 0xF0)
        {
//          case 0x00:    //快捷键单动
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
        
        case 0x10:    //方向键
          
          switch(m_key)
          {
            case 0x11:    //上----切换单动动作            
              choose_action = (choose_action + G_ACTION_MAX - 1) % G_ACTION_MAX;

              //界面向上翻
              dis_msg[0] = DIS_DIR_UP;
              dis_msg[1] = choose_action;
              xQueueSend(Queue_Analyze_To_Display_Handle, dis_msg, 10);
              break;
            
            case 0x12:    //下----切换单动动作
              choose_action = (choose_action + G_ACTION_MAX + 1) % G_ACTION_MAX;

              //界面向下翻
              dis_msg[0] = DIS_DIR_DOWN;
              dis_msg[1] = choose_action;
              xQueueSend(Queue_Analyze_To_Display_Handle, dis_msg, 10);
              break;
            
//              case 0x13:    //左----切换架号（减）
//                if(print_num == NUM_MIN)
//                {
//                  break;
//                }
//                
//                //在按下切换键之前，动作快捷键与菜单选中无效
//                control_num = 0;
//                print_num--;
//                
//                dis_msg[0] = DIS_DIR_LR;
//                dis_msg[1] = print_num;
//                xQueueSend(Queue_Analyze_To_Display_Handle, dis_msg, 10);
//                break;
//              
//              case 0x14:    //右----切换架号（加）
//                if(print_num == NUM_MAX)
//                {
//                  break;
//                }
//                
//                //在按下切换键之前，动作快捷键与菜单选中无效
//                control_num = 0;
//                print_num++;
//                
//                dis_msg[0] = DIS_DIR_LR;
//                dis_msg[1] = print_num;
//                xQueueSend(Queue_Analyze_To_Display_Handle, dis_msg, 10);
//                break;
            
          }
          
          break;
        case 0x20:    //功能键
          if(m_key == 0x22)       //左成组
          {
            dis_msg[0] = DIS_GROUP_TO_LEFT;
            xQueueSend(Queue_Analyze_To_Display_Handle, dis_msg, 10);
          }
          else if(m_key == 0x23)  //右成组
          {
            dis_msg[0] = DIS_GROUP_TO_RIGHT;
            xQueueSend(Queue_Analyze_To_Display_Handle, dis_msg, 10);
          }
          break;
        case 0x30:    //功能键
          if(m_key == 0x32)    //取消
          {
            print_num = 0;
            enter_len = 0;
            current_state = STATE_IDEL;
            //显示主界面
            //清屏
            dis_msg[0] = DIS_JUMP_MAIN;
            xQueueSend(Queue_Analyze_To_Display_Handle, dis_msg, 10);
            //数字默认0
            dis_msg[0] = DIS_MAIN_NUM;
            dis_msg[1] = print_num;
            xQueueSend(Queue_Analyze_To_Display_Handle, dis_msg, 10);
          }
          else if(m_key == 0x31)  //确认
          {
            if(m_dir == 0xEE)
            {
              dis_msg[0] = DIS_NO_DIR;
              xQueueSend(Queue_Analyze_To_Display_Handle, dis_msg, 10);
              continue;
            }
            
            dis_msg[0] = DIS_CLEAN_RED_MESSAGE;
            xQueueSend(Queue_Analyze_To_Display_Handle, dis_msg, 10);
            
                                                              //0x55 头
            send_msg[1] = GROUP_SEND_MESSAGE;                 //0xC5 标志位
            send_msg[2] = print_num;                          //架号
            send_msg[3] = group_menu[choose_action].action;   //动作编号
            send_msg[4] = group_para.qua[group_choose];       //动作数量
            send_msg[5] = last_dir;                           //方向
            send_msg[6] = group_last_num;
                                                              //[7]~[8]为空
            m_crc = CRC16(send_msg, 10);
                                                              //0xAA 尾
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
  //显示连接中
  dis_msg[0] = DIS_CONNECTING;
  xQueueSend(Queue_Analyze_To_Display_Handle, dis_msg, 10);
  
//  //zigbee发送
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
//  //最多等待5s
//  for(i = 0; i < 100; i++)
//  {
//    if(xQueueReceive(Queue_Zigbee_Receive_Handle, recv_msg, 50) == pdTRUE)   //最多等待5s
//    {
//      //连接判断
//      if(1) //测试用
//      {
//            //测试用
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
//    //可加入判断急停代码
//    
//  }
//  if(ifrecv == 0)
//  {
//    dis_msg[0] = DIS_CONNECTION_FAILED;
//    xQueueSend(Queue_Analyze_To_Display_Handle, dis_msg, 10);
//    return;
//  }
  
  //默认右增
  group_para.dir = 2;
  
  //程序数量改为默认值，移架为3，其他为5
  group_para.qua[0] = 3;
  group_para.qua[1] = 5;
  group_para.qua[2] = 5;
  group_para.qua[3] = 5;
  group_para.qua[4] = 5;
  group_para.qua[5] = 5;
  group_choose = 0;
  last_group_dir = 0;
  
  m_dir = 0xEE;     //默认未选择方向
  
  current_state = STATE_GROUP;
  choose_action = 0; //默认选中第一个
  dis_msg[0] = DIS_JUMP_GROUP;
  dis_msg[1] = group_para.dir;
  dis_msg[2] = group_para.qua[group_choose];
  xQueueSend(Queue_Analyze_To_Display_Handle, dis_msg, 10);
}

