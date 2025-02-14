#ifndef __BSP_DISPLAY_H__
#define __BSP_DISPLAY_H__

#include "main.h"

/*----------------------------------------------------------------------------------*/
#define START_UP_DELAY 2000       //启动延时

#define START_PIC_DELAY 5000      //开机图片显示时长

/*颜色编号--默认*/        
#define DEFAULT_COLOR_BACK               34//背景
#define DEFAULT_COLOR_TEXT               15    //默认颜色（文字、数字）
#define DEFAULT_COLOR_CHOOSE_BACK        46    //选中文字的背景色
#define DEFAULT_COLOR_NUM_BACK           0     //选中架号数字的背景色
#define DEFAULT_COLOR_CONNECTION         1     //连接文字颜色
/*主界面选中界面显示架号的坐标*/
#define MAIN_ELENUM_STR_X   18    //文字
#define MAIN_ELENUM_X       120
#define MAIN_ELENUM_Y       42
/*显示连接文字的坐标*/
#define MAIN_CONNECTION_X   10    //文字
#define MAIN_CONNECTION_Y   0
/*大标题的坐标*/
#define FIRST_LINE_X     180
#define FIRST_LINE_Y     42
/*选中菜单支架参数坐标（包括采煤机位置）*/
#define CHOOSE_PARA_STR_X             18    //全体文字的X轴
#define CHOOSE_PARA_MINUS_X           90    //负号的X轴
#define CHOOSE_PARA_NUM_X             120   //全体数字的X轴
#define CHOOSE_PARA_LOCATION_Y        68    //采煤机位置
#define CHOOSE_PARA_DISTANCE_Y        94    //行程传感器
#define CHOOSE_PARA_PRESSURE_Y        120   //压力传感器
#define CHOOSE_PARA_INCLINATION_Y1    146   //倾角传感器1
#define CHOOSE_PARA_INCLINATION_Y2    172   //倾角传感器2
#define CHOOSE_PARA_INCLINATION_Y3    198   //倾角传感器3

/*选中菜单列表中各选项的坐标*/
#define CHOOSE_MENU_ITEM_X    180  
#define CHOOSE_MENU_ITEM_Y1   68
#define CHOOSE_MENU_ITEM_Y2   94
#define CHOOSE_MENU_ITEM_Y3   120
#define CHOOSE_MENU_ITEM_Y4   146
#define CHOOSE_MENU_ITEM_Y5   172
#define CHOOSE_MENU_ITEM_Y6   198
/*成组菜单列表中各选项的坐标*/
#define GROUP_MENU_ITEM_X    180  
#define GROUP_MENU_ITEM_Y1   68
#define GROUP_MENU_ITEM_Y2   94
#define GROUP_MENU_ITEM_Y3   120
#define GROUP_MENU_ITEM_Y4   146
#define GROUP_MENU_ITEM_Y5   172
#define GROUP_MENU_ITEM_Y6   198

/*成组菜单方向显示坐标*/
#define GROUP_ORIENTATION_SX        21    //文字X
#define GROUP_ORIENTATION_SY        68    //文字Y
#define GROUP_ORIENTATION_Y         94    //数字及标志Y
#define GROUP_ORIENTATION_NUM1      20    //数字1
#define GROUP_ORIENTATION_NUM2      120   //数字2
#define GROUP_ORIENTATION_STR       55    //方向标志

/*低电量显示的坐标*/                
#define POWER_DIS_X             180
#define POWER_DIS_Y             0


