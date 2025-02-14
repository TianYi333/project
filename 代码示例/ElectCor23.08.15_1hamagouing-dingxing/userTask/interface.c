#include "interface.h"
#include "stm32f4xx_hal.h"
#include "main.h"
#include "sensor.h"
#include "bsp_oled19264.h"
#include "main_logic.h"
#include "set.h"
#include "follow.h"
/***********************ÿ����ʾ������***********************************/



struct _disp_menu  _variant_Interface_disp;
char DeviceNum[4];
uint8_t variant_Service_DeviceNum;
char variant_interface_SingalMove[11];//����������ʾ�ַ��������
char variant_interface_SingalLR[9];//�����ڼ���ʾ�ַ��������
char variant_interface_Dive[7];//������ͨ����ʾ
char variant_interface_Grouptail[4];//����ܺ�β
char variant_interface_Grouphead[4];//����ܺ�ͷ
char variant_interface_arrows[4];//�����ͷ
char variant_interface_move[7];//���鶯��
/***********************��ʾҳ�����***********************************/
 struct _disp_context STOP_page[6] = {
									  //0123456789abcdef/
	 {page_First_Line, column_1_Line, "                "},
	 {page_First_Line, column_1_Line,  "֧��:     [��ͣ]"},
	 {page_First_Line, column_6_Line,DeviceNum},
	 {page_Second_Line, column_1_Line, "                "},
	 {page_Third_Line, column_1_Line, "                "},
	 {page_Fouth_Line, column_1_Line, "                "},
	 
 };
 struct _disp_context default_page[12] = {
                                     //0123456789abcdef/
	 {page_First_Line, column_1_Line, "                "},
	{page_First_Line, column_1_Line,  "֧��:     [    ]"},
	{page_First_Line, column_6_Line,DeviceNum},
	{page_First_Line, column_12_Line,_variant_Mainlogic_statement.PRESENTstate_view},
	{page_Second_Line,column_1_Line, "                "},
	{page_Second_Line, column_1_Line,_sensor_distance.sensor_view},	
	{page_Second_Line, column_9_Line,_sensor_pressure.sensor_view},
	{page_Third_Line, column_1_Line, "���˽Ƕ�:       "},
	{page_Third_Line, column_10_Line, _sensor_Connectingrod_angle.sensor_view},	
    {page_Fouth_Line, column_1_Line, "ú��:     [    ]"},
	{page_Fouth_Line, column_7_Line, array_Follow_Cutterposition},
	{page_Fouth_Line, column_12_Line, array_Follow_Cutterderction},

};
static struct _disp_context Singal_Chose_page[5] = {
                                    //0123456789abcdef/
	{0, column_1_Line, " ����           "},
	{0, column_9_Line, variant_interface_SingalLR},	
	{1, column_1_Line," ��   �໤   �� "},
	{2, column_1_Line, " ��   �׵�   �� "},
	{3, column_1_Line, " �� ���ö��� �� "},

};
static struct _disp_context GROUP_Left_page[8] = {
                      //0123456789abcdef/
	{page_First_Line, column_1_Line, "�����          "},
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
	{page_First_Line, column_1_Line, " �ҳ���         "},
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
	{page_First_Line, column_1_Line,  "֧��:     [    ]"},
	{page_First_Line, column_6_Line,DeviceNum},
	{page_First_Line, column_12_Line,_variant_Mainlogic_statement.PRESENTstate_view},
	
	{page_Second_Line, column_1_Line, "                "},
	{page_Third_Line, column_1_Line, "                "},
	{page_Fouth_Line, column_1_Line, "                "},
	{page_Third_Line, column_1_Line, " ���ڳ��鶯��    "},


};
static struct _disp_context GROUP_ALARM_page[8] = {
                                    //0123456789abcdef/
	{page_First_Line, column_1_Line, "                "},
	{page_First_Line, column_1_Line,  "֧��:     [    ]"},
	{page_First_Line, column_6_Line,DeviceNum},
	{page_First_Line, column_12_Line,_variant_Mainlogic_statement.PRESENTstate_view},
	
