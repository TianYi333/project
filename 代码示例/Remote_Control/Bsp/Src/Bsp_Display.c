#include "Bsp_Display.h"

struct selection selection_menu[25];    //选中菜单各项--实际用到18个
struct selection group_menu[8];         //成组菜单各项--实际用到6个--也使用选中菜单的结构体

QueueHandle_t Queue_Analyze_To_Display_Handle;    //解析任务至显示任务信息（用于控制屏幕显示）

uint8_t A2D_msg[8];
uint8_t Dis_in[8];
uint8_t Cursor;         //光标位置----1-6
uint8_t last_dir;       //上次的方向----0x00：无；0x11：小，小，0x12：小，大；0x21：大，小，0x22：大，大
uint8_t last_group_dir; //记录最后一次按下的是左/右成组
uint8_t group_other_num;       //成组方向最后一台的架号
uint8_t group_last_num;        //成组最后动作的架号

//数字转换字符串（前补0）
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

// 显示函数
void Display_Function(void)
{
  char m_dir[50] = {0};
  char *LOGO = "123456";
  
//  int i = 1;
    
  //开机启动延时
  vTaskDelay(START_UP_DELAY);
  
////现在在每次用到底色的位置重新选择
//  Display_Set_Str_Back(DEFAULT_COLOR_CHOOSE_BACK);
  
  //屏幕亮度
  sprintf(m_dir, "BL(20);\r\n");
  
  HAL_UART_Transmit(&huart1, (uint8_t *)m_dir, strlen(m_dir), 100); 
  
  vTaskDelay(100);
  
  //横屏
  sprintf(m_dir, "DIR(3);\r\n");
  
  HAL_UART_Transmit(&huart1, (uint8_t *)m_dir, strlen(m_dir), 100); 
  
  vTaskDelay(100);
  
  //开机图片
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
      case DIS_CLEAN:       //清屏
        Display_Clean(DEFAULT_COLOR_BACK, 1);  //默认颜色
        break;
      
      case DIS_JUMP_MAIN:   //跳转至主界面
        Display_Main_Menu();
        break;
      
      case DIS_JUMP_CHOOSE: //跳转至选中界面--默认选中第13个动作
        Display_Choose_Menu(A2D_msg[1], choose_action);
        //显示支架参数
        Display_Choose_Para_Item(A2D_msg);
        break;
      
      case LOW_POWER:   //低电量
        Display_Low_Power(0);
        break;
      case HIGH_POWER:  //高电量
        Display_Low_Power(1);
        break;
      
      case DIS_CONNECTING:          //连接中
        Display_Connecting();
        break;
      
      case DIS_CONNECTION_FAILED:   //连接失败
        Display_Connection_Failed();
        break;
      
      case DIS_IN_ACTION:           //动作中
        Display_In_Action();
        break;
      
      case DIS_NO_DIR:            //未设置方向
        Display_No_Dir();
        break;
      
      case DIS_CLEAN_RED_MESSAGE: //去除红色提示
        Display_Clean_Red_Message();
        break;
      
      case DIS_JUMP_GROUP:  //跳转至成组界面--默认选中第1个动作
        Display_Group_Menu(A2D_msg[1], choose_action);
        break;

      case DIS_DIR_LR:         //触发方向键
        Display_Choose_Menu_Num_Change(A2D_msg[1]);
        break;
      
      case DIS_DIR_UP:         //触发方向键
        Display_Choose_Menu_Change_List(0, A2D_msg[1]);
        break;
      
      case DIS_DIR_DOWN:       //触发方向键
        Display_Choose_Menu_Change_List(1, A2D_msg[1]);
        break;
      
      case DIS_GROUP_TO_LEFT:  //左成组键
        Display_Group_Serial_Num(1);
        last_group_dir = 1;
        break;
            
      case DIS_GROUP_TO_RIGHT: //右成组键
        Display_Group_Serial_Num(2);
        last_group_dir = 2;
        break;
      
      
//      case DIS_STOP:        //急停
//        Display_Str(FIRST_LINE_X, FIRST_LINE_y, "急停", DEFAULT_COLOR_TEXT, 32, 0x88);
//        break;
      
      case DIS_MAIN_NUM:    //主界面显示待选架号
        //Display_Num(uint16_t x, uint16_t y, uint8_t num, uint8_t color)
        Display_Str(MAIN_ELENUM_STR_X, MAIN_ELENUM_Y, "架号：", DEFAULT_COLOR_TEXT, 24, DEFAULT_COLOR_BACK);
        Display_Num(MAIN_ELENUM_X, MAIN_ELENUM_Y, A2D_msg[1], DEFAULT_COLOR_TEXT, DEFAULT_COLOR_NUM_BACK);
        break;
      
      case DIS_PARA_REFRESH_2:
//      case DIS_PARA_REFRESH_3: 
//      case DIS_PARA_REFRESH_4:
        Display_Choose_Para_Item(A2D_msg);
        break;
//      case DIS_ONGOING:     //显示正在进行的动作
//        
//        break;
      
      
    }
  }
  
}

