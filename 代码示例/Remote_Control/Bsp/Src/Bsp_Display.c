#include "Bsp_Display.h"

struct selection selection_menu[25];    //ѡ�в˵�����--ʵ���õ�18��
struct selection group_menu[8];         //����˵�����--ʵ���õ�6��--Ҳʹ��ѡ�в˵��Ľṹ��

QueueHandle_t Queue_Analyze_To_Display_Handle;    //������������ʾ������Ϣ�����ڿ�����Ļ��ʾ��

uint8_t A2D_msg[8];
uint8_t Dis_in[8];
uint8_t Cursor;         //���λ��----1-6
uint8_t last_dir;       //�ϴεķ���----0x00���ޣ�0x11��С��С��0x12��С����0x21����С��0x22���󣬴�
uint8_t last_group_dir; //��¼���һ�ΰ��µ�����/�ҳ���
uint8_t group_other_num;       //���鷽�����һ̨�ļܺ�
uint8_t group_last_num;        //����������ļܺ�

//����ת���ַ�����ǰ��0��
char *pstr[256]={"000","001","002","003","004","005","006","007","008","009","010","011","012","013","014","015",
                 "016","017","018","019","020","021","022","023","024","025","026","027","028","029","030","031",
                 "032","033","034","035","036","037","038","039","040","041","042","043","044","045","046","047",
                 "048","049","050","051","052","053","054","055","056","057","058","059","060","061","062","063",
                 "064","065","066","067","068","069","070","071","072","073","074","075","076","077","078","079",
                 "080","081","082","083","084","085","086","087","088","089","090","091","092","093","094","095",
                 "096","097","098","099","100","101","102","103","104","105","106","107","108","109","110","111",
                 "112","113","114","115","116","117","118","119","120","121","122","123","124","125","126","127",
                 "128","129","130","131","132","133","134","135","136","137","138","139","140","141","142","143",
                 "144","145","146","147","148","149","150","151","152","153","154","155","156","157","158","159",
                 "160","161","162","163","164","165","166","167","168","169","170","171","172","173","174","175",
                 "176","177","178","179","180","181","182","183","184","185","186","187","188","189","190","191",
                 "192","193","194","195","196","197","198","199","200","201","202","203","204","205","206","207",
                 "208","209","210","211","212","213","214","215","216","217","218","219","220","221","222","223",
                 "224","225","226","227","228","229","230","231","232","233","234","235","236","237","238","239",
                 "240","241","242","243","244","245","246","247","248","249","250","251","252","253","254","255"};

// ��ʾ����
void Display_Function(void)
{
  char m_dir[50] = {0};
  char *LOGO = "123456";
  
//  int i = 1;
    
  //����������ʱ
  vTaskDelay(START_UP_DELAY);
  
////������ÿ���õ���ɫ��λ������ѡ��
//  Display_Set_Str_Back(DEFAULT_COLOR_CHOOSE_BACK);
  
  //��Ļ����
  sprintf(m_dir, "BL(20);\r\n");
  
  HAL_UART_Transmit(&huart1, (uint8_t *)m_dir, strlen(m_dir), 100); 
  
  vTaskDelay(100);
  
  //����
  sprintf(m_dir, "DIR(3);\r\n");
  
  HAL_UART_Transmit(&huart1, (uint8_t *)m_dir, strlen(m_dir), 100); 
  
  vTaskDelay(100);
  
  //����ͼƬ
  sprintf(m_dir, "FSIMG(2250752,0,0,320,240,0;\r\n");
  
  HAL_UART_Transmit(&huart1, (uint8_t *)m_dir, strlen(m_dir), 100); 
  
  vTaskDelay(START_PIC_DELAY);
  
  Selection_Menu_Init();
  
  Group_Menu_Init();
  

  
//  Display_Clean(DEFAULT_COLOR_BACK);
//  vTaskDelay(1000);
  
  Display_Main_Menu();

  while(1)
  {
    xQueueReceive(Queue_Analyze_To_Display_Handle, A2D_msg, portMAX_DELAY);

    switch(A2D_msg[0])
    {
      case DIS_CLEAN:       //����
        Display_Clean(DEFAULT_COLOR_BACK, 1);  //Ĭ����ɫ
        break;
      
      case DIS_JUMP_MAIN:   //��ת��������
        Display_Main_Menu();
        break;
      
      case DIS_JUMP_CHOOSE: //��ת��ѡ�н���--Ĭ��ѡ�е�13������
        Display_Choose_Menu(A2D_msg[1], choose_action);
        //��ʾ֧�ܲ���
        Display_Choose_Para_Item(A2D_msg);
        break;
      
      case LOW_POWER:   //�͵���
        Display_Low_Power(0);
        break;
      case HIGH_POWER:  //�ߵ���
        Display_Low_Power(1);
        break;
      
      case DIS_CONNECTING:          //������
        Display_Connecting();
        break;
      
      case DIS_CONNECTION_FAILED:   //����ʧ��
        Display_Connection_Failed();
        break;
      
      case DIS_IN_ACTION:           //������
        Display_In_Action();
        break;
      
      case DIS_NO_DIR:            //δ���÷���
        Display_No_Dir();
        break;
      
      case DIS_CLEAN_RED_MESSAGE: //ȥ����ɫ��ʾ
        Display_Clean_Red_Message();
        break;
      
      case DIS_JUMP_GROUP:  //��ת���������--Ĭ��ѡ�е�1������
        Display_Group_Menu(A2D_msg[1], choose_action);
        break;

      case DIS_DIR_LR:         //���������
        Display_Choose_Menu_Num_Change(A2D_msg[1]);
        break;
      
      case DIS_DIR_UP:         //���������
        Display_Choose_Menu_Change_List(0, A2D_msg[1]);
        break;
      
      case DIS_DIR_DOWN:       //���������
        Display_Choose_Menu_Change_List(1, A2D_msg[1]);
        break;
      
      case DIS_GROUP_TO_LEFT:  //������
        Display_Group_Serial_Num(1);
        last_group_dir = 1;
        break;
            
      case DIS_GROUP_TO_RIGHT: //�ҳ����
        Display_Group_Serial_Num(2);
        last_group_dir = 2;
        break;
      
      
//      case DIS_STOP:        //��ͣ
//        Display_Str(FIRST_LINE_X, FIRST_LINE_y, "��ͣ", DEFAULT_COLOR_TEXT, 32, 0x88);
//        break;
      
      case DIS_MAIN_NUM:    //��������ʾ��ѡ�ܺ�
        //Display_Num(uint16_t x, uint16_t y, uint8_t num, uint8_t color)
        Display_Str(MAIN_ELENUM_STR_X, MAIN_ELENUM_Y, "�ܺţ�", DEFAULT_COLOR_TEXT, 24, DEFAULT_COLOR_BACK);
        Display_Num(MAIN_ELENUM_X, MAIN_ELENUM_Y, A2D_msg[1], DEFAULT_COLOR_TEXT, DEFAULT_COLOR_NUM_BACK);
        break;
      
      case DIS_PARA_REFRESH_2:
//      case DIS_PARA_REFRESH_3: 
//      case DIS_PARA_REFRESH_4:
        Display_Choose_Para_Item(A2D_msg);
        break;
//      case DIS_ONGOING:     //��ʾ���ڽ��еĶ���
//        
//        break;
      
      
    }
  }
  
}