	{page_Second_Line, column_1_Line, "                "},
	{page_Third_Line, column_1_Line, "                "},
	{page_Fouth_Line, column_1_Line, "                "},
	{page_Third_Line, column_1_Line, " ����׼��   "},

};
static struct _disp_context Bus_move[5] = {
                                    //0123456789abcdef/
	{page_Second_Line, column_1_Line,"�������ߵ������� "},
	{page_Third_Line, column_1_Line, "                "},
	{page_Third_Line, column_1_Line, variant_interface_SingalMove},
	{page_Fouth_Line, column_1_Line, "                "},
	{page_Fouth_Line, column_6_Line, variant_interface_Dive},
};

static struct _disp_context Singal_move[4] = {
                                    //0123456789abcdef/
	{page_Second_Line, column_1_Line,"�����ڼܵ������� "},
	{page_Third_Line, column_1_Line, "                "},
	{page_Third_Line, column_1_Line, variant_interface_SingalMove},
	{page_Fouth_Line, column_1_Line, "                "},
};

struct _disp_context Singal_Controled_move[5] = {
                                    //0123456789abcdef/
	{page_Second_Line, column_1_Line,"�����ڼܵ������� "},
	{page_Third_Line, column_1_Line, "                "},
	{page_Third_Line, column_1_Line, variant_interface_SingalMove},
	{page_Fouth_Line, column_1_Line, "                "},
	{page_Fouth_Line, column_6_Line, variant_interface_Dive},
	
};
   
struct _disp_context Follow_Controled_move[4] = {
                                    //0123456789abcdef/
	{page_Second_Line, column_1_Line,"���ڸ�������     "},
	{page_Third_Line, column_1_Line, "                "},
	{page_Third_Line, column_1_Line, variant_interface_SingalMove},
	{page_Fouth_Line, column_1_Line, "                "},

	
};




struct _disp_context setting_GLOBAL_SET[28] = {
                     //0123456789abcdef/
	{0, column_1_Line,"    ��������    "},
	
	{1, column_1_Line,"���ܺ�:       "},
	{1, column_13_Line,Structset_GlobalSet_MaxDeviceNum1_1.Setting_Char},
	
	{2, column_1_Line,"��С�ܺ�:       "},
	{2, column_13_Line,Structset_GlobalSet_minDeviceNum2_1.Setting_Char},
	
	{3, column_1_Line,"����ʱ��:      s"},
	{3, column_13_Line,Structset_GlobalSet_Settingtime3_1.Setting_Char},
	
	{4, column_1_Line,"��ַ����:       "},
	{4, column_14_Line,Structset_GlobalSet_Increase4_4.Setting_Char},
	
	{5, column_1_Line,"ת��������:     "},
	{5, column_14_Line,Structset_GlobalSet_Conswitch5_2.Setting_Char},
	
	{6, column_1_Line,"ú��λ��:       "},
	{6, column_13_Line,Structset_GlobalSet_Conposition6_1.Setting_Char},
	
	{7, column_1_Line,"ת����λ��:     "},
	{7, column_14_Line,Structset_GlobalSet_PositionUpload7_2.Setting_Char},
	
	{8, column_1_Line,"�ڼܼ��:       "},
	{8, column_14_Line,Structset_GlobalSet_Adjacentcheak8_2.Setting_Char},
	
	{9, column_1_Line,"�ڼܱ���:       "},
	{9, column_14_Line,Structset_GlobalSet_AdjacentWarring9_2.Setting_Char},
	
	{10, column_1_Line,"�ڼ�ʱ��:     s"},
	{10, column_13_Line,Structset_GlobalSet_Adjacenttime10_1.Setting_Char},
	
	{11, column_1_Line,"���߼��:       "},
	{11, column_14_Line,Structset_GlobalSet_BUSworkcheak11_2.Setting_Char},
	