/*
选中界面初始化
设置动作编号与对应的贴图地址
picnum由屏幕厂家软件设置
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
  
  /*实际未使用*/
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

/*成组界面初始化*/
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

//屏幕显示数字（x，y，数字，颜色，底色）----color：15为白色，0为黑色
void Display_Num(uint16_t x, uint16_t y, uint8_t num, uint8_t color, uint8_t back)
{
  char cmd[50] = {0};
  if(back == 0x88)
  {
    //无底色
    sprintf(cmd,"DC24(%3d,%3d,\'%s\',%d);\r\n",(int)x,(int)y,(char*)pstr[num],color);
  }
  else
  {
    //有底色
    sprintf(cmd,"SBC(%d);DCV24(%3d,%3d,\'%s\',%d);\r\n",(int)back,(int)x,(int)y,(char*)pstr[num],color);
  }
  HAL_UART_Transmit(&huart1, (uint8_t *)cmd, strlen(cmd), 100); 
  CheckBusy();
}

//清屏（颜色）----------暂时只用到了默认颜色
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

//设置文字底色（颜色）----------暂时只用到了默认颜色
void Display_Set_Str_Back(uint8_t color)
{
  char cmd[50] = {0};
  sprintf(cmd,"SBC(%d);\r\n",(int)color);
  HAL_UART_Transmit(&huart1, (uint8_t *)cmd, strlen(cmd), 100); 
  CheckBusy();
}