/*
ѡ�н����ʼ��
���ö���������Ӧ����ͼ��ַ
picnum����Ļ�����������
*/
void Selection_Menu_Init(void)
{
  selection_menu[0].action  = ACTION_0_ACT;
  selection_menu[0].picnum  = 0x00;
  selection_menu[0].action_string = pvPortMalloc(sizeof(ACTION_0));
  memcpy(selection_menu[0].action_string, ACTION_0, sizeof(ACTION_0));
  
  selection_menu[1].action  = ACTION_1_ACT;
  selection_menu[1].picnum  = 0x00;
  selection_menu[1].action_string = pvPortMalloc(sizeof(ACTION_1));
  memcpy(selection_menu[1].action_string, ACTION_1, sizeof(ACTION_1));
  
  selection_menu[2].action  = ACTION_2_ACT;
  selection_menu[2].picnum  = 0x00;
  selection_menu[2].action_string = pvPortMalloc(sizeof(ACTION_2));
  memcpy(selection_menu[2].action_string, ACTION_2, sizeof(ACTION_2));
  
  selection_menu[3].action  = ACTION_3_ACT;
  selection_menu[3].picnum  = 0x00;
  selection_menu[3].action_string = pvPortMalloc(sizeof(ACTION_3));
  memcpy(selection_menu[3].action_string, ACTION_3, sizeof(ACTION_3));
  
  selection_menu[4].action  = ACTION_4_ACT;
  selection_menu[4].picnum  = 0x00;
  selection_menu[4].action_string = pvPortMalloc(sizeof(ACTION_4));
  memcpy(selection_menu[4].action_string, ACTION_4, sizeof(ACTION_4));
  
  selection_menu[5].action  = ACTION_5_ACT;
  selection_menu[5].picnum  = 0x00;
  selection_menu[5].action_string = pvPortMalloc(sizeof(ACTION_5));
  memcpy(selection_menu[5].action_string, ACTION_5, sizeof(ACTION_5));
  
  selection_menu[6].action  = ACTION_6_ACT;
  selection_menu[6].picnum  = 0x00;
  selection_menu[6].action_string = pvPortMalloc(sizeof(ACTION_6));
  memcpy(selection_menu[6].action_string, ACTION_6, sizeof(ACTION_6));
  
  selection_menu[7].action  = ACTION_7_ACT;
  selection_menu[7].picnum  = 0x00;
  selection_menu[7].action_string = pvPortMalloc(sizeof(ACTION_7));
  memcpy(selection_menu[7].action_string, ACTION_7, sizeof(ACTION_7));
  
  selection_menu[8].action  = ACTION_8_ACT;
  selection_menu[8].picnum  = 0x00;
  selection_menu[8].action_string = pvPortMalloc(sizeof(ACTION_8));
  memcpy(selection_menu[8].action_string, ACTION_8, sizeof(ACTION_8));
  
  selection_menu[9].action  = ACTION_9_ACT;
  selection_menu[9].picnum  = 0x00;
  selection_menu[9].action_string = pvPortMalloc(sizeof(ACTION_9));
  memcpy(selection_menu[9].action_string, ACTION_9, sizeof(ACTION_9));
  
  selection_menu[10].action = ACTION_10_ACT;
  selection_menu[10].picnum = 0x00;
  selection_menu[10].action_string = pvPortMalloc(sizeof(ACTION_10));
  memcpy(selection_menu[10].action_string, ACTION_10, sizeof(ACTION_10));
  
  selection_menu[11].action = ACTION_11_ACT;
  selection_menu[11].picnum = 0x00;
  selection_menu[11].action_string = pvPortMalloc(sizeof(ACTION_11));
  memcpy(selection_menu[11].action_string, ACTION_11, sizeof(ACTION_11));
  
  selection_menu[12].action = ACTION_12_ACT;
  selection_menu[12].picnum = 0x00;
  selection_menu[12].action_string = pvPortMalloc(sizeof(ACTION_12));
  memcpy(selection_menu[12].action_string, ACTION_12, sizeof(ACTION_12));
  
  selection_menu[13].action = ACTION_13_ACT;
  selection_menu[13].picnum = 0x00;
  selection_menu[13].action_string = pvPortMalloc(sizeof(ACTION_13));
  memcpy(selection_menu[13].action_string, ACTION_13, sizeof(ACTION_13));
  
  selection_menu[14].action = ACTION_14_ACT;
  selection_menu[14].picnum = 0x00;
  selection_menu[14].action_string = pvPortMalloc(sizeof(ACTION_14));
  memcpy(selection_menu[14].action_string, ACTION_14, sizeof(ACTION_14));
  
  selection_menu[15].action = ACTION_15_ACT;
  selection_menu[15].picnum = 0x00;
  selection_menu[15].action_string = pvPortMalloc(sizeof(ACTION_15));
  memcpy(selection_menu[15].action_string, ACTION_15, sizeof(ACTION_15));
  
  selection_menu[16].action = ACTION_16_ACT;
  selection_menu[16].picnum = 0x00;
  selection_menu[16].action_string = pvPortMalloc(sizeof(ACTION_16));
  memcpy(selection_menu[16].action_string, ACTION_16, sizeof(ACTION_16));
  
  selection_menu[17].action = ACTION_17_ACT;
  selection_menu[17].picnum = 0x00;
  selection_menu[17].action_string = pvPortMalloc(sizeof(ACTION_17));
  memcpy(selection_menu[17].action_string, ACTION_17, sizeof(ACTION_17));
  
  /*ʵ��δʹ��*/
  selection_menu[18].action = 0x47;
  selection_menu[18].picnum = 0x00;
  selection_menu[18].action_string = pvPortMalloc(sizeof(ACTION_18));
  memcpy(selection_menu[18].action_string, ACTION_18, sizeof(ACTION_18));
  
  selection_menu[19].action = 0x48;
  selection_menu[19].picnum = 0x00;
  selection_menu[19].action_string = pvPortMalloc(sizeof(ACTION_19));
  memcpy(selection_menu[19].action_string, ACTION_19, sizeof(ACTION_19));
  
  selection_menu[20].action = 0x49;
  selection_menu[20].picnum = 0x00;
  selection_menu[20].action_string = pvPortMalloc(sizeof(ACTION_20));
  memcpy(selection_menu[20].action_string, ACTION_20, sizeof(ACTION_20));
  
  selection_menu[21].action = 0x4A;
  selection_menu[21].picnum = 0x00;
  selection_menu[21].action_string = pvPortMalloc(sizeof(ACTION_21));
  memcpy(selection_menu[21].action_string, ACTION_21, sizeof(ACTION_21));
  
  selection_menu[22].action = 0x4B;
  selection_menu[22].picnum = 0x00;
  selection_menu[22].action_string = pvPortMalloc(sizeof(ACTION_22));
  memcpy(selection_menu[22].action_string, ACTION_22, sizeof(ACTION_22));
  
  selection_menu[23].action = 0x4C;
  selection_menu[23].picnum = 0x00;
  selection_menu[23].action_string = pvPortMalloc(sizeof(ACTION_23));
  memcpy(selection_menu[23].action_string, ACTION_23, sizeof(ACTION_23));
}