	{12, column_1_Line,"���߱���:       "},
  	{12, column_14_Line,Structset_GlobalSet_BUSreport12_2.Setting_Char},
					  //0123456789abcdef/
	{13, column_1_Line,"�ܼ�ʱ��:      s"},
	{13, column_13_Line,Structset_GlobalSet_BUScheakTime13_1.Setting_Char},
};

/***************����*****************/


struct _disp_context setting_SERVICE[11] = {
                     //0123456789abcdef/
	{0, column_1_Line,"      ����      "},
	
	
	{1, column_1_Line,"֧�ܼܺ�:       "},
	{1, column_13_Line,Structset_Service_DeviceNum1_1.Setting_Char},
	
	{2, column_1_Line,"�ڼ����:       "},
	{2, column_14_Line,Structset_Service_Adjacentcorrelation2_2.Setting_Char},
	
	{3, column_1_Line,"ң�ؿ���:       "},
	{3, column_14_Line,Structset_Service_RemoteCortrol3_2.Setting_Char},
	
	{4, column_1_Line,"��������������  "},
	
	{5, column_1_Line,"��������Ŵ���  "},
	
	{6, column_1_Line,"���ڼ�����      "},
	
	{7, column_1_Line,"���ڼ�����      "},
	
};
struct _disp_context setting_DEFAULT_PARAMETERS[21] = {
                     //0123456789abcdef/
	{0, column_1_Line,"     Ĭ�ϲ���    "},
					  //0123456789abcdef/
	{1, column_1_Line, "����ѹ��:       "},
	{1, column_14_Line, Structset_Default_Pressurecheak1_2.Setting_Char},
	
	{2, column_1_Line, "�����г�:       "},
	{2, column_14_Line, Structset_Default_Distancecheak2_2.Setting_Char},
	
	{3, column_1_Line, "������ʱ��:    s"},
	{3, column_13_Line, Structset_Default_Pushtime3_1.Setting_Char},
	
	{4, column_1_Line, "�������г�:     "},
	{4, column_13_Line, Structset16_Default_Pushdistance4_1.Setting_Char},
	
	{5, column_1_Line, "��ѹ����:   MPa"},
	{5, column_10_Line, Structset_Default_PressureLowlimt5_1.Setting_Char},
	
	{6, column_1_Line, "��ѹ����:   MPa"},
	{6, column_10_Line, Structset_Default_PressureHighlimt6_1.Setting_Char},
	
	{7, column_1_Line, "Ŀ��ѹ��:   MPa"},
	{7, column_10_Line, Structset_Default_aimPressure7_1.Setting_Char},
	
	{8, column_1_Line, "����ʱ��:      s"},
	{8, column_13_Line, Structset_Default_StepuppressureTime8_1.Setting_Char},
	
	{9, column_1_Line, "��ѹ���:      s"},
	{9, column_13_Line, Structset_Default_StepuppressureGap9_1.Setting_Char},
	
	{10, column_1_Line, "��ѹ����:      "},
	{10, column_13_Line, Structset_Default_Stepuppressuretimes10_1.Setting_Char},

};
struct _disp_context setting_ADVANCING[51] = {
                     //0123456789abcdef/
	{0, column_1_Line,"      �Ƽ�      "},
					  //0123456789abcdef/
	{1, column_1_Line, "�Ƽܶ���:       "},
	{1, column_13_Line, Structset_Framemove_Forbiden1_3.Setting_Char},
	
	{2, column_1_Line, "�Ƽܱ���:       "},
	{2, column_14_Line, Structset_Framemove_Alarm2_2.Setting_Char},
	
	{3, column_1_Line, "����ʱ��:      s"},
	{3, column_13_Line, Structset_Framemove_AlarmTime3_1.Setting_Char},
	
	{4, column_1_Line, "�Ƽ�ѹ��:    MPa"},//����ѹ������ж����ѹ�����£�������������Ƽܡ�
	{4, column_11_Line, Structset_Framemove_movePressure4_1.Setting_Char},
	
	{5, column_1_Line, "����ѹ��:    MPa"},
	{5, column_11_Line, Structset_Framemove_TransPressure5_1.Setting_Char},
	
