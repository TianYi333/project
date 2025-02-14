#include "interface.h"
#include "stm32f4xx_hal.h"
#include "main.h"
#include "sensor.h"
#include "bsp_oled19264.h"
#include "main_logic.h"
#include "set.h"
#include "follow.h"
/***********************每行显示的数据***********************************/



struct _disp_menu  _variant_Interface_disp;
char DeviceNum[4];
uint8_t variant_Service_DeviceNum;
char variant_interface_SingalMove[11];//单动动作显示字符存放数组
char variant_interface_SingalLR[9];//单动邻架显示字符存放数组
char variant_interface_Dive[7];//驱动器通信显示
char variant_interface_Grouptail[4];//成组架号尾
char variant_interface_Grouphead[4];//成组架号头
char variant_interface_arrows[4];//成组箭头
char variant_interface_move[7];//成组动作
/***********************显示页面分类***********************************/
 struct _disp_context STOP_page[6] = {
									  //0123456789abcdef/
	 {page_First_Line, column_1_Line, "                "},
	 {page_First_Line, column_1_Line,  "支架:     [急停]"},
	 {page_First_Line, column_6_Line,DeviceNum},
	 {page_Second_Line, column_1_Line, "                "},
	 {page_Third_Line, column_1_Line, "                "},
	 {page_Fouth_Line, column_1_Line, "                "},
	 
 };
 struct _disp_context default_page[12] = {
                                     //0123456789abcdef/
	 {page_First_Line, column_1_Line, "                "},
	{page_First_Line, column_1_Line,  "支架:     [    ]"},
	{page_First_Line, column_6_Line,DeviceNum},
	{page_First_Line, column_12_Line,_variant_Mainlogic_statement.PRESENTstate_view},
	{page_Second_Line,column_1_Line, "                "},
	{page_Second_Line, column_1_Line,_sensor_distance.sensor_view},	
	{page_Second_Line, column_9_Line,_sensor_pressure.sensor_view},
	{page_Third_Line, column_1_Line, "连杆角度:       "},
	{page_Third_Line, column_10_Line, _sensor_Connectingrod_angle.sensor_view},	
    {page_Fouth_Line, column_1_Line, "煤机:     [    ]"},
	{page_Fouth_Line, column_7_Line, array_Follow_Cutterposition},
	{page_Fouth_Line, column_12_Line, array_Follow_Cutterderction},

};
static struct _disp_context Singal_Chose_page[5] = {
                                    //0123456789abcdef/
	{0, column_1_Line, " 单动           "},
	{0, column_9_Line, variant_interface_SingalLR},	
	{1, column_1_Line," 伸   侧护   收 "},
	{2, column_1_Line, " 伸   底调   收 "},
	{3, column_1_Line, " 伸 备用动作 收 "},

};
static struct _disp_context GROUP_Left_page[8] = {
                      //0123456789abcdef/
	{page_First_Line, column_1_Line, "左成组          "},
	{page_First_Line, column_8_Line, variant_interface_move},	
	{page_Second_Line, column_1_Line, "                "},
	{page_Third_Line, column_1_Line, "                "},
	{page_Fouth_Line, column_1_Line, "                "},
	{page_Third_Line, column_3_Line, variant_interface_Grouphead},
	{page_Third_Line, column_7_Line, variant_interface_arrows},
	{page_Third_Line, column_11_Line, variant_interface_Grouptail},
	

};
static struct _disp_context GROUP_Right_page[8] = {
                                    //0123456789abcdef/
	{page_First_Line, column_1_Line, " 右成组         "},
	{page_First_Line, column_8_Line, variant_interface_move},	
	{page_Second_Line, column_1_Line, "                "},
	{page_Third_Line, column_1_Line, "                "},
	{page_Fouth_Line, column_1_Line, "                "},
	{page_Third_Line, column_3_Line, variant_interface_Grouphead},
	{page_Third_Line, column_7_Line, variant_interface_arrows},
	{page_Third_Line, column_11_Line, variant_interface_Grouptail},

};
static struct _disp_context GROUP_MOVE_page[8] = {
                                    //0123456789abcdef/
	{page_First_Line, column_1_Line, "                "},
	{page_First_Line, column_1_Line,  "支架:     [    ]"},
	{page_First_Line, column_6_Line,DeviceNum},
	{page_First_Line, column_12_Line,_variant_Mainlogic_statement.PRESENTstate_view},
	