/*��������ʼ��*/
void Group_Menu_Init(void)
{
  group_menu[0].action = G_ACTION_0_ACT;
  group_menu[0].picnum = 0x00;
  group_menu[0].action_string = pvPortMalloc(sizeof(G_ACTION_0));
  memcpy(group_menu[0].action_string, G_ACTION_0, sizeof(G_ACTION_0));
  
  group_menu[1].action = G_ACTION_1_ACT;
  group_menu[1].picnum = 0x00;
  group_menu[1].action_string = pvPortMalloc(sizeof(G_ACTION_1));
  memcpy(group_menu[1].action_string, G_ACTION_1, sizeof(G_ACTION_1));
  
  group_menu[2].action = G_ACTION_2_ACT;
  group_menu[2].picnum = 0x00;
  group_menu[2].action_string = pvPortMalloc(sizeof(G_ACTION_2));
  memcpy(group_menu[2].action_string, G_ACTION_2, sizeof(G_ACTION_2));
  
  group_menu[3].action = G_ACTION_3_ACT;
  group_menu[3].picnum = 0x00;
  group_menu[3].action_string = pvPortMalloc(sizeof(G_ACTION_3));
  memcpy(group_menu[3].action_string, G_ACTION_3, sizeof(G_ACTION_3));
  
  group_menu[4].action = G_ACTION_4_ACT;
  group_menu[4].picnum = 0x00;
  group_menu[4].action_string = pvPortMalloc(sizeof(G_ACTION_4));
  memcpy(group_menu[4].action_string, G_ACTION_4, sizeof(G_ACTION_4));
  
  group_menu[5].action = G_ACTION_5_ACT;
  group_menu[5].picnum = 0x00;
  group_menu[5].action_string = pvPortMalloc(sizeof(G_ACTION_5));
  memcpy(group_menu[5].action_string, G_ACTION_5, sizeof(G_ACTION_5));
  
  group_menu[6].action = G_ACTION_6_ACT;
  group_menu[6].picnum = 0x00;
  group_menu[6].action_string = pvPortMalloc(sizeof(G_ACTION_6));
  memcpy(group_menu[6].action_string, G_ACTION_6, sizeof(G_ACTION_6));
  
  group_menu[7].action = G_ACTION_7_ACT;
  group_menu[7].picnum = 0x00;
  group_menu[7].action_string = pvPortMalloc(sizeof(G_ACTION_7));
  memcpy(group_menu[7].action_string, G_ACTION_7, sizeof(G_ACTION_7));
}

uint8_t bad_time;

void CheckBusy(void)  
{
  uint8_t loop = 0;
	while(1)
	{
    if(ok==0x0f)
     break;
    vTaskDelay(5);
    loop++;
    if(loop == 40)
    {
     bad_time++;
     break;
    }
	}		
	
	ok=0;
}

//��Ļ��ʾ���֣�x��y�����֣���ɫ����ɫ��----color��15Ϊ��ɫ��0Ϊ��ɫ
void Display_Num(uint16_t x, uint16_t y, uint8_t num, uint8_t color, uint8_t back)
{
  char cmd[50] = {0};
  if(back == 0x88)
  {
    //�޵�ɫ
    sprintf(cmd,"DC24(%3d,%3d,\'%s\',%d);\r\n",(int)x,(int)y,(char*)pstr[num],color);
  }
  else
  {
    //�е�ɫ
    sprintf(cmd,"SBC(%d);DCV24(%3d,%3d,\'%s\',%d);\r\n",(int)back,(int)x,(int)y,(char*)pstr[num],color);
  }
  HAL_UART_Transmit(&huart1, (uint8_t *)cmd, strlen(cmd), 100); 
  CheckBusy();
}

//��������ɫ��----------��ʱֻ�õ���Ĭ����ɫ
void Display_Clean(uint8_t color, uint8_t ifmain)
{
  char cmd[200] = {0};
////  sprintf(cmd,"CLR(%d);\r\n",(int)color);
  if(ifmain == 0)
  {
    sprintf(cmd,"FSIMG(2404352,0,0,320,240,0);\r\n");
  }
  else
  {
    sprintf(cmd,"FSIMG(2097152,0,0,320,240,0);\r\n");
  }
  HAL_UART_Transmit(&huart1, (uint8_t *)cmd, strlen(cmd), 100); 
  CheckBusy();
}

//�������ֵ�ɫ����ɫ��----------��ʱֻ�õ���Ĭ����ɫ
void Display_Set_Str_Back(uint8_t color)
{
  char cmd[50] = {0};
  sprintf(cmd,"SBC(%d);\r\n",(int)color);
  HAL_UART_Transmit(&huart1, (uint8_t *)cmd, strlen(cmd), 100); 
  CheckBusy();
}