/*
屏幕显示字符串（x，y，字符串，颜色，大小，底色）
color：15为白色，0为黑色
large：16，24，32，48
back：0：无，1：有
*/
void Display_Str(uint16_t x, uint16_t y, char *str, uint16_t color, uint16_t large, uint16_t back)
{
  //char *cmd = pvPortMalloc(200);
  char cmd[200];
  switch(large)//高度
  {
    case 16:
      if(back == 0x88)//是否有底色
        sprintf(cmd, "DC16(%d, %d, \'%s\', %d);\r\n", (uint16_t)x, (uint16_t)y, (char *)str, (uint16_t)color);
      else
        sprintf(cmd, "SBC(%d);DCV16(%3d, %3d, \'%s\', %3d);\r\n", (uint16_t)back, (uint16_t)x, (uint16_t)y, (char *)str, (uint16_t)color);
      break;
    case 24:
      if(back == 0x88)//是否有底色
        sprintf(cmd, "DC24(%3d, %3d, \'%s\', %3d);\r\n", (uint16_t)x, (uint16_t)y, (char *)str, (uint16_t)color);
      else
        sprintf(cmd, "SBC(%d);DCV24(%3d, %3d, \'%s\', %3d);\r\n", (uint16_t)back, (uint16_t)x, (uint16_t)y, (char *)str, (uint16_t)color);
      break;
    case 32:
      if(back == 0x88)//是否有底色
        sprintf(cmd, "DC32(%3d, %3d, \'%s\', %3d);\r\n", (uint16_t)x, (uint16_t)y, (char *)str, (uint16_t)color);
      else
        sprintf(cmd, "SBC(%d);DCV32(%3d, %3d, \'%s\', %3d);\r\n", (uint16_t)back, (uint16_t)x, (uint16_t)y, (char *)str, (uint16_t)color);
      break;
    case 48:
      if(back == 0x88)//是否有底色
        sprintf(cmd, "DC48(%3d, %3d, \'%s\', %3d, %3d);\r\n", (uint16_t)x, (uint16_t)y, (char *)str, (uint16_t)color, 0);
      else
        sprintf(cmd, "SBC(%d);DC48(%3d, %3d, \'%s\', %3d, %3d);\r\n", (uint16_t)back, (uint16_t)x, (uint16_t)y, (char *)str, (uint16_t)color, 1);
      break;

  }

  HAL_UART_Transmit(&huart1, (uint8_t *)cmd, strlen(cmd), 100); 
  CheckBusy();
  //vPortFree(cmd);
}
//显示初始界面----简易说明、煤机位置
void Display_Main_Menu(void)
{
  //清屏
  Display_Clean(DEFAULT_COLOR_BACK, 0);
  //显示数字
  Display_Str(MAIN_ELENUM_STR_X, MAIN_ELENUM_Y, "架号：", DEFAULT_COLOR_TEXT, 24, DEFAULT_COLOR_BACK);
  Display_Num(MAIN_ELENUM_X, MAIN_ELENUM_Y, 0, DEFAULT_COLOR_TEXT, DEFAULT_COLOR_NUM_BACK);
  
  Display_Str(CHOOSE_PARA_STR_X, CHOOSE_PARA_LOCATION_Y, "位置：", DEFAULT_COLOR_TEXT, 24, DEFAULT_COLOR_BACK);
  Display_Num(CHOOSE_PARA_NUM_X, CHOOSE_PARA_LOCATION_Y, 0, DEFAULT_COLOR_TEXT, DEFAULT_COLOR_NUM_BACK);
}
//清除红色提示信息
void Display_Clean_Red_Message(void)
{
  Display_Str(MAIN_CONNECTION_X, MAIN_CONNECTION_Y, "              ", DEFAULT_COLOR_CONNECTION, 24, DEFAULT_COLOR_BACK);
}
//屏幕显示连接中
void Display_Connecting(void)
{
  Dis_in[0] = 5;
  Display_Str(MAIN_CONNECTION_X, MAIN_CONNECTION_Y, "连接中…", DEFAULT_COLOR_CONNECTION, 24, DEFAULT_COLOR_BACK);
  xQueueSend(Queue_Clean_Red_Handle, Dis_in, 10);
}
//屏幕显示连接失败
void Display_Connection_Failed(void)
{
  Dis_in[0] = 3;
  Display_Str(MAIN_CONNECTION_X, MAIN_CONNECTION_Y, "连接失败", DEFAULT_COLOR_CONNECTION, 24, DEFAULT_COLOR_BACK);
  xQueueSend(Queue_Clean_Red_Handle, Dis_in, 10);
}
//屏幕显示连接中
void Display_In_Action(void)
{
  Dis_in[0] = 1;
  Display_Str(MAIN_CONNECTION_X, MAIN_CONNECTION_Y, "动作中…", DEFAULT_COLOR_CONNECTION, 24, DEFAULT_COLOR_BACK);
  xQueueSend(Queue_Clean_Red_Handle, Dis_in, 10);
}
//屏幕显示未选择方向
void Display_No_Dir(void)
{
  Dis_in[0] = 3;
  Display_Str(MAIN_CONNECTION_X, MAIN_CONNECTION_Y, "未选择方向", DEFAULT_COLOR_CONNECTION, 24, DEFAULT_COLOR_BACK);
  xQueueSend(Queue_Clean_Red_Handle, Dis_in, 10);
}
//屏幕显示选中界面（选中的架号，默认动作编号）
void Display_Choose_Menu(uint8_t num, uint8_t action)
{
  Cursor = 0;
  //清屏
  Display_Clean(DEFAULT_COLOR_BACK, 1);
  //显示数字（位置与主界面相同）
  Display_Str(MAIN_ELENUM_STR_X, MAIN_ELENUM_Y, "架号：", DEFAULT_COLOR_TEXT, 24, DEFAULT_COLOR_BACK);
  Display_Num(MAIN_ELENUM_X, MAIN_ELENUM_Y, print_num, DEFAULT_COLOR_TEXT, DEFAULT_COLOR_NUM_BACK);
  //切换标题----32大小，无底色
  Display_Str(FIRST_LINE_X, FIRST_LINE_Y, "选中菜单", DEFAULT_COLOR_TEXT, 24, 0x88);
  //显示列表
  Display_Choose_Menu_Item(0, action);               //默认第一个动作
  Display_Choose_Menu_Item(1, (action + 1) % 18);    //默认第二个动作
  Display_Choose_Menu_Item(2, (action + 2) % 18);    //默认第三个动作
  Display_Choose_Menu_Item(3, (action + 3) % 18);    //默认第四个动作
  Display_Choose_Menu_Item(4, (action + 4) % 18);    //默认第五个动作
  Display_Choose_Menu_Item(5, (action + 5) % 18);    //默认第六个动作
}
//选中界面架号改变（架号）
void Display_Choose_Menu_Num_Change(uint8_t num)
{
  //显示数字（位置与主界面相同）
  Display_Num(MAIN_ELENUM_X, MAIN_ELENUM_Y, num, DEFAULT_COLOR_TEXT, DEFAULT_COLOR_NUM_BACK);
  //若有行程/压力等显示，需要暂时写为XX
}
//
void Display_Choose_Para_Item(uint8_t para[8])
{
//  //倾角转换
//  int16_t inc[3] = {0};
//  uint8_t ifminus[3] = {0}, i;
  switch(para[0])
  {
    case DIS_JUMP_CHOOSE:
      
      Display_Str(CHOOSE_PARA_STR_X, CHOOSE_PARA_LOCATION_Y, "位置：", DEFAULT_COLOR_TEXT, 24, DEFAULT_COLOR_BACK);
      Display_Str(CHOOSE_PARA_STR_X, CHOOSE_PARA_DISTANCE_Y, "行程：", DEFAULT_COLOR_TEXT, 24, DEFAULT_COLOR_BACK);
      Display_Str(CHOOSE_PARA_STR_X, CHOOSE_PARA_PRESSURE_Y, "压力：", DEFAULT_COLOR_TEXT, 24, DEFAULT_COLOR_BACK);
      Display_Str(CHOOSE_PARA_STR_X, CHOOSE_PARA_INCLINATION_Y1, "倾角1：", DEFAULT_COLOR_TEXT, 24, DEFAULT_COLOR_BACK);
      Display_Str(CHOOSE_PARA_STR_X, CHOOSE_PARA_INCLINATION_Y2, "倾角2：", DEFAULT_COLOR_TEXT, 24, DEFAULT_COLOR_BACK);
      Display_Str(CHOOSE_PARA_STR_X, CHOOSE_PARA_INCLINATION_Y3, "倾角3：", DEFAULT_COLOR_TEXT, 24, DEFAULT_COLOR_BACK);
    
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
    //非选中项使用背景色作为底色，可减少刷屏次数
    Display_Str(m_x, m_y, selection_menu[num].action_string, DEFAULT_COLOR_TEXT, 24, DEFAULT_COLOR_BACK);
  }
}