	{6, column_1_Line, "֧��ѹ��:    MPa"},
	{6, column_11_Line, Structset_Framemove_SupportPressure6_1.Setting_Char},
	
	{7, column_1_Line, "����������:     "},
	{7, column_14_Line, Structset_Framemove_TelescopictMove7_2.Setting_Char},
	
	{8, column_1_Line, "����嶯��:     "},
	{8, column_14_Line, Structset_Framemove_PlateMove8_2.Setting_Char},
	
	{9, column_1_Line, "������ʱ:      s"},
	{9, column_13_Line, Structset_Framemove_ColumndownDelay9_1.Setting_Char},
	
	{10, column_1_Line, "����ʱ��:      s"},
	{10, column_13_Line, Structset_Framemove_ColumndownTime10_1.Setting_Char},
	
	{11, column_1_Line, "����ƽ����:     "},
	{11, column_14_Line, Structset_Framemove_ColumndownBalanceaction11_2.Setting_Char},
	
	{12, column_1_Line, "��������:       "},
	{12, column_14_Line, Structset_Framemove_Columndownsideplate12_2.Setting_Char},
	
	{13, column_1_Line, "�ٽ�������:     "},
	{13, column_13_Line, Structset_Framemove_DcolumnAgainFrequency13_1.Setting_Char},
	
	{14, column_1_Line, "̧�׶���:       "},
	{14, column_14_Line, Structset_Framemove_BmttomUP14_2.Setting_Char},
	
	{15, column_1_Line, "�г�Ŀ��:     mm"},
	{15, column_12_Line, Structset16_Framemove_Advancaim15_1.Setting_Char},
	
	{16, column_1_Line, "�Ƽ�ʱ��:      s"},
	{16, column_13_Line, Structset_Framemove_AdvancTime16_1.Setting_Char},
	
	{17, column_1_Line, "�Ƽܲ໤:       "},
	{17, column_14_Line, Structset_Framemove_Advancsideplate17_2.Setting_Char},
	
	{18, column_1_Line, "���Ƽ�ʱ��:    s"},
	{18, column_13_Line, Structset_Framemove_AdvancagainTime18_1.Setting_Char},
	
	{19, column_1_Line, "����ʱ��:      s"},
	{19, column_13_Line, Structset_Framemove_ColumnUPtime19_1.Setting_Char},
	
	{20, column_1_Line, "����ƽ��:       "},
	{20, column_14_Line, Structset_Framemove_ColumnUPBalanceaction20_2.Setting_Char},
	
	{21, column_1_Line, "�����໤:       "},
	{21, column_14_Line, Structset_Framemove_ColumnUPsideplatew21_2.Setting_Char},
	
	{22, column_1_Line, "�Ƽ�����:       "},
	{22, column_14_Line, Structset_Framemove_Spray22_2.Setting_Char},
	
	{23, column_1_Line, "����ʱ��:      s"},
	{23, column_13_Line, Structset_Framemove_SprayTime23_1.Setting_Char},
	
	{24, column_1_Line, "������:       "},
	{24, column_13_Line, Structset_Framemove_GroupSeparatedNum24_1.Setting_Char},
	
	{25, column_1_Line, "���鷶Χ:       "},
	{25, column_13_Line, Structset_Framemove_GroupRange25_1.Setting_Char},

};
struct _disp_context setting_GROUP_CHUTE_PUSH[15] = {
                     //0123456789abcdef/
	{0, column_1_Line,"      ����      "},
				      //0123456789abcdef/
	{1, column_1_Line, "���ﶯ��:       "},
	{1, column_13_Line, Structset_ChutePush_Forbiden1_3.Setting_Char},
	
	{2, column_1_Line, "����ʱ��:      s"},
	{2, column_13_Line, Structset_ChutePush_time2_1.Setting_Char},
	
	{3, column_1_Line, "Ŀ���г�:     mm"},
	{3, column_12_Line, Structset16_ChutePush_aimDistance3_1.Setting_Char},
	
	{4, column_1_Line, "��Сѹ��:    MPa"},
	{4, column_11_Line, Structset_ChutePush_Pressure4_1.Setting_Char},
	