/*
��Ļ��ʾ�ַ�����x��y���ַ�������ɫ����С����ɫ��
color��15Ϊ��ɫ��0Ϊ��ɫ
large��16��24��32��48
back��0���ޣ�1����
*/
void Display_Str(uint16_t x, uint16_t y, char *str, uint16_t color, uint16_t large, uint16_t back)
{
  //char *cmd = pvPortMalloc(200);
  char cmd[200];
  switch(large)//�߶�
  {
    case 16:
      if(back == 0x88)//�Ƿ��е�ɫ
        sprintf(cmd, "DC16(%d, %d, \'%s\', %d);\r\n", (uint16_t)x, (uint16_t)y, (char *)str, (uint16_t)color);
      else
        sprintf(cmd, "SBC(%d);DCV16(%3d, %3d, \'%s\', %3d);\r\n", (uint16_t)back, (uint16_t)x, (uint16_t)y, (char *)str, (uint16_t)color);
      break;
    case 24:
      if(back == 0x88)//�Ƿ��е�ɫ
        sprintf(cmd, "DC24(%3d, %3d, \'%s\', %3d);\r\n", (uint16_t)x, (uint16_t)y, (char *)str, (uint16_t)color);
      else
        sprintf(cmd, "SBC(%d);DCV24(%3d, %3d, \'%s\', %3d);\r\n", (uint16_t)back, (uint16_t)x, (uint16_t)y, (char *)str, (uint16_t)color);
      break;
    case 32:
      if(back == 0x88)//�Ƿ��е�ɫ
        sprintf(cmd, "DC32(%3d, %3d, \'%s\', %3d);\r\n", (uint16_t)x, (uint16_t)y, (char *)str, (uint16_t)color);
      else
        sprintf(cmd, "SBC(%d);DCV32(%3d, %3d, \'%s\', %3d);\r\n", (uint16_t)back, (uint16_t)x, (uint16_t)y, (char *)str, (uint16_t)color);
      break;
    case 48:
      if(back == 0x88)//�Ƿ��е�ɫ
        sprintf(cmd, "DC48(%3d, %3d, \'%s\', %3d, %3d);\r\n", (uint16_t)x, (uint16_t)y, (char *)str, (uint16_t)color, 0);
      else
        sprintf(cmd, "SBC(%d);DC48(%3d, %3d, \'%s\', %3d, %3d);\r\n", (uint16_t)back, (uint16_t)x, (uint16_t)y, (char *)str, (uint16_t)color, 1);
      break;

  }

  HAL_UART_Transmit(&huart1, (uint8_t *)cmd, strlen(cmd), 100); 
  CheckBusy();
  //vPortFree(cmd);
}
//��ʾ��ʼ����----����˵����ú��λ��
void Display_Main_Menu(void)
{
  //����
  Display_Clean(DEFAULT_COLOR_BACK, 0);
  //��ʾ����
  Display_Str(MAIN_ELENUM_STR_X, MAIN_ELENUM_Y, "�ܺţ�", DEFAULT_COLOR_TEXT, 24, DEFAULT_COLOR_BACK);
  Display_Num(MAIN_ELENUM_X, MAIN_ELENUM_Y, 0, DEFAULT_COLOR_TEXT, DEFAULT_COLOR_NUM_BACK);
  
  Display_Str(CHOOSE_PARA_STR_X, CHOOSE_PARA_LOCATION_Y, "λ�ã�", DEFAULT_COLOR_TEXT, 24, DEFAULT_COLOR_BACK);
  Display_Num(CHOOSE_PARA_NUM_X, CHOOSE_PARA_LOCATION_Y, 0, DEFAULT_COLOR_TEXT, DEFAULT_COLOR_NUM_BACK);
}
//�����ɫ��ʾ��Ϣ
void Display_Clean_Red_Message(void)
{
  Display_Str(MAIN_CONNECTION_X, MAIN_CONNECTION_Y, "              ", DEFAULT_COLOR_CONNECTION, 24, DEFAULT_COLOR_BACK);
}
//��Ļ��ʾ������
void Display_Connecting(void)
{
  Dis_in[0] = 5;
  Display_Str(MAIN_CONNECTION_X, MAIN_CONNECTION_Y, "�����С�", DEFAULT_COLOR_CONNECTION, 24, DEFAULT_COLOR_BACK);
  xQueueSend(Queue_Clean_Red_Handle, Dis_in, 10);
}
//��Ļ��ʾ����ʧ��
void Display_Connection_Failed(void)
{
  Dis_in[0] = 3;
  Display_Str(MAIN_CONNECTION_X, MAIN_CONNECTION_Y, "����ʧ��", DEFAULT_COLOR_CONNECTION, 24, DEFAULT_COLOR_BACK);
  xQueueSend(Queue_Clean_Red_Handle, Dis_in, 10);
}
//��Ļ��ʾ������
void Display_In_Action(void)
{
  Dis_in[0] = 1;
  Display_Str(MAIN_CONNECTION_X, MAIN_CONNECTION_Y, "�����С�", DEFAULT_COLOR_CONNECTION, 24, DEFAULT_COLOR_BACK);
  xQueueSend(Queue_Clean_Red_Handle, Dis_in, 10);
}
//��Ļ��ʾδѡ����
void Display_No_Dir(void)
{
  Dis_in[0] = 3;
  Display_Str(MAIN_CONNECTION_X, MAIN_CONNECTION_Y, "δѡ����", DEFAULT_COLOR_CONNECTION, 24, DEFAULT_COLOR_BACK);
  xQueueSend(Queue_Clean_Red_Handle, Dis_in, 10);
}
//��Ļ��ʾѡ�н��棨ѡ�еļܺţ�Ĭ�϶�����ţ�
void Display_Choose_Menu(uint8_t num, uint8_t action)
{
  Cursor = 0;
  //����
  Display_Clean(DEFAULT_COLOR_BACK, 1);
  //��ʾ���֣�λ������������ͬ��
  Display_Str(MAIN_ELENUM_STR_X, MAIN_ELENUM_Y, "�ܺţ�", DEFAULT_COLOR_TEXT, 24, DEFAULT_COLOR_BACK);
  Display_Num(MAIN_ELENUM_X, MAIN_ELENUM_Y, print_num, DEFAULT_COLOR_TEXT, DEFAULT_COLOR_NUM_BACK);
  //�л�����----32��С���޵�ɫ
  Display_Str(FIRST_LINE_X, FIRST_LINE_Y, "ѡ�в˵�", DEFAULT_COLOR_TEXT, 24, 0x88);
  //��ʾ�б�
  Display_Choose_Menu_Item(0, action);               //Ĭ�ϵ�һ������
  Display_Choose_Menu_Item(1, (action + 1) % 18);    //Ĭ�ϵڶ�������
  Display_Choose_Menu_Item(2, (action + 2) % 18);    //Ĭ�ϵ���������
  Display_Choose_Menu_Item(3, (action + 3) % 18);    //Ĭ�ϵ��ĸ�����
  Display_Choose_Menu_Item(4, (action + 4) % 18);    //Ĭ�ϵ��������
  Display_Choose_Menu_Item(5, (action + 5) % 18);    //Ĭ�ϵ���������
}
//ѡ�н���ܺŸı䣨�ܺţ�
void Display_Choose_Menu_Num_Change(uint8_t num)
{
  //��ʾ���֣�λ������������ͬ��
  Display_Num(MAIN_ELENUM_X, MAIN_ELENUM_Y, num, DEFAULT_COLOR_TEXT, DEFAULT_COLOR_NUM_BACK);
  //�����г�/ѹ������ʾ����Ҫ��ʱдΪXX
}
//
void Display_Choose_Para_Item(uint8_t para[8])
{
//  //���ת��
//  int16_t inc[3] = {0};
//  uint8_t ifminus[3] = {0}, i;
  switch(para[0])
  {
    case DIS_JUMP_CHOOSE:
      
      Display_Str(CHOOSE_PARA_STR_X, CHOOSE_PARA_LOCATION_Y, "λ�ã�", DEFAULT_COLOR_TEXT, 24, DEFAULT_COLOR_BACK);
      Display_Str(CHOOSE_PARA_STR_X, CHOOSE_PARA_DISTANCE_Y, "�г̣�", DEFAULT_COLOR_TEXT, 24, DEFAULT_COLOR_BACK);
      Display_Str(CHOOSE_PARA_STR_X, CHOOSE_PARA_PRESSURE_Y, "ѹ����", DEFAULT_COLOR_TEXT, 24, DEFAULT_COLOR_BACK);
      Display_Str(CHOOSE_PARA_STR_X, CHOOSE_PARA_INCLINATION_Y1, "���1��", DEFAULT_COLOR_TEXT, 24, DEFAULT_COLOR_BACK);
      Display_Str(CHOOSE_PARA_STR_X, CHOOSE_PARA_INCLINATION_Y2, "���2��", DEFAULT_COLOR_TEXT, 24, DEFAULT_COLOR_BACK);
      Display_Str(CHOOSE_PARA_STR_X, CHOOSE_PARA_INCLINATION_Y3, "���3��", DEFAULT_COLOR_TEXT, 24, DEFAULT_COLOR_BACK);
    
//    case DIS_PARA_REFRESH_3:
    
      Display_Num(CHOOSE_PARA_NUM_X, CHOOSE_PARA_LOCATION_Y, para[2], DEFAULT_COLOR_TEXT, DEFAULT_COLOR_NUM_BACK);
    
      Display_Num(CHOOSE_PARA_NUM_X, CHOOSE_PARA_DISTANCE_Y, para[3] << 8 | para[4], DEFAULT_COLOR_TEXT, DEFAULT_COLOR_NUM_BACK);
      Display_Num(CHOOSE_PARA_NUM_X, CHOOSE_PARA_PRESSURE_Y, (para[5] << 8 | para[6]) / 10, DEFAULT_COLOR_TEXT, DEFAULT_COLOR_NUM_BACK);
    
      break;
    
    case DIS_PARA_REFRESH_2:
//    case DIS_PARA_REFRESH_4:
      
//      memcpy(&inc[0], &para[2], 2);
//      memcpy(&inc[1], &para[4], 2);
//      memcpy(&inc[2], &para[6], 2);
//    
//      for(i = 0; i < 3; i++)
//      {
//        if(inc[i] < 0)
//        {
//          inc[i] = - inc[i];
//          ifminus[i] = 1;
//        }
//        else
//        {
//          ifminus[i] = 0;
//        }
//      }
      
      if(para[2] == '-')
      {
        Display_Str(CHOOSE_PARA_MINUS_X, CHOOSE_PARA_INCLINATION_Y1, "-", DEFAULT_COLOR_TEXT, 24, DEFAULT_COLOR_BACK);
        Display_Num(CHOOSE_PARA_NUM_X, CHOOSE_PARA_INCLINATION_Y1, para[3], DEFAULT_COLOR_TEXT, DEFAULT_COLOR_NUM_BACK);
      }
      else
      {
        Display_Str(CHOOSE_PARA_MINUS_X, CHOOSE_PARA_INCLINATION_Y1, " ", DEFAULT_COLOR_TEXT, 24, DEFAULT_COLOR_BACK);
        Display_Num(CHOOSE_PARA_NUM_X, CHOOSE_PARA_INCLINATION_Y1, para[3], DEFAULT_COLOR_TEXT, DEFAULT_COLOR_NUM_BACK);
      }
      
      if(para[4] == '-')
      {
        Display_Str(CHOOSE_PARA_MINUS_X, CHOOSE_PARA_INCLINATION_Y2, "-", DEFAULT_COLOR_TEXT, 24, DEFAULT_COLOR_BACK);
        Display_Num(CHOOSE_PARA_NUM_X, CHOOSE_PARA_INCLINATION_Y2, para[5], DEFAULT_COLOR_TEXT, DEFAULT_COLOR_NUM_BACK);
      }
      else
      {
        Display_Str(CHOOSE_PARA_MINUS_X, CHOOSE_PARA_INCLINATION_Y2, " ", DEFAULT_COLOR_TEXT, 24, DEFAULT_COLOR_BACK);
        Display_Num(CHOOSE_PARA_NUM_X, CHOOSE_PARA_INCLINATION_Y2, para[5], DEFAULT_COLOR_TEXT, DEFAULT_COLOR_NUM_BACK);
      }
      
      if(para[6] == '-')
      {
        Display_Str(CHOOSE_PARA_MINUS_X, CHOOSE_PARA_INCLINATION_Y3, "-", DEFAULT_COLOR_TEXT, 24, DEFAULT_COLOR_BACK);
        Display_Num(CHOOSE_PARA_NUM_X, CHOOSE_PARA_INCLINATION_Y3, para[7], DEFAULT_COLOR_TEXT, DEFAULT_COLOR_NUM_BACK);
      }
      else
      {
        Display_Str(CHOOSE_PARA_MINUS_X, CHOOSE_PARA_INCLINATION_Y3, " ", DEFAULT_COLOR_TEXT, 24, DEFAULT_COLOR_BACK);
        Display_Num(CHOOSE_PARA_NUM_X, CHOOSE_PARA_INCLINATION_Y3, para[7], DEFAULT_COLOR_TEXT, DEFAULT_COLOR_NUM_BACK);
      }
      
////      Display_Num(CHOOSE_PARA_NUM_X, CHOOSE_PARA_INCLINATION_Y1, para[2] << 8 | para[3], DEFAULT_COLOR_TEXT, DEFAULT_COLOR_NUM_BACK);
////      Display_Num(CHOOSE_PARA_NUM_X, CHOOSE_PARA_INCLINATION_Y2, para[4] << 8 | para[5], DEFAULT_COLOR_TEXT, DEFAULT_COLOR_NUM_BACK);
////      Display_Num(CHOOSE_PARA_NUM_X, CHOOSE_PARA_INCLINATION_Y3, para[6] << 8 | para[7], DEFAULT_COLOR_TEXT, DEFAULT_COLOR_NUM_BACK);
      
      break;
  }
}