/*dis_msg*/
#define DIS_CLEAN               0x00    //清屏
#define DIS_JUMP_MAIN           0x01    //跳转至主界面
#define DIS_DIR_LR              0x03    //触发方向键(左/右)
#define DIS_DIR_UP              0x04    //触发方向键(上)
#define DIS_DIR_DOWN            0x05    //触发方向键(下)
#define DIS_JUMP_GROUP          0x06    //跳转至成组界面
#define DIS_CLEAN_RED_MESSAGE   0xEA    //去除红色提示信息
#define DIS_CONNECTING          0xE0    //连接中
#define DIS_CONNECTION_FAILED   0xE1    //连接失败
#define DIS_IN_ACTION           0xE2    //动作中
#define DIS_MAIN_NUM            0x10    //主界面显示待选架号
#define DIS_GROUP_TO_LEFT       0x21    //成组界面左成组变化
#define DIS_GROUP_TO_RIGHT      0x22    //成组界面右成组变化
#define DIS_NO_DIR              0xEE    //成组状态按下确认时未选择方向

#define DIS_JUMP_CHOOSE         0xCC    //跳转至选中界面
#define DIS_PARA_REFRESH_2      0xCD    //参数刷新
//#define DIS_PARA_REFRESH_3      0xCA    //参数刷新
//#define DIS_PARA_REFRESH_4      0xCB    //参数刷新
//#define DIS_ONGOING             0xA0    //显示正在进行的动作

#define LOW_POWER               0xBB    //电量不足
#define HIGH_POWER              0xBC    //电量不足
/*-------------------------------------------------------------------------------------*/

#define   MOVE_NONE                                0          //无
#define   MOVE_COLUMN_UP                           0x01       //升立柱
#define   MOVE_COLUMN_DOWN                         0x02       //降立柱
#define   MOVE_HYDRAULICFRAME_ADVANCING            0x03       //移架
#define   MOVE_CHUTE_PUSH                          0x04       //推溜
#define   MOVE_BOTTOM_UP                           0x05       //抬底
#define   MOVE_FUEL_SPRAY                          0x06       //喷雾
#define   MOVE_UPLIFT_BOTTOM_COLUMN_DOWN           0x07       //抬底降柱
#define   MOVE_UPLIFT_BOTTOM_HYDRAULIC_ADVANCING   0x08       //抬底移架
#define   MOVE_BALANCE_BEAM_STRETCH                0x09       //伸平衡梁
#define   MOVE_BALANCE_BEAM_WITHDRAW               0x0a       //收平衡梁
#define   MOVE_SIDE_PLATE_STRETCH                  0x0b       //伸侧推
#define   MOVE_SIDE_PLATE_WITHDRAW                 0x0c       //收侧推
#define   MOVE_TELESCOPIC_GIRDER_STRETCH           0x0d       //伸伸缩梁
#define   MOVE_TELESCOPIC_GIRDER_WITHDRAW          0x0e       //收伸缩梁
#define   MOVE_PLATE_STRETCH                       0x0f       //伸护帮板
#define   MOVE_PLATE_WITHDRAW                      0x10       //收护帮板
#define   MOVE_HYDRAULIC_BOTTOM_STRETCH            0x11       //伸底调
#define   MOVE_HYDRAULIC_BOTTOM_WITHDRAW           0x12       //收底调
#define   MOVE_DROP_AD_UP                          0x13       //降移升