	{page_Second_Line, column_1_Line, "                "},
	{page_Third_Line, column_1_Line, "                "},
	{page_Fouth_Line, column_1_Line, "                "},
	{page_Third_Line, column_1_Line, " 正在成组动作    "},


};
static struct _disp_context GROUP_ALARM_page[8] = {
                                    //0123456789abcdef/
	{page_First_Line, column_1_Line, "                "},
	{page_First_Line, column_1_Line,  "支架:     [    ]"},
	{page_First_Line, column_6_Line,DeviceNum},
	{page_First_Line, column_12_Line,_variant_Mainlogic_statement.PRESENTstate_view},
	
	{page_Second_Line, column_1_Line, "                "},
	{page_Third_Line, column_1_Line, "                "},
	{page_Fouth_Line, column_1_Line, "                "},
	{page_Third_Line, column_1_Line, " 成组准备   "},

};
static struct _disp_context Bus_move[5] = {
                                    //0123456789abcdef/
	{page_Second_Line, column_1_Line,"正在总线单动动作 "},
	{page_Third_Line, column_1_Line, "                "},
	{page_Third_Line, column_1_Line, variant_interface_SingalMove},
	{page_Fouth_Line, column_1_Line, "                "},
	{page_Fouth_Line, column_6_Line, variant_interface_Dive},
};

static struct _disp_context Singal_move[4] = {
                                    //0123456789abcdef/
	{page_Second_Line, column_1_Line,"正在邻架单动控制 "},
	{page_Third_Line, column_1_Line, "                "},
	{page_Third_Line, column_1_Line, variant_interface_SingalMove},
	{page_Fouth_Line, column_1_Line, "                "},
};

struct _disp_context Singal_Controled_move[5] = {
                                    //0123456789abcdef/
	{page_Second_Line, column_1_Line,"正在邻架单动动作 "},
	{page_Third_Line, column_1_Line, "                "},
	{page_Third_Line, column_1_Line, variant_interface_SingalMove},
	{page_Fouth_Line, column_1_Line, "                "},
	{page_Fouth_Line, column_6_Line, variant_interface_Dive},
	
};
   
struct _disp_context Follow_Controled_move[4] = {
                                    //0123456789abcdef/
	{page_Second_Line, column_1_Line,"正在跟机动作     "},
	{page_Third_Line, column_1_Line, "                "},
	{page_Third_Line, column_1_Line, variant_interface_SingalMove},
	{page_Fouth_Line, column_1_Line, "                "},

	
};




struct _disp_context setting_GLOBAL_SET[28] = {
                     //0123456789abcdef/
	{0, column_1_Line,"    整体设置    "},
	
	{1, column_1_Line,"最大架号:       "},
	{1, column_13_Line,Structset_GlobalSet_MaxDeviceNum1_1.Setting_Char},
	
	{2, column_1_Line,"最小架号:       "},
	{2, column_13_Line,Structset_GlobalSet_minDeviceNum2_1.Setting_Char},
	
	{3, column_1_Line,"主控时间:      s"},
	{3, column_13_Line,Structset_GlobalSet_Settingtime3_1.Setting_Char},
	
	{4, column_1_Line,"地址增向:       "},
	{4, column_14_Line,Structset_GlobalSet_Increase4_4.Setting_Char},
	
	{5, column_1_Line,"转换器开关:     "},
	{5, column_14_Line,Structset_GlobalSet_Conswitch5_2.Setting_Char},
	