void Display_Choose_Menu_Item(uint8_t height, uint8_t num)
{
  uint8_t m_x = CHOOSE_MENU_ITEM_X,m_y;
  uint8_t ifchoose = 0;
  switch(height)
  {
    case 0:
      m_y = CHOOSE_MENU_ITEM_Y1;
      if(Cursor == 0)
      {
        ifchoose = 1;
      }
      break;
    case 1:
      if(Cursor == 1)
      {
        ifchoose = 1;
      }
      m_y = CHOOSE_MENU_ITEM_Y2;
      break;
    case 2:
      if(Cursor == 2)
      {
        ifchoose = 1;
      }
      m_y = CHOOSE_MENU_ITEM_Y3;
      break;
    case 3:
      if(Cursor == 3)
      {
        ifchoose = 1;
      }
      m_y = CHOOSE_MENU_ITEM_Y4;
      break;
    case 4:
      if(Cursor == 4)
      {
        ifchoose = 1;
      }
      m_y = CHOOSE_MENU_ITEM_Y5;
      break;
    case 5:
      if(Cursor == 5)
      {
        ifchoose = 1;
      }
      m_y = CHOOSE_MENU_ITEM_Y6;
      break;
  }
  if(ifchoose)
  {
    Display_Str(m_x, m_y, selection_menu[num].action_string, DEFAULT_COLOR_TEXT, 24, DEFAULT_COLOR_CHOOSE_BACK);
  }
  else
  {
    //��ѡ����ʹ�ñ���ɫ��Ϊ��ɫ���ɼ���ˢ������
    Display_Str(m_x, m_y, selection_menu[num].action_string, DEFAULT_COLOR_TEXT, 24, DEFAULT_COLOR_BACK);
  }
}