	{5, column_1_Line, "����ֵ:     mm"},
	{5, column_12_Line, Structset16_ChutePush_Maximumdifference5_1.Setting_Char},
	
	{6, column_1_Line, "������:       "},
	{6, column_13_Line, Structset_ChutePush_GroupSeparatedNum6_1.Setting_Char},
	
	{7, column_1_Line, "���ﷶΧ:       "},
	{7, column_13_Line, Structset_ChutePush_GroupRange7_1.Setting_Char},
};
struct _disp_context setting_Telescopict[19] = {
                     //0123456789abcdef/
	{0, column_1_Line,"    ǰ������   "},
		              //0123456789abcdef/
	{1, column_1_Line, "ǰ������:       "},
	{1, column_13_Line, Structset_Telescopict_Forbiden1_3.Setting_Char},
	                  //0123456789abcdef/
	{2, column_1_Line, "���ʱ��:      s"},
	{2, column_13_Line, Structset_Telescopict_StretchTime2_1.Setting_Char},
	
	{3, column_1_Line, "����ʱ��:      s"},
	{3, column_13_Line, Structset_Telescopict_ShrinkTime3_1.Setting_Char},
	
	{4, column_1_Line, "����ʱ��:      s"},
	{4, column_13_Line, Structset_Telescopict_DcolumnTime4_1.Setting_Char},
	
	{5, column_1_Line, "ǰ��������:   "},
	{5, column_13_Line, Structset_Telescopict_Separatednum5_1.Setting_Char},
	
	{6, column_1_Line, "ǰ�����ﷶΧ:   "},
	{6, column_13_Line, Structset_Telescopict_Range6_1.Setting_Char},
	
	{7, column_1_Line, "���ﶯ��:       "},
	{7, column_13_Line, Structset_PLATE_Forbiden7_3.Setting_Char},
	
	{8, column_1_Line, "�커��ʱ��:    s"},
	{8, column_13_Line, Structset_PLATE_StretchTime8_1.Setting_Char},
	
	{9, column_1_Line, "�ջ���ʱ��:    s"},
	{9, column_13_Line, Structset_PLATE_ShrinkTime9_1.Setting_Char},
};
struct _disp_context setting_ChutePullMistspray[21] = {
                      //0123456789abcdef/
	{0, column_1_Line, "   ���������   "},
	                  //0123456789abcdef/
	{1, column_1_Line, "���ﶯ��:       "},
	{1, column_13_Line, Structset_ChutePull_Forbiden1_3.Setting_Char},
	
	{2, column_1_Line, "����ʱ��:      s"},
	{2, column_13_Line, Structset_ChutePull_time2_1.Setting_Char},
	
	{3, column_1_Line, "Ŀ���г�:     mm"},
	{3, column_12_Line, Structset_ChutePull_aimDistance3_1.Setting_Char},
	
	{4, column_1_Line, "��Сѹ��:    MPa"},
	{4, column_11_Line, Structset_ChutePull_Pressure4_1.Setting_Char},
	
	{5, column_1_Line, "������:       "},
	{5, column_13_Line, Structset_ChutePull_GroupSeparatedNum5_1.Setting_Char},
	
	{6, column_1_Line, "���ﷶΧ:       "},
	{6, column_13_Line, Structset_ChutePull_GroupRange6_1.Setting_Char},
	
	{7, column_1_Line, "������:       "},
	{7, column_13_Line, Structset_Mistspray_Forbiden7_3.Setting_Char},
	
	{8, column_1_Line, "������:       "},
	{8, column_13_Line, Structset_Mistspray_SeparatedNum8_1.Setting_Char},
	
	{9, column_1_Line, "����Χ:       "},
	{9, column_13_Line, Structset_Mistspray_Range9_1.Setting_Char},
	
	{10, column_1_Line, "����ʱ��:      s"},
	{10, column_13_Line, Structset_Mistspray_Time10_1.Setting_Char},
};
struct _disp_context setting_NC[4] = {
                                    //0123456789abcdef/
	{0, column_1_Line,"      ����      "},
	