	{6, column_1_Line,"煤机位置:       "},
	{6, column_13_Line,Structset_GlobalSet_Conposition6_1.Setting_Char},
	
	{7, column_1_Line,"转换器位置:     "},
	{7, column_14_Line,Structset_GlobalSet_PositionUpload7_2.Setting_Char},
	
	{8, column_1_Line,"邻架检测:       "},
	{8, column_14_Line,Structset_GlobalSet_Adjacentcheak8_2.Setting_Char},
	
	{9, column_1_Line,"邻架报警:       "},
	{9, column_14_Line,Structset_GlobalSet_AdjacentWarring9_2.Setting_Char},
	
	{10, column_1_Line,"邻检时间:     s"},
	{10, column_13_Line,Structset_GlobalSet_Adjacenttime10_1.Setting_Char},
	
	{11, column_1_Line,"总线检测:       "},
	{11, column_14_Line,Structset_GlobalSet_BUSworkcheak11_2.Setting_Char},
	
	{12, column_1_Line,"总线报送:       "},
  	{12, column_14_Line,Structset_GlobalSet_BUSreport12_2.Setting_Char},
					  //0123456789abcdef/
	{13, column_1_Line,"总检时间:      s"},
	{13, column_13_Line,Structset_GlobalSet_BUScheakTime13_1.Setting_Char},
};

/***************服务*****************/


struct _disp_context setting_SERVICE[11] = {
                     //0123456789abcdef/
	{0, column_1_Line,"      服务      "},
	
	
	{1, column_1_Line,"支架架号:       "},
	{1, column_13_Line,Structset_Service_DeviceNum1_1.Setting_Char},
	
	{2, column_1_Line,"邻架相关:       "},
	{2, column_14_Line,Structset_Service_Adjacentcorrelation2_2.Setting_Char},
	
	{3, column_1_Line,"遥控开关:       "},
	{3, column_14_Line,Structset_Service_RemoteCortrol3_2.Setting_Char},
	
	{4, column_1_Line,"控制器参数传递  "},
	
	{5, column_1_Line,"控制器编号传递  "},
	
	{6, column_1_Line,"右邻架重启      "},
	
	{7, column_1_Line,"左邻架重启      "},
	
};
struct _disp_context setting_DEFAULT_PARAMETERS[21] = {
                     //0123456789abcdef/
	{0, column_1_Line,"     默认参数    "},
					  //0123456789abcdef/
	{1, column_1_Line, "立柱压力:       "},
	{1, column_14_Line, Structset_Default_Pressurecheak1_2.Setting_Char},
	
	{2, column_1_Line, "推移行程:       "},
	{2, column_14_Line, Structset_Default_Distancecheak2_2.Setting_Char},
	
	{3, column_1_Line, "自推溜时间:    s"},
	{3, column_13_Line, Structset_Default_Pushtime3_1.Setting_Char},
	
	{4, column_1_Line, "自推溜行程:     "},
	{4, column_13_Line, Structset16_Default_Pushdistance4_1.Setting_Char},
	
	{5, column_1_Line, "补压下限:   MPa"},
	{5, column_10_Line, Structset_Default_PressureLowlimt5_1.Setting_Char},
	
	{6, column_1_Line, "补压上限:   MPa"},
	{6, column_10_Line, Structset_Default_PressureHighlimt6_1.Setting_Char},
	
	{7, column_1_Line, "目标压力:   MPa"},
	{7, column_10_Line, Structset_Default_aimPressure7_1.Setting_Char},
	
	{8, column_1_Line, "单次时间:      s"},
	{8, column_13_Line, Structset_Default_StepuppressureTime8_1.Setting_Char},
	
	{9, column_1_Line, "补压间隔:      s"},
	{9, column_13_Line, Structset_Default_StepuppressureGap9_1.Setting_Char},
	