//��Ļ��ʾ������棨ѡ�еļܺţ�Ĭ�϶�����ţ�
void Display_Group_Menu(uint8_t num, uint8_t action)
{
  Cursor = 0;
  //����
  Display_Clean(DEFAULT_COLOR_BACK, 1);
  //��ʾ���֣�λ������������ͬ��
  Display_Str(MAIN_ELENUM_STR_X, MAIN_ELENUM_Y, "�ܺţ�", DEFAULT_COLOR_TEXT, 24, DEFAULT_COLOR_BACK);
  Display_Num(MAIN_ELENUM_X, MAIN_ELENUM_Y, print_num, DEFAULT_COLOR_TEXT, DEFAULT_COLOR_NUM_BACK);
  //�л�����----32��С���޵�ɫ
  Display_Str(FIRST_LINE_X, FIRST_LINE_Y, "����˵�", DEFAULT_COLOR_TEXT, 24, 0x88);
  //��ʾ�б�
  Display_Group_Menu_Item(0, action);               //Ĭ�ϵ�һ������
  Display_Group_Menu_Item(1, (action + 1) % 18);    //Ĭ�ϵڶ�������
  Display_Group_Menu_Item(2, (action + 2) % 18);    //Ĭ�ϵ���������
  Display_Group_Menu_Item(3, (action + 3) % 18);    //Ĭ�ϵ��ĸ�����
  Display_Group_Menu_Item(4, (action + 4) % 18);    //Ĭ�ϵ��������
  Display_Group_Menu_Item(5, (action + 5) % 18);    //Ĭ�ϵ���������
  //��ʾ����ܺż�����
  Display_Str(GROUP_ORIENTATION_SX, GROUP_ORIENTATION_SY, GROUP_TO_STR, DEFAULT_COLOR_TEXT, 24, 0x88);
  Display_Num(GROUP_ORIENTATION_NUM1, GROUP_ORIENTATION_Y, 0, DEFAULT_COLOR_TEXT, DEFAULT_COLOR_NUM_BACK);
  Display_Num(GROUP_ORIENTATION_NUM2, GROUP_ORIENTATION_Y, 0, DEFAULT_COLOR_TEXT, DEFAULT_COLOR_NUM_BACK);
  Display_Str(GROUP_ORIENTATION_STR, GROUP_ORIENTATION_Y, GROUP_TO_NONE, DEFAULT_COLOR_TEXT, 24, 0x88);
}