	{1, column_1_Line, "                "},

	{2, column_1_Line, "                "},
	
	{3, column_1_Line, "                "},
	
};
struct _disp_context setting_AUTOMATION[7] = {
                     //0123456789abcdef/
	{0, column_1_Line,"      ����      "},
	
	{1, column_1_Line, "��������         "},

	{2, column_1_Line, "��������        "},
	
	{3, column_1_Line, "������ͷ        "},
	{3, column_13_Line, Structset_Follow_Head1_1.Setting_Char},
	
	{4, column_1_Line, "������β        "},
	{4, column_13_Line, Structset_Follow_Tail2_1.Setting_Char},
};
struct _disp_context setting_FAULT[20] = {
                     //0123456789abcdef/
	{0, column_1_Line,"     �����б�    "},
					  //0123456789abcdef/
	{1, column_1_Line, "���ڼ�ͨѶ:      "},
	{1, column_13_Line, Structset_Fault_Lcommunicate1_5.Setting_Char},
	
	{2, column_1_Line, "���ڼ�ͨѶ:      "},
	{2, column_13_Line, Structset_Fault_Rcommunicate2_5.Setting_Char},
	
	{3, column_1_Line, "����ͨѶ:        "},
	{3, column_13_Line, Structset_Fault_BUScommunicate3_5.Setting_Char},
	
	{4, column_1_Line, "�Զ���ѹ:        "},
	{4, column_13_Line, Structset_Fault_PressureStepup4_5.Setting_Char},
	
	{5, column_1_Line, "ѹ��������:      "},
	{5, column_13_Line, Structset_Fault_Sensorpressure5_5.Setting_Char},
	
	{6, column_1_Line, "�г̴�����:      "},
	{6, column_13_Line, Structset_Fault_Sensordistance6_5.Setting_Char},
	
	{7, column_1_Line, "��Ǵ�����:      "},
	{7, column_13_Line, Structset_Fault_Sensorangel7_5.Setting_Char},
	
	{8, column_1_Line, "ң�ؽ�����:      "},
	{8, column_13_Line, Structset_Fault_Receiver8_5.Setting_Char},
	
	{9, column_1_Line, "������:          "},
	{9, column_13_Line, Structset_Fault_Driver9_5.Setting_Char},
	
	{10, column_1_Line, "��ͣ�б�:       "},
};

static struct _disp_context Workcondition1[7] = {
                      //0123456789abcdef//
	
	
	{page_First_Line, column_1_Line, "      ����      "},
	{page_Second_Line, column_1_Line,"����:           "},
	{page_Second_Line, column_7_Line,_sensor_canopy_angle.sensor_view},
	{page_Third_Line, column_1_Line, "����:           "},
	{page_Third_Line, column_7_Line,_sensor_Connectingrod_angle.sensor_view},
	{page_Fouth_Line, column_1_Line, "ƽ��:           "},
	{page_Fouth_Line, column_7_Line,_sensor_Plate_angle.sensor_view},

};
static struct _disp_context Workcondition2[7] = {
                      //0123456789abcdef//
	{page_First_Line, column_1_Line, "      ����      "},
	{page_Second_Line, column_1_Line,"�г�:           "},
	{page_Second_Line, column_7_Line, _sensor_distance.sensor_view},	
	{page_Third_Line, column_1_Line, "ѹ��:           "},
	{page_Third_Line, column_7_Line, _sensor_pressure.sensor_view},
	{page_Fouth_Line, column_1_Line, "֧�ܸ߶�:     mm"},
	{page_Fouth_Line, column_10_Line, array_Altitude_view},
};


/***********************Ĭ�ϳ�ʼ����***********************************/

uint8_t variant_Interface_SETTINGcursorLimit;
uint8_t variant_interface_viewMes[9];
uint32_t variant_interface_Time;
uint8_t  Count_interface_Refresh;
/***********************�����ʾλ��***********************************/

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
///***********************_variant_Interface_disp.cursor��***********************************/
}