//屏幕显示成组界面（选中的架号，默认动作编号）
void Display_Group_Menu(uint8_t num, uint8_t action)
{
  Cursor = 0;
  //清屏
  Display_Clean(DEFAULT_COLOR_BACK, 1);
  //显示数字（位置与主界面相同）
  Display_Str(MAIN_ELENUM_STR_X, MAIN_ELENUM_Y, "架号：", DEFAULT_COLOR_TEXT, 24, DEFAULT_COLOR_BACK);
  Display_Num(MAIN_ELENUM_X, MAIN_ELENUM_Y, print_num, DEFAULT_COLOR_TEXT, DEFAULT_COLOR_NUM_BACK);
  //切换标题----32大小，无底色
  Display_Str(FIRST_LINE_X, FIRST_LINE_Y, "成组菜单", DEFAULT_COLOR_TEXT, 24, 0x88);
  //显示列表
  Display_Group_Menu_Item(0, action);               //默认第一个动作
  Display_Group_Menu_Item(1, (action + 1) % 18);    //默认第二个动作
  Display_Group_Menu_Item(2, (action + 2) % 18);    //默认第三个动作
  Display_Group_Menu_Item(3, (action + 3) % 18);    //默认第四个动作
  Display_Group_Menu_Item(4, (action + 4) % 18);    //默认第五个动作
  Display_Group_Menu_Item(5, (action + 5) % 18);    //默认第六个动作
  //显示成组架号及方向
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
    //根据数量更新左侧架号显示
    
  }
  else
  {
    //非选中项使用背景色作为底色，可减少刷屏次数
    Display_Str(m_x, m_y, group_menu[num].action_string, DEFAULT_COLOR_TEXT, 24, DEFAULT_COLOR_BACK);
  }
}