	{10, column_1_Line, "补压次数:      "},
	{10, column_13_Line, Structset_Default_Stepuppressuretimes10_1.Setting_Char},

};
struct _disp_context setting_ADVANCING[51] = {
                     //0123456789abcdef/
	{0, column_1_Line,"      移架      "},
					  //0123456789abcdef/
	{1, column_1_Line, "移架动作:       "},
	{1, column_13_Line, Structset_Framemove_Forbiden1_3.Setting_Char},
	
	{2, column_1_Line, "移架报警:       "},
	{2, column_14_Line, Structset_Framemove_Alarm2_2.Setting_Char},
	
	{3, column_1_Line, "报警时间:      s"},
	{3, column_13_Line, Structset_Framemove_AlarmTime3_1.Setting_Char},
	
	{4, column_1_Line, "移架压力:    MPa"},//立柱压力必须卸至此压力以下，程序才允许本架移架。
	{4, column_11_Line, Structset_Framemove_movePressure4_1.Setting_Char},
	
	{5, column_1_Line, "过度压力:    MPa"},
	{5, column_11_Line, Structset_Framemove_TransPressure5_1.Setting_Char},
	
	{6, column_1_Line, "支撑压力:    MPa"},
	{6, column_11_Line, Structset_Framemove_SupportPressure6_1.Setting_Char},
	
	{7, column_1_Line, "伸缩梁动作:     "},
	{7, column_14_Line, Structset_Framemove_TelescopictMove7_2.Setting_Char},
	
	{8, column_1_Line, "护帮板动作:     "},
	{8, column_14_Line, Structset_Framemove_PlateMove8_2.Setting_Char},
	
	{9, column_1_Line, "降柱延时:      s"},
	{9, column_13_Line, Structset_Framemove_ColumndownDelay9_1.Setting_Char},
	
	{10, column_1_Line, "降柱时间:      s"},
	{10, column_13_Line, Structset_Framemove_ColumndownTime10_1.Setting_Char},
	
	{11, column_1_Line, "降柱平衡梁:     "},
	{11, column_14_Line, Structset_Framemove_ColumndownBalanceaction11_2.Setting_Char},
	
	{12, column_1_Line, "降柱侧推:       "},
	{12, column_14_Line, Structset_Framemove_Columndownsideplate12_2.Setting_Char},
	
	{13, column_1_Line, "再降柱次数:     "},
	{13, column_13_Line, Structset_Framemove_DcolumnAgainFrequency13_1.Setting_Char},
	
	{14, column_1_Line, "抬底动作:       "},
	{14, column_14_Line, Structset_Framemove_BmttomUP14_2.Setting_Char},
	
	{15, column_1_Line, "行程目标:     mm"},
	{15, column_12_Line, Structset16_Framemove_Advancaim15_1.Setting_Char},
	
	{16, column_1_Line, "移架时间:      s"},
	{16, column_13_Line, Structset_Framemove_AdvancTime16_1.Setting_Char},
	
	{17, column_1_Line, "移架侧护:       "},
	{17, column_14_Line, Structset_Framemove_Advancsideplate17_2.Setting_Char},
	
	{18, column_1_Line, "再移架时间:    s"},
	{18, column_13_Line, Structset_Framemove_AdvancagainTime18_1.Setting_Char},
	
	{19, column_1_Line, "升柱时间:      s"},
	{19, column_13_Line, Structset_Framemove_ColumnUPtime19_1.Setting_Char},
	
	{20, column_1_Line, "升柱平衡:       "},
	{20, column_14_Line, Structset_Framemove_ColumnUPBalanceaction20_2.Setting_Char},
	
	{21, column_1_Line, "升柱侧护:       "},
	{21, column_14_Line, Structset_Framemove_ColumnUPsideplatew21_2.Setting_Char},
	
	{22, column_1_Line, "移架喷雾:       "},
	{22, column_14_Line, Structset_Framemove_Spray22_2.Setting_Char},
	