/*动作名称及指令（24个）*/
#define ACTION_0    "收伸缩  "
#define ACTION_0_ACT   MOVE_TELESCOPIC_GIRDER_WITHDRAW
#define ACTION_1    "升柱    "
#define ACTION_1_ACT   MOVE_COLUMN_UP
#define ACTION_2    "推溜    "
#define ACTION_2_ACT   MOVE_CHUTE_PUSH
#define ACTION_3    "伸平衡  "
#define ACTION_3_ACT   MOVE_BALANCE_BEAM_STRETCH
#define ACTION_4    "伸护帮  "
#define ACTION_4_ACT   MOVE_PLATE_STRETCH
#define ACTION_5    "伸伸缩  "
#define ACTION_5_ACT   MOVE_TELESCOPIC_GIRDER_STRETCH
#define ACTION_6    "降柱    "
#define ACTION_6_ACT   MOVE_COLUMN_DOWN
#define ACTION_7    "拉溜    "
#define ACTION_7_ACT   MOVE_HYDRAULICFRAME_ADVANCING
#define ACTION_8    "收平衡  "
#define ACTION_8_ACT   MOVE_BALANCE_BEAM_WITHDRAW
#define ACTION_9    "收护帮  "
#define ACTION_9_ACT   MOVE_PLATE_WITHDRAW
#define ACTION_10   "抬底    "
#define ACTION_10_ACT   MOVE_BOTTOM_UP
#define ACTION_11   "喷雾    "
#define ACTION_11_ACT   MOVE_FUEL_SPRAY
#define ACTION_12   "伸侧推  "
#define ACTION_12_ACT   MOVE_SIDE_PLATE_STRETCH
#define ACTION_13   "收侧推  "
#define ACTION_13_ACT   MOVE_SIDE_PLATE_WITHDRAW
#define ACTION_14   "伸底调  "
#define ACTION_14_ACT   MOVE_HYDRAULIC_BOTTOM_STRETCH
#define ACTION_15   "收底调  "
#define ACTION_15_ACT   MOVE_HYDRAULIC_BOTTOM_WITHDRAW
#define ACTION_16   "抬底降柱"
#define ACTION_16_ACT   MOVE_UPLIFT_BOTTOM_COLUMN_DOWN
#define ACTION_17   "抬底移架"
#define ACTION_17_ACT   MOVE_UPLIFT_BOTTOM_HYDRAULIC_ADVANCING
/*未使用*/
#define ACTION_18   "动作18"
#define ACTION_19   "动作19"
#define ACTION_20   "动作20"
#define ACTION_21   "动作21"
#define ACTION_22   "动作22"
#define ACTION_23   "动作23"

/*成组名称及指令（6个）*/

#define   MOVE_GROUP_ADVANCING                           0x20          //移架
#define   MOVE_GROUP_CHUTE_PUSH                          0x21          //推溜
#define   MOVE_GROUPTELESCOPICPLATE_GIRDER_WITHDRAW      0x24         //收伸缩梁护帮板
#define   MOVE_GROUP_CHUTE_PULL                          0x22         //拉溜
#define   MOVE_GROUP_TELESCOPIC_GIRDER_STRETCH           0x23          //伸伸缩两
#define   MOVE_GROUP_PLATE_STRETCH                       0x25          //伸护帮板

#define G_ACTION_0    "自动移架"
#define G_ACTION_0_ACT   MOVE_GROUP_ADVANCING
#define G_ACTION_1    "推溜    "
#define G_ACTION_1_ACT   MOVE_GROUP_CHUTE_PUSH
#define G_ACTION_2    "收伸缩梁"
#define G_ACTION_2_ACT   MOVE_GROUPTELESCOPICPLATE_GIRDER_WITHDRAW
#define G_ACTION_3    "拉溜    "
#define G_ACTION_3_ACT   MOVE_GROUP_CHUTE_PULL
#define G_ACTION_4    "伸伸缩梁"
#define G_ACTION_4_ACT   MOVE_GROUP_TELESCOPIC_GIRDER_STRETCH
#define G_ACTION_5    "伸护帮板"
#define G_ACTION_5_ACT   MOVE_GROUP_PLATE_STRETCH

/*未使用*/
#define G_ACTION_6    "动作06  "
#define G_ACTION_6_ACT   0x57
#define G_ACTION_7    "动作07  "
#define G_ACTION_7_ACT   0x58

////#define G_ACTION_0    "成组自动移架"
////#define G_ACTION_0_ACT   0x51
////#define G_ACTION_1    "成组拉溜    "
////#define G_ACTION_1_ACT   0x52
////#define G_ACTION_2    "成组收伸缩梁"
////#define G_ACTION_2_ACT   0x53
////#define G_ACTION_3    "成组伸伸缩梁"
////#define G_ACTION_3_ACT   0x54
////#define G_ACTION_4    "成组收侧护帮"
////#define G_ACTION_4_ACT   0x55
////#define G_ACTION_5    "成组伸侧护帮"
////#define G_ACTION_5_ACT   0x56
/////*未使用*/
////#define G_ACTION_6    "成组动作06  "
////#define G_ACTION_6_ACT   0x57
////#define G_ACTION_7    "成组动作07  "
////#define G_ACTION_7_ACT   0x58

