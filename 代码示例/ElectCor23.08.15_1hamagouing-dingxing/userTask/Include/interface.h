#ifndef __INTERFACE_H__
#define __INTERFACE_H__
#include "stm32f4xx_hal.h"


extern char variant_interface_Dive[7];//驱动器通信显示


/***********************_variant_Interface_disp.type用，表明屏幕显示***********************************/
#define   TYPE_DEFAULT                      1
#define   TYPE_SINGLE_CHOSE                 2
#define   TYPE_SINGLE_MOVE                  3
#define   TYPE_SINGLE_MOVEED                4
#define   TYPE_SETTING_GLOBAL_SET            5
#define   TYPE_SETTING_SERVICE              6
#define   TYPE_SETTING_DEFAULT_PARAMETERS        7
#define   TYPE_SETTING_GROUP_ADVANCING         8
#define   TYPE_SETTING_GROUP_CHUTE_PUSH         9
#define   TYPE_SETTING_GROUP_BALANCE              10
#define   TYPE_SETTING_GROUP_TELESCOPIC           11
#define   TYPE_SETTING_GROUP_NC                    12
#define   TYPE_SETTING_AUTOMATION                  13
#define   TYPE_SETTING_FAULT                       14
#define   TYPE_WORKINGCONDITION_WORKINGCONDITION1              15
#define   TYPE_WORKINGCONDITION_WORKINGCONDITION2  16
#define   TYPE_URGENCY_STOP                        17
#define   TYPE_GROUP_LEFT                       18
#define   TYPE_GROUP_RIGHT                       19
#define   TYPE_GROUP_MOVE                       20
#define   TYPE_GROUP_ALARM                       21
#define   TYPE_GROUP_PAUSE                       22
#define   TYPE_FOLLOW_MOVE                           23
/***********************_variant_Interface_disp.upright用***********************************/

#define   UPRIGHT_SINGLE_CHOSE   1
#define   UPRIGHT_SINGLE_MOVE    0
/***********************_variant_Interface_disp.cursor用***********************************/
#define   CURSOR_TYPE_SINGLE_CHOSE_LIMT   4
#define   CURSOR_TYPE_SETTING_GLOBAL_SET_LIMT   14
#define   CURSOR_TYPE_SETTING_SERVICE_LIMT   8
#define   CURSOR_TYPE_SETTING_DEFAULT_PARAMETERS_LIMT   11
#define   CURSOR_TYPE_SETTING_GROUP_ADVANCING_LIMT   26
#define   CURSOR_TYPE_SETTING_GROUP_CHUTE_PUSH_LIMT   8
#define   CURSOR_TYPE_SETTING_GROUP_BALANCE_LIMT   11
#define   CURSOR_TYPE_SETTING_GROUP_TELESCOPIC_LIMT   10
#define   CURSOR_TYPE_SETTING_GROUP_NC_LIMT   3
#define   CURSOR_TYPE_SETTING_AUTOMATION_LIMT   5
#define   CURSOR_TYPE_SETTING_FAULT_LIMT   11

struct _disp_menu{	
	
	uint8_t		  cursor;					//表示条目行数  只有123四行
    uint8_t       type; 
};


typedef struct _disp_context{
	unsigned char page_y;						//坐标y
	unsigned char column_x;						//坐标x
    char *Use_Char;						//内容
}_disp_context;


extern char DeviceNum[4];
extern uint8_t variant_Service_DeviceNum;


#define page_First_Line   1
#define page_Second_Line  3
#define page_Third_Line   5
#define page_Fouth_Line   7

#define column_1_Line    0
#define column_2_Line    8
#define column_3_Line    16
#define column_4_Line    24
#define column_5_Line    32
#define column_6_Line    40
#define column_7_Line    48
#define column_8_Line    56
#define column_9_Line    64
#define column_10_Line   72
#define column_11_Line   80
#define column_12_Line   88
#define column_13_Line   96
#define column_14_Line   104
#define column_15_Line   112
#define column_16_Line   120





/******************
_sensor_distance.sensor_view[7]
_sensor_pressure.sensor_view[7]
_sensor_Connectingrod_angle.sensor_view[7]
_sensor_canopy_angle.sensor_view[7]
_sensor_Plate_angle.sensor_view[7]

_sensor_distance.sensor_value
_sensor_pressure.sensor_value
_sensor_Connectingrod_angle.sensor_value
_sensor_canopy_angle.sensor_value
_sensor_Plate_angle.sensor_value
************/
/**************复制区*************/


void Display_context_show(_disp_context s[],uint8_t length,uint8_t cursor_flag,uint8_t cursor_limit);
void Interface_Mian_Logic_View(void);
extern struct _disp_menu  _variant_Interface_disp;
extern char variant_interface_SingalMove[11];//单动动作显示字符存放数组
extern char variant_interface_SingalLR[9];

extern uint8_t variant_interface_viewMes[9];
extern uint32_t variant_interface_Time;
extern uint8_t  Count_interface_Refresh;
extern uint8_t variant_Interface_SETTINGcursorLimit;
extern struct _disp_context default_page[12];
extern char variant_interface_Grouptail[4];//成组架号尾
extern char variant_interface_Grouphead[4];//成组架号头
extern char variant_interface_arrows[4];//成组箭头
extern char variant_interface_move[7];//成组动作
#endif

