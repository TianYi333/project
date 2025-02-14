#ifndef __SET_H__
#define __SET_H__
#include "stm32f4xx_hal.h"





typedef struct Set_Struct
{
	char Setting_Char[4];
	uint8_t Setting_Para;
	uint8_t veiw_type;
	
}Set_Struct;

typedef struct Set_Struct_charlong
{
	char Setting_Char[5];
	uint8_t Setting_Para;
	uint8_t veiw_type;
	
}Set_Struct_charlong;

typedef struct Set_Struct_paralong
{
	char Setting_Char[4];
	uint16_t Setting_Para;
	uint8_t veiw_type;
	
}Set_Struct_paralong;


/*****显示类型*****

1  数字变量
2  开1关0
3  允许1禁止0
4  左DIRECTION_LEFT右DIRECTION_RIGHT
5  故障1正常0
6   急停1正常0
*****************/
/***************整体设置*****************/


extern Set_Struct  Structset_GlobalSet_MaxDeviceNum1_1;//最大架号
extern Set_Struct  Structset_GlobalSet_minDeviceNum2_1;//最小架号
extern Set_Struct  Structset_GlobalSet_Settingtime3_1;//主控时间
extern Set_Struct  Structset_GlobalSet_Increase4_4;//地址增向
extern Set_Struct  Structset_GlobalSet_Conswitch5_2;//转换器开关
extern Set_Struct  Structset_GlobalSet_Conposition6_1;//转换器位置
extern Set_Struct  Structset_GlobalSet_PositionUpload7_2;//煤机位置上传
extern Set_Struct  Structset_GlobalSet_Adjacentcheak8_2;//邻架检测
extern Set_Struct  Structset_GlobalSet_AdjacentWarring9_2;//邻架报警
extern Set_Struct  Structset_GlobalSet_Adjacenttime10_1;//邻架检测时间
extern Set_Struct  Structset_GlobalSet_BUSworkcheak11_2;//总线检测
extern Set_Struct  Structset_GlobalSet_BUSreport12_2;//总线报送
extern Set_Struct  Structset_GlobalSet_BUScheakTime13_1;//总线检测时间

/***************服务*****************/
extern Set_Struct  Structset_Service_DeviceNum1_1;//架号
extern Set_Struct  Structset_Service_Adjacentcorrelation2_2;//邻架相关
extern Set_Struct  Structset_Service_RemoteCortrol3_2;//遥控开关


/*****显示类型*****
1  数字变量
2  开1关0
3  允许1禁止0
4  左DIRECTION_LEFT右DIRECTION_RIGHT
5  故障1正常0
6   急停1正常0
*****************/

/***************默认参数*****************/
extern Set_Struct  Structset_Default_Pressurecheak1_2;//立柱压力检测
extern Set_Struct  Structset_Default_Distancecheak2_2;//推移行程检测
extern Set_Struct  Structset_Default_Pushtime3_1;//本架推溜时间
extern Set_Struct_paralong  Structset16_Default_Pushdistance4_1;//本架推溜行程
extern Set_Struct  Structset_Default_PressureLowlimt5_1;//补压力下限
extern Set_Struct  Structset_Default_PressureHighlimt6_1;//补压力上限
extern Set_Struct  Structset_Default_aimPressure7_1;//目标压力
extern Set_Struct  Structset_Default_StepuppressureTime8_1;//单次时间
extern Set_Struct  Structset_Default_StepuppressureGap9_1;//补压间隔
extern Set_Struct  Structset_Default_Stepuppressuretimes10_1;//补压次数



/***************移架*****************/
extern Set_Struct_charlong  Structset_Framemove_Forbiden1_3;//移架动作禁止
extern Set_Struct  Structset_Framemove_Alarm2_2;//移架报警 开关
extern Set_Struct  Structset_Framemove_AlarmTime3_1;//报警时间
extern Set_Struct  Structset_Framemove_movePressure4_1;//移架压力     立柱压力必须卸至此压力以下，程序才允许本架移架。
extern Set_Struct  Structset_Framemove_TransPressure5_1;//过度压力      立柱压力必须升至此压力以上，程序才允许邻架作自动移架。    邻架相关
extern Set_Struct  Structset_Framemove_SupportPressure6_1;//支撑压力   目标压力
extern Set_Struct  Structset_Framemove_TelescopictMove7_2;//伸缩梁动作 
extern Set_Struct  Structset_Framemove_PlateMove8_2;//护帮板动作
extern Set_Struct  Structset_Framemove_ColumndownDelay9_1;//降柱延时  伸缩梁护帮版动作后延时降柱
extern Set_Struct  Structset_Framemove_ColumndownTime10_1;//降柱时间
extern Set_Struct  Structset_Framemove_ColumndownBalanceaction11_2;//降柱时平衡梁动作开关
extern Set_Struct  Structset_Framemove_Columndownsideplate12_2;//降柱侧推开关
extern Set_Struct  Structset_Framemove_DcolumnAgainFrequency13_1;//再降柱次数
extern Set_Struct  Structset_Framemove_BmttomUP14_2;//抬底动作开关
extern Set_Struct_paralong  Structset16_Framemove_Advancaim15_1;//移架行程目标
extern Set_Struct  Structset_Framemove_AdvancTime16_1;//移架时间   移架允许最大时间
extern Set_Struct  Structset_Framemove_Advancsideplate17_2;//移架侧护开关
extern Set_Struct  Structset_Framemove_AdvancagainTime18_1;//再移架时间
extern Set_Struct  Structset_Framemove_ColumnUPtime19_1;//升柱时间
extern Set_Struct  Structset_Framemove_ColumnUPBalanceaction20_2;//升柱平衡开关
extern Set_Struct  Structset_Framemove_ColumnUPsideplatew21_2;//升柱侧护开关
extern Set_Struct  Structset_Framemove_Spray22_2;//移架喷雾开关
extern Set_Struct  Structset_Framemove_SprayTime23_1;//喷雾时间
extern Set_Struct  Structset_Framemove_GroupSeparatedNum24_1;//成组间隔
extern Set_Struct  Structset_Framemove_GroupRange25_1;//成组范围