/*----------------------------------------------------------------------------------*/

#define GROUP_TO_STR    "动作支架"
#define GROUP_TO_NONE   "----"
#define GROUP_TO_LEFT   "<-<-"
#define GROUP_TO_RIGHT  "->->"

/*----------------------------------------------------------------------------------*/

extern QueueHandle_t Queue_Analyze_To_Display_Handle;    //解析任务至显示任务信息（用于控制屏幕显示）

/* 任务函数 */
void Display_Function(void);		// 显示函数
/*选中界面初始化*/
void Selection_Menu_Init(void); 
/*成组界面初始化*/
void Group_Menu_Init(void); 
/*等待屏幕结束反馈*/
void CheckBusy(void);           

/*清屏（颜色）*/
void Display_Clean(uint8_t color, uint8_t ifmain);
/*设置文字底色（颜色）*/
void Display_Set_Str_Back(uint8_t color);

/*屏幕显示数字（x，y，数字，颜色，底色）----color：15为白色，0为黑色*/
void Display_Num(uint16_t x, uint16_t y, uint8_t num, uint8_t color, uint8_t back);

/*
屏幕显示字符串（x，y，字符串，颜色，大小，底色）
color：15为白色，0为黑色
large：16，24，32，48
back：底色，为0x88时表示为无底色
*/
void Display_Str(uint16_t x, uint16_t y, char *str, uint16_t color, uint16_t large, uint16_t back);
/*显示初始界面----简易说明、煤机位置*/
void Display_Main_Menu(void);
/*清除红色提示信息*/
void Display_Clean_Red_Message(void);
/*屏幕显示连接中*/
void Display_Connecting(void);
/*屏幕显示连接失败*/
void Display_Connection_Failed(void);
/*屏幕显示动作中*/
void Display_In_Action(void);
/*屏幕显示未选择方向*/
void Display_No_Dir(void);
/*屏幕显示选中界面（选中的架号，默认动作编号）*/
void Display_Choose_Menu(uint8_t num, uint8_t action);
/*选中界面架号改变（架号）*/
void Display_Choose_Menu_Num_Change(uint8_t num);
/*显示选中界面支架参数*/
void Display_Choose_Para_Item(uint8_t para[8]);
/*显示选中界面列表中的单项（高度，编号）*/
void Display_Choose_Menu_Item(uint8_t height, uint8_t num);
/*显示列表变化*/
void Display_Choose_Menu_Change_List(uint8_t direction, uint8_t action);
/*屏幕显示成组界面（选中的架号，默认动作编号）*/
void Display_Group_Menu(uint8_t num, uint8_t action);
/*显示成组界面列表中的单项（高度，编号）*/
void Display_Group_Menu_Item(uint8_t height, uint8_t num);
/*显示成组界面动作范围及方向*/
void Display_Group_Serial_Num(uint8_t key);
/*显示低电量提示信息*/
void Display_Low_Power(uint8_t power);



/*----------------------------------------------------------------------------------*/
struct selection    //选中列表结构体
{
  uint8_t action;       //动作编号
  uint8_t picnum;       //贴图序号
  char *action_string;  //显示名称
};

/*----------------------------------------------------------------------------------*/

extern char *pstr[256];

extern struct selection selection_menu[25];
extern struct selection group_menu[8];   

extern uint8_t last_dir;   //上次的方向----0x00：无；0x11：小，小，0x12：小，大；0x21：大，小，0x22：大，大

extern uint8_t last_group_dir; //记录最后一次按下的是左/右成组
extern uint8_t group_other_num;       //成组方向最后一台的架号
extern uint8_t group_last_num;        //成组最后动作的架号

#endif