void Display_Group_Menu_Item(uint8_t height, uint8_t num)
{
  uint8_t m_x = GROUP_MENU_ITEM_X,m_y;
  uint8_t ifchoose = 0;
  switch(height)
  {
    case 0:
      m_y = GROUP_MENU_ITEM_Y1;
      if(Cursor == 0)
      {
        ifchoose = 1;
      }
      break;
    case 1:
      if(Cursor == 1)
      {
        ifchoose = 1;
      }
      m_y = GROUP_MENU_ITEM_Y2;
      break;
    case 2:
      if(Cursor == 2)
      {
        ifchoose = 1;
      }
      m_y = GROUP_MENU_ITEM_Y3;
      break;
    case 3:
      if(Cursor == 3)
      {
        ifchoose = 1;
      }
      m_y = GROUP_MENU_ITEM_Y4;
      break;
    case 4:
      if(Cursor == 4)
      {
        ifchoose = 1;
      }
      m_y = GROUP_MENU_ITEM_Y5;
      break;
    case 5:
      if(Cursor == 5)
      {
        ifchoose = 1;
      }
      m_y = GROUP_MENU_ITEM_Y6;
      break;
  }
  if(ifchoose)
  {
    Display_Str(m_x, m_y, group_menu[num].action_string, DEFAULT_COLOR_TEXT, 24, DEFAULT_COLOR_CHOOSE_BACK);
    //���������������ܺ���ʾ
    
  }
  else
  {
    //��ѡ����ʹ�ñ���ɫ��Ϊ��ɫ���ɼ���ˢ������
    Display_Str(m_x, m_y, group_menu[num].action_string, DEFAULT_COLOR_TEXT, 24, DEFAULT_COLOR_BACK);
  }
}

/*��ʾ�б�仯*/
void Display_Choose_Menu_Change_List(uint8_t direction, uint8_t action)
{
  uint8_t i;
  //�Ϸ�ʱ��겻������or�·�ʱ��겻������ʱ���˵����岻����ѡ����Ŀ�����仯
  if(direction == 0 && Cursor != 0)       //���Ϸ�����֮ǰ��겻�����Ϸ�
  {
      Cursor--;
      group_choose = (group_choose + 5) % 6;
  }
  else if(direction == 1 && Cursor != 5)  //���·�����֮ǰ��겻�����·�
  {
    Cursor++;
    group_choose = (group_choose + 7) % 6;
  }
  //���ݽ���״ִ̬�в�ͬ
  switch(current_state)
  {
    case STATE_SELECTION:
      for(i = 0;i < 6; i++)
      {
        Display_Choose_Menu_Item((Cursor + i) % 6, (ACTION_MAX + action + (Cursor + i) % 6 - Cursor) % ACTION_MAX); 
      }
      break;
    case STATE_GROUP:
      for(i = 0;i < 6; i++)
      {
        Display_Group_Menu_Item((Cursor + i) % 6, (G_ACTION_MAX + action + (Cursor + i) % 6 - Cursor) % G_ACTION_MAX); 
      }
    break;
  }
}