	{23, column_1_Line, "喷雾时间:      s"},
	{23, column_13_Line, Structset_Framemove_SprayTime23_1.Setting_Char},
	
	{24, column_1_Line, "成组间隔:       "},
	{24, column_13_Line, Structset_Framemove_GroupSeparatedNum24_1.Setting_Char},
	
	{25, column_1_Line, "成组范围:       "},
	{25, column_13_Line, Structset_Framemove_GroupRange25_1.Setting_Char},

};
struct _disp_context setting_GROUP_CHUTE_PUSH[15] = {
                     //0123456789abcdef/
	{0, column_1_Line,"      推溜      "},
				      //0123456789abcdef/
	{1, column_1_Line, "推溜动作:       "},
	{1, column_13_Line, Structset_ChutePush_Forbiden1_3.Setting_Char},
	
	{2, column_1_Line, "推溜时间:      s"},
	{2, column_13_Line, Structset_ChutePush_time2_1.Setting_Char},
	
	{3, column_1_Line, "目标行程:     mm"},
	{3, column_12_Line, Structset16_ChutePush_aimDistance3_1.Setting_Char},
	
	{4, column_1_Line, "最小压力:    MPa"},
	{4, column_11_Line, Structset_ChutePush_Pressure4_1.Setting_Char},
	
	{5, column_1_Line, "最大差值:     mm"},
	{5, column_12_Line, Structset16_ChutePush_Maximumdifference5_1.Setting_Char},
	
	{6, column_1_Line, "推溜间隔:       "},
	{6, column_13_Line, Structset_ChutePush_GroupSeparatedNum6_1.Setting_Char},
	
	{7, column_1_Line, "推溜范围:       "},
	{7, column_13_Line, Structset_ChutePush_GroupRange7_1.Setting_Char},
};
struct _disp_context setting_Telescopict[19] = {
                     //0123456789abcdef/
	{0, column_1_Line,"    前梁护帮   "},
		              //0123456789abcdef/
	{1, column_1_Line, "前梁动作:       "},
	{1, column_13_Line, Structset_Telescopict_Forbiden1_3.Setting_Char},
	                  //0123456789abcdef/
	{2, column_1_Line, "伸出时间:      s"},
	{2, column_13_Line, Structset_Telescopict_StretchTime2_1.Setting_Char},
	
	{3, column_1_Line, "缩回时间:      s"},
	{3, column_13_Line, Structset_Telescopict_ShrinkTime3_1.Setting_Char},
	
	{4, column_1_Line, "降柱时间:      s"},
	{4, column_13_Line, Structset_Telescopict_DcolumnTime4_1.Setting_Char},
	
	{5, column_1_Line, "前梁护帮间隔:   "},
	{5, column_13_Line, Structset_Telescopict_Separatednum5_1.Setting_Char},
	
	{6, column_1_Line, "前梁护帮范围:   "},
	{6, column_13_Line, Structset_Telescopict_Range6_1.Setting_Char},
	
	{7, column_1_Line, "护帮动作:       "},
	{7, column_13_Line, Structset_PLATE_Forbiden7_3.Setting_Char},
	
	{8, column_1_Line, "伸护帮时间:    s"},
	{8, column_13_Line, Structset_PLATE_StretchTime8_1.Setting_Char},
	
	{9, column_1_Line, "收护帮时间:    s"},
	{9, column_13_Line, Structset_PLATE_ShrinkTime9_1.Setting_Char},
};
struct _disp_context setting_ChutePullMistspray[21] = {
                      //0123456789abcdef/
	{0, column_1_Line, "   拉溜和喷雾   "},
	                  //0123456789abcdef/
	{1, column_1_Line, "拉溜动作:       "},
	{1, column_13_Line, Structset_ChutePull_Forbiden1_3.Setting_Char},
	