/****************推溜********************/
extern Set_Struct_charlong  Structset_ChutePush_Forbiden1_3;//推溜动作禁止
extern Set_Struct  Structset_ChutePush_time2_1;//推溜时间
extern Set_Struct_paralong  Structset16_ChutePush_aimDistance3_1;//目标行程
extern Set_Struct  Structset_ChutePush_Pressure4_1;//推溜最小压力
extern Set_Struct_paralong  Structset16_ChutePush_Maximumdifference5_1;//最大差值
extern Set_Struct  Structset_ChutePush_GroupSeparatedNum6_1;//推溜间隔
extern Set_Struct  Structset_ChutePush_GroupRange7_1;//推溜范围


/****************伸缩梁护帮********************/

extern Set_Struct_charlong  Structset_Telescopict_Forbiden1_3;
extern Set_Struct  Structset_Telescopict_StretchTime2_1;
extern Set_Struct  Structset_Telescopict_ShrinkTime3_1;
extern Set_Struct  Structset_Telescopict_DcolumnTime4_1;//动作伸缩梁时的降柱时间
extern Set_Struct  Structset_Telescopict_Separatednum5_1;
extern Set_Struct  Structset_Telescopict_Range6_1;
extern Set_Struct_charlong  Structset_PLATE_Forbiden7_3;
extern Set_Struct  Structset_PLATE_StretchTime8_1;
extern Set_Struct  Structset_PLATE_ShrinkTime9_1;


/****************拉溜和喷雾********************/
extern Set_Struct_charlong  Structset_ChutePull_Forbiden1_3;//拉溜动作禁止
extern Set_Struct  Structset_ChutePull_time2_1;//拉溜时间
extern Set_Struct  Structset_ChutePull_aimDistance3_1;//目标行程
extern Set_Struct  Structset_ChutePull_Pressure4_1;//推溜最小压力
extern Set_Struct  Structset_ChutePull_GroupSeparatedNum5_1;//推溜间隔
extern Set_Struct  Structset_ChutePull_GroupRange6_1;//推溜范围
extern Set_Struct_charlong  Structset_Mistspray_Forbiden7_3;//喷雾间隔
extern Set_Struct  Structset_Mistspray_SeparatedNum8_1;//喷雾间隔
extern Set_Struct  Structset_Mistspray_Range9_1;//范围
extern Set_Struct  Structset_Mistspray_Time10_1;//时间

/****************故障列表********************/
extern Set_Struct_charlong  Structset_Fault_Lcommunicate1_5;//左邻架通信故障
extern Set_Struct_charlong  Structset_Fault_Rcommunicate2_5;//右邻架通信故障
extern Set_Struct_charlong  Structset_Fault_BUScommunicate3_5;//总线通信故障
extern Set_Struct_charlong  Structset_Fault_PressureStepup4_5;//补压故障
extern Set_Struct_charlong  Structset_Fault_Sensorpressure5_5;//压力传感器故障
extern Set_Struct_charlong  Structset_Fault_Sensordistance6_5;//行程传感器故障
extern Set_Struct_charlong  Structset_Fault_Sensorangel7_5;//倾角传感器故障
extern Set_Struct_charlong  Structset_Fault_Receiver8_5;//遥控器接收器故障
extern Set_Struct_charlong  Structset_Fault_Driver9_5;//驱动器故障

/*******************跟机******************/
extern Set_Struct  Structset_Follow_Head1_1;//范围
extern Set_Struct  Structset_Follow_Tail2_1;//时间


void Set_Para_to_char16(uint16_t parameter,uint8_t veiw_type,char *Set_char);
void Set_Para_to_char(uint8_t parameter,uint8_t veiw_type,char *Set_char);
void Set_Init_char(void);
void Init_set_typeFuction(void);
void Set_Init_Para(void);
void  Init_Set_Vlaue(void);
uint8_t PasswdInput_Set_Fun(void);
uint16_t Set_Prar_Change_Input(uint8_t veiw_type,uint8_t Previous_Para);

extern uint8_t Flag_Set_Passwd ;



extern uint8_t Flag_Set_Save ;



#endif