/*��ʾ������涯����Χ������*/
void Display_Group_Serial_Num(uint8_t key)
{
  if(key == 0)
  {
    if(last_group_dir == 0)
    {
      return;
    }
    else if((last_group_dir == 1 && group_para.dir == 2) || (last_group_dir == 2 && group_para.dir == 1))
    {
      Display_Num(GROUP_ORIENTATION_NUM1, GROUP_ORIENTATION_Y, (print_num - (group_para.qua[group_choose] - 1) < NUM_MIN ? NUM_MIN : print_num - (group_para.qua[group_choose] - 1)), DEFAULT_COLOR_TEXT, DEFAULT_COLOR_NUM_BACK);
      Display_Num(GROUP_ORIENTATION_NUM2, GROUP_ORIENTATION_Y, print_num, DEFAULT_COLOR_TEXT, DEFAULT_COLOR_NUM_BACK);
    }
    else 
    {
      Display_Num(GROUP_ORIENTATION_NUM1, GROUP_ORIENTATION_Y, print_num, DEFAULT_COLOR_TEXT, DEFAULT_COLOR_NUM_BACK);
      Display_Num(GROUP_ORIENTATION_NUM2, GROUP_ORIENTATION_Y, (print_num + (group_para.qua[group_choose] - 1) > NUM_MAX ? NUM_MAX : print_num + (group_para.qua[group_choose] - 1)), DEFAULT_COLOR_TEXT, DEFAULT_COLOR_NUM_BACK);
    }
  }
  if(group_para.dir == 2)    //����ʱ
  {
    if(key == 1)   //������
    {
//      Display_Num(GROUP_ORIENTATION_NUM1, GROUP_ORIENTATION_Y, print_num - (group_para.qua[group_choose] - 1), DEFAULT_COLOR_TEXT, DEFAULT_COLOR_NUM_BACK);
      group_other_num = print_num - (group_para.qua[group_choose] - 1) < NUM_MIN ? NUM_MIN : print_num - (group_para.qua[group_choose] - 1);
      Display_Num(GROUP_ORIENTATION_NUM1, GROUP_ORIENTATION_Y, group_other_num, DEFAULT_COLOR_TEXT, DEFAULT_COLOR_NUM_BACK);
      Display_Num(GROUP_ORIENTATION_NUM2, GROUP_ORIENTATION_Y, print_num, DEFAULT_COLOR_TEXT, DEFAULT_COLOR_NUM_BACK);
     
      if(last_dir == 0x00 || ((last_dir & 0xF0) == 0x20)) //�ϴη������
      {
        last_dir = 0x11;
        group_last_num = group_other_num;
        Display_Str(GROUP_ORIENTATION_STR, GROUP_ORIENTATION_Y, GROUP_TO_LEFT, DEFAULT_COLOR_TEXT, 24, DEFAULT_COLOR_BACK);
      }
      else if(last_dir == 0x11)
      {
        last_dir = 0x12;
        group_last_num = print_num;
        Display_Str(GROUP_ORIENTATION_STR, GROUP_ORIENTATION_Y, GROUP_TO_RIGHT, DEFAULT_COLOR_TEXT, 24, DEFAULT_COLOR_BACK);
      }
      else if(last_dir == 0x12)
      {
        last_dir = 0x11;
        group_last_num = group_other_num;
        Display_Str(GROUP_ORIENTATION_STR, GROUP_ORIENTATION_Y, GROUP_TO_LEFT, DEFAULT_COLOR_TEXT, 24, DEFAULT_COLOR_BACK);
      }
    }
    else if(key == 2)
    {
      group_other_num = print_num + (group_para.qua[group_choose] - 1) > NUM_MAX ? NUM_MAX : print_num + (group_para.qua[group_choose] - 1);
      Display_Num(GROUP_ORIENTATION_NUM1, GROUP_ORIENTATION_Y, print_num, DEFAULT_COLOR_TEXT, DEFAULT_COLOR_NUM_BACK);
//      Display_Num(GROUP_ORIENTATION_NUM2, GROUP_ORIENTATION_Y, print_num + (group_para.qua[group_choose] - 1) , DEFAULT_COLOR_TEXT, DEFAULT_COLOR_NUM_BACK);
      Display_Num(GROUP_ORIENTATION_NUM2, GROUP_ORIENTATION_Y, group_other_num, DEFAULT_COLOR_TEXT, DEFAULT_COLOR_NUM_BACK);
      if(last_dir == 0x00 || ((last_dir & 0xF0) == 0x10)) //�ϴη��ҳ���
      {
        last_dir = 0x22;
        group_last_num = group_other_num;
        Display_Str(GROUP_ORIENTATION_STR, GROUP_ORIENTATION_Y, GROUP_TO_RIGHT, DEFAULT_COLOR_TEXT, 24, DEFAULT_COLOR_BACK);
      }
      else if(last_dir == 0x22)
      {
        last_dir = 0x21;
        group_last_num = print_num;
        Display_Str(GROUP_ORIENTATION_STR, GROUP_ORIENTATION_Y, GROUP_TO_LEFT, DEFAULT_COLOR_TEXT, 24, DEFAULT_COLOR_BACK);
      }
      else if(last_dir == 0x21)
      {
        last_dir = 0x22;
        group_last_num = group_other_num;
        Display_Str(GROUP_ORIENTATION_STR, GROUP_ORIENTATION_Y, GROUP_TO_RIGHT, DEFAULT_COLOR_TEXT, 24, DEFAULT_COLOR_BACK);
      }
    }
  }
  else if(group_para.dir == 1)    //����ʱ
  {
    if(key == 1)   //������
    {
      group_other_num = print_num + (group_para.qua[group_choose] - 1) > NUM_MAX ? NUM_MAX : print_num + (group_para.qua[group_choose] - 1);
      Display_Num(GROUP_ORIENTATION_NUM1, GROUP_ORIENTATION_Y, group_other_num, DEFAULT_COLOR_TEXT, DEFAULT_COLOR_NUM_BACK);
      Display_Num(GROUP_ORIENTATION_NUM2, GROUP_ORIENTATION_Y, print_num, DEFAULT_COLOR_TEXT, DEFAULT_COLOR_NUM_BACK);
      if(last_dir == 0x00 || ((last_dir & 0xF0) == 0x20)) //�ϴη������
      {
        last_dir = 0x11;
        group_last_num = print_num;
        Display_Str(GROUP_ORIENTATION_STR, GROUP_ORIENTATION_Y, GROUP_TO_LEFT, DEFAULT_COLOR_TEXT, 24, DEFAULT_COLOR_BACK);
      }
      else if(last_dir == 0x11)
      {
        last_dir = 0x12;
        group_last_num = group_other_num;
        Display_Str(GROUP_ORIENTATION_STR, GROUP_ORIENTATION_Y, GROUP_TO_RIGHT, DEFAULT_COLOR_TEXT, 24, DEFAULT_COLOR_BACK);
      }
      else if(last_dir == 0x12)
      {
        last_dir = 0x11;
        group_last_num = print_num;
        Display_Str(GROUP_ORIENTATION_STR, GROUP_ORIENTATION_Y, GROUP_TO_LEFT, DEFAULT_COLOR_TEXT, 24, DEFAULT_COLOR_BACK);
      }
    }
    else if(key == 2)
    {
      group_other_num = print_num - (group_para.qua[group_choose] - 1) < NUM_MIN ? NUM_MIN : print_num - (group_para.qua[group_choose] - 1);
      Display_Num(GROUP_ORIENTATION_NUM1, GROUP_ORIENTATION_Y, print_num, DEFAULT_COLOR_TEXT, DEFAULT_COLOR_NUM_BACK);
      Display_Num(GROUP_ORIENTATION_NUM2, GROUP_ORIENTATION_Y, group_other_num, DEFAULT_COLOR_TEXT, DEFAULT_COLOR_NUM_BACK);
      if(last_dir == 0x00 || ((last_dir & 0xF0) == 0x10)) //�ϴη��ҳ���
      {
        last_dir = 0x21;
        group_last_num = group_other_num;
        Display_Str(GROUP_ORIENTATION_STR, GROUP_ORIENTATION_Y, GROUP_TO_LEFT, DEFAULT_COLOR_TEXT, 24, DEFAULT_COLOR_BACK);
      }
      else if(last_dir == 0x21)
      {
        last_dir = 0x22;
        group_last_num = print_num;
        Display_Str(GROUP_ORIENTATION_STR, GROUP_ORIENTATION_Y, GROUP_TO_RIGHT, DEFAULT_COLOR_TEXT, 24, DEFAULT_COLOR_BACK);
      }
      else if(last_dir == 0x22)
      {
        last_dir = 0x21;
        group_last_num = group_other_num;
        Display_Str(GROUP_ORIENTATION_STR, GROUP_ORIENTATION_Y, GROUP_TO_LEFT, DEFAULT_COLOR_TEXT, 24, DEFAULT_COLOR_BACK);
      }
    }
  }
  
  m_dir = last_dir;
}

//��ʾ�͵�����ʾ��Ϣ
//power��0���͵�����1���ߵ���
void Display_Low_Power(uint8_t power)
{
  //��¼��һ���ǵ͵������Ǹߵ���
  static uint8_t last_dis = 1;
  //�͵�����ʾ����������ѭ����˸
  static uint8_t low_time = 0;
  if(power == 0)
  {
    last_dis = 0;
    if(low_time < 3)
    {
      //��ʾ�͵���
      Display_Str(POWER_DIS_X, POWER_DIS_Y, "��������", DEFAULT_COLOR_CONNECTION, 24, DEFAULT_COLOR_BACK);
    }
    else
    {
      //����ʾ
      Display_Str(POWER_DIS_X, POWER_DIS_Y, "        ", DEFAULT_COLOR_CONNECTION, 24, DEFAULT_COLOR_BACK);
    }
    low_time = (low_time + 1) % 6;
  }
  else
  {
    if(last_dis == 0)
    {
      //����ʾ
      Display_Str(POWER_DIS_X, POWER_DIS_Y, "        ", DEFAULT_COLOR_CONNECTION, 24, DEFAULT_COLOR_BACK);
    }
    low_time = 0;
  }
}