	{2, column_1_Line, "拉溜时间:      s"},
	{2, column_13_Line, Structset_ChutePull_time2_1.Setting_Char},
	
	{3, column_1_Line, "目标行程:     mm"},
	{3, column_12_Line, Structset_ChutePull_aimDistance3_1.Setting_Char},
	
	{4, column_1_Line, "最小压力:    MPa"},
	{4, column_11_Line, Structset_ChutePull_Pressure4_1.Setting_Char},
	
	{5, column_1_Line, "拉溜间隔:       "},
	{5, column_13_Line, Structset_ChutePull_GroupSeparatedNum5_1.Setting_Char},
	
	{6, column_1_Line, "拉溜范围:       "},
	{6, column_13_Line, Structset_ChutePull_GroupRange6_1.Setting_Char},
	
	{7, column_1_Line, "喷雾动作:       "},
	{7, column_13_Line, Structset_Mistspray_Forbiden7_3.Setting_Char},
	
	{8, column_1_Line, "喷雾间隔:       "},
	{8, column_13_Line, Structset_Mistspray_SeparatedNum8_1.Setting_Char},
	
	{9, column_1_Line, "喷雾范围:       "},
	{9, column_13_Line, Structset_Mistspray_Range9_1.Setting_Char},
	
	{10, column_1_Line, "喷雾时间:      s"},
	{10, column_13_Line, Structset_Mistspray_Time10_1.Setting_Char},
};
struct _disp_context setting_NC[4] = {
                                    //0123456789abcdef/
	{0, column_1_Line,"      备用      "},
	
	{1, column_1_Line, "                "},

	{2, column_1_Line, "                "},
	
	{3, column_1_Line, "                "},
	
};
struct _disp_context setting_AUTOMATION[7] = {
                     //0123456789abcdef/
	{0, column_1_Line,"      跟机      "},
	
	{1, column_1_Line, "开启跟机         "},

	{2, column_1_Line, "结束跟机        "},
	
	{3, column_1_Line, "跟机机头        "},
	{3, column_13_Line, Structset_Follow_Head1_1.Setting_Char},
	
	{4, column_1_Line, "跟机机尾        "},
	{4, column_13_Line, Structset_Follow_Tail2_1.Setting_Char},
};
struct _disp_context setting_FAULT[20] = {
                     //0123456789abcdef/
	{0, column_1_Line,"     故障列表    "},
					  //0123456789abcdef/
	{1, column_1_Line, "左邻架通讯:      "},
	{1, column_13_Line, Structset_Fault_Lcommunicate1_5.Setting_Char},
	
	{2, column_1_Line, "右邻架通讯:      "},
	{2, column_13_Line, Structset_Fault_Rcommunicate2_5.Setting_Char},
	
	{3, column_1_Line, "总线通讯:        "},
	{3, column_13_Line, Structset_Fault_BUScommunicate3_5.Setting_Char},
	
	{4, column_1_Line, "自动补压:        "},
	{4, column_13_Line, Structset_Fault_PressureStepup4_5.Setting_Char},
	
	{5, column_1_Line, "压力传感器:      "},
	{5, column_13_Line, Structset_Fault_Sensorpressure5_5.Setting_Char},
	
	{6, column_1_Line, "行程传感器:      "},
	{6, column_13_Line, Structset_Fault_Sensordistance6_5.Setting_Char},
	
	{7, column_1_Line, "倾角传感器:      "},
	{7, column_13_Line, Structset_Fault_Sensorangel7_5.Setting_Char},
	
	{8, column_1_Line, "遥控接收器:      "},
	{8, column_13_Line, Structset_Fault_Receiver8_5.Setting_Char},
	
	{9, column_1_Line, "驱动器:          "},
	{9, column_13_Line, Structset_Fault_Driver9_5.Setting_Char},
	
	{10, column_1_Line, "急停列表:       "},
};