/*显示列表变化*/
void Display_Choose_Menu_Change_List(uint8_t direction, uint8_t action)
{
  uint8_t i;
  //上翻时光标不在最上or下翻时光标不在最下时，菜单整体不动，选中条目发生变化
  if(direction == 0 && Cursor != 0)       //向上翻，且之前光标不在最上方
  {
      Cursor--;
      group_choose = (group_choose + 5) % 6;
  }
  else if(direction == 1 && Cursor != 5)  //向下翻，且之前光标不在最下方
  {
    Cursor++;
    group_choose = (group_choose + 7) % 6;
  }
  //根据界面状态执行不同
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

/*显示成组界面动作范围及方向*/
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
  if(group_para.dir == 2)    //右增时
  {
    if(key == 1)   //左成组键
    {
//      Display_Num(GROUP_ORIENTATION_NUM1, GROUP_ORIENTATION_Y, print_num - (group_para.qua[group_choose] - 1), DEFAULT_COLOR_TEXT, DEFAULT_COLOR_NUM_BACK);
      group_other_num = print_num - (group_para.qua[group_choose] - 1) < NUM_MIN ? NUM_MIN : print_num - (group_para.qua[group_choose] - 1);
      Display_Num(GROUP_ORIENTATION_NUM1, GROUP_ORIENTATION_Y, group_other_num, DEFAULT_COLOR_TEXT, DEFAULT_COLOR_NUM_BACK);
      Display_Num(GROUP_ORIENTATION_NUM2, GROUP_ORIENTATION_Y, print_num, DEFAULT_COLOR_TEXT, DEFAULT_COLOR_NUM_BACK);
     
      if(last_dir == 0x00 || ((last_dir & 0xF0) == 0x20)) //上次非左成组
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
      if(last_dir == 0x00 || ((last_dir & 0xF0) == 0x10)) //上次非右成组
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
  else if(group_para.dir == 1)    //左增时
  {
    if(key == 1)   //左成组键
    {
      group_other_num = print_num + (group_para.qua[group_choose] - 1) > NUM_MAX ? NUM_MAX : print_num + (group_para.qua[group_choose] - 1);
      Display_Num(GROUP_ORIENTATION_NUM1, GROUP_ORIENTATION_Y, group_other_num, DEFAULT_COLOR_TEXT, DEFAULT_COLOR_NUM_BACK);
      Display_Num(GROUP_ORIENTATION_NUM2, GROUP_ORIENTATION_Y, print_num, DEFAULT_COLOR_TEXT, DEFAULT_COLOR_NUM_BACK);
      if(last_dir == 0x00 || ((last_dir & 0xF0) == 0x20)) //上次非左成组
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
      if(last_dir == 0x00 || ((last_dir & 0xF0) == 0x10)) //上次非右成组
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

//显示低电量提示信息
//power：0：低电量，1：高电量
void Display_Low_Power(uint8_t power)
{
  //记录上一次是低电量还是高电量
  static uint8_t last_dis = 1;
  //低电量显示计数，用于循环闪烁
  static uint8_t low_time = 0;
  if(power == 0)
  {
    last_dis = 0;
    if(low_time < 3)
    {
      //显示低电量
      Display_Str(POWER_DIS_X, POWER_DIS_Y, "电量不足", DEFAULT_COLOR_CONNECTION, 24, DEFAULT_COLOR_BACK);
    }
    else
    {
      //不显示
      Display_Str(POWER_DIS_X, POWER_DIS_Y, "        ", DEFAULT_COLOR_CONNECTION, 24, DEFAULT_COLOR_BACK);
    }
    low_time = (low_time + 1) % 6;
  }
  else
  {
    if(last_dis == 0)
    {
      //不显示
      Display_Str(POWER_DIS_X, POWER_DIS_Y, "        ", DEFAULT_COLOR_CONNECTION, 24, DEFAULT_COLOR_BACK);
    }
    low_time = 0;
  }
}