static struct _disp_context Workcondition1[7] = {
                      //0123456789abcdef//
	
	
	{page_First_Line, column_1_Line, "      工况      "},
	{page_Second_Line, column_1_Line,"顶梁:           "},
	{page_Second_Line, column_7_Line,_sensor_canopy_angle.sensor_view},
	{page_Third_Line, column_1_Line, "连杆:           "},
	{page_Third_Line, column_7_Line,_sensor_Connectingrod_angle.sensor_view},
	{page_Fouth_Line, column_1_Line, "平衡:           "},
	{page_Fouth_Line, column_7_Line,_sensor_Plate_angle.sensor_view},

};
static struct _disp_context Workcondition2[7] = {
                      //0123456789abcdef//
	{page_First_Line, column_1_Line, "      工况      "},
	{page_Second_Line, column_1_Line,"行程:           "},
	{page_Second_Line, column_7_Line, _sensor_distance.sensor_view},	
	{page_Third_Line, column_1_Line, "压力:           "},
	{page_Third_Line, column_7_Line, _sensor_pressure.sensor_view},
	{page_Fouth_Line, column_1_Line, "支架高度:     mm"},
	{page_Fouth_Line, column_10_Line, array_Altitude_view},
};


/***********************默认初始界面***********************************/

uint8_t variant_Interface_SETTINGcursorLimit;
uint8_t variant_interface_viewMes[9];
uint32_t variant_interface_Time;
uint8_t  Count_interface_Refresh;
/***********************光标显示位置***********************************/

void Display_context_show(_disp_context s[],uint8_t length,uint8_t cursor_flag,uint8_t cursor_limit)
{



		if(cursor_flag == 0)
		{
				for(uint8_t i=0;i<length;i++)
					{				
						Display_String(s[i].page_y,s[i].column_x,s[i].Use_Char);			
					}	
		}					
		else if(cursor_flag == 1)
		{
		


				if(_variant_Interface_disp.cursor == 1)
				{
					for(uint8_t i=0;i<length;i++)
					{				
						if(s[i].page_y ==  0)                                                                                                                                                                                                                                    
							Display_String(page_First_Line,s[i].column_x,s[i].Use_Char);
						else if(s[i].page_y  ==  _variant_Interface_disp.cursor)
							Display_String_Reverse(page_Second_Line ,s[i].column_x,s[i].Use_Char);
						else if(s[i].page_y  ==  (_variant_Interface_disp.cursor+1))
							Display_String(page_Third_Line ,s[i].column_x,s[i].Use_Char);
						else if(s[i].page_y  ==  (_variant_Interface_disp.cursor+2))
							Display_String(page_Fouth_Line ,s[i].column_x,s[i].Use_Char);
						
					}
				}
				else if((_variant_Interface_disp.cursor > 1)&&(_variant_Interface_disp.cursor<cursor_limit))		
				{					
					
					for(uint8_t i=0;i<length;i++)
					{				
						if(s[i].page_y == 0)                                                                                                                                                                                                                                    
							Display_String(page_First_Line,s[i].column_x,s[i].Use_Char);
						else if(s[i].page_y  ==  (_variant_Interface_disp.cursor-1))
							Display_String(page_Second_Line ,s[i].column_x,s[i].Use_Char);
						else if(s[i].page_y  ==  _variant_Interface_disp.cursor)
							Display_String_Reverse(page_Third_Line ,s[i].column_x,s[i].Use_Char);
						else if(s[i].page_y  ==  (_variant_Interface_disp.cursor+1))
							Display_String(page_Fouth_Line ,s[i].column_x,s[i].Use_Char);			
					}
					
		
				}
				else if(_variant_Interface_disp.cursor == cursor_limit)		
				{					
					
					for(uint8_t i=0;i<length;i++)
					{				
						if(s[i].page_y == 0)                                                                                                                                                                                                                                    
							Display_String(page_First_Line,s[i].column_x,s[i].Use_Char);
						else if(s[i].page_y  ==  (_variant_Interface_disp.cursor-2))
							Display_String(page_Second_Line ,s[i].column_x,s[i].Use_Char);
						else if(s[i].page_y  ==  _variant_Interface_disp.cursor-1)
							Display_String(page_Third_Line ,s[i].column_x,s[i].Use_Char);
						else if(s[i].page_y  ==  (_variant_Interface_disp.cursor))
							Display_String_Reverse(page_Fouth_Line ,s[i].column_x,s[i].Use_Char);			
					}
					
		
				}
		
		}
		
		
		
		

}

void Interface_Mian_Logic_View(void)
{
	
			switch(_variant_Interface_disp.type)
			{
			
				case TYPE_DEFAULT:
					Main_logic_STATEchose();
					Display_context_show(default_page,12,0,0);
					break;				
				case TYPE_SINGLE_CHOSE:
					Display_context_show(Singal_Chose_page,5,1,3);
					break;			
				case TYPE_SINGLE_MOVE:
            Display_context_show(Singal_move,4,0,0);          
					break;		          
       case TYPE_SINGLE_MOVEED:                     
          if( state_controlded_movetype == 0)
         {
            Display_context_show(Singal_Controled_move,5,0,0);
          }
          else
          {
            Display_context_show(Bus_move,5,0,0);
          }
					break;     
				case TYPE_SETTING_GLOBAL_SET:
					Display_context_show(setting_GLOBAL_SET,27,1,13);
					break;
				case TYPE_SETTING_SERVICE:
					Display_context_show(setting_SERVICE,11,1,7);
					break;
				case TYPE_SETTING_DEFAULT_PARAMETERS:
					Display_context_show(setting_DEFAULT_PARAMETERS,21,1,10);
					break;
				case TYPE_SETTING_GROUP_ADVANCING:
					Display_context_show(setting_ADVANCING,51,1,25);
					break;
				case TYPE_SETTING_GROUP_CHUTE_PUSH:
					Display_context_show(setting_GROUP_CHUTE_PUSH,15,1,7);
					break;
				case TYPE_SETTING_GROUP_BALANCE:
					Display_context_show(setting_ChutePullMistspray,21,1,10);
					break;
				case TYPE_SETTING_GROUP_TELESCOPIC:
					Display_context_show(setting_Telescopict,19,1,9);
					break;
				case TYPE_SETTING_GROUP_NC:
					Display_context_show(setting_NC,4,1,3 );
					break;
				case TYPE_SETTING_AUTOMATION:
					Display_context_show(setting_AUTOMATION,7,1,4);
					break;
				case TYPE_SETTING_FAULT:
					Display_context_show(setting_FAULT,20,1,10);
					break;
				case TYPE_WORKINGCONDITION_WORKINGCONDITION1:
					Display_context_show(Workcondition1,7,0,0);
					break;
				case TYPE_WORKINGCONDITION_WORKINGCONDITION2:
					Display_context_show(Workcondition2,7,0,0);
					break;
				  case TYPE_URGENCY_STOP:
					Display_context_show(STOP_page,6,0,0);
					break;
				  case TYPE_GROUP_RIGHT:
					Display_context_show(GROUP_Right_page,8,0,0);
					break;
				  case TYPE_GROUP_LEFT:
					Display_context_show(GROUP_Left_page,8,0,0);
					break;
				  case TYPE_GROUP_MOVE:
					  Main_logic_STATEchose();
					Display_context_show(GROUP_MOVE_page,8,0,0);
					break;
				  case TYPE_GROUP_ALARM:
					  Main_logic_STATEchose();
					Display_context_show(GROUP_ALARM_page,8,0,0);
					break;
				  case TYPE_FOLLOW_MOVE:
					Display_context_show(Follow_Controled_move,4,0,0);
					break;
			}
///***********************_variant_Interface_disp.cursor用***********************************/
}


