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


/*****��ʾ����*****

1  ���ֱ���
2  ��1��0
3  ����1��ֹ0
4  ��DIRECTION_LEFT��DIRECTION_RIGHT
5  ����1����0
6   ��ͣ1����0
*****************/
/***************��������*****************/


extern Set_Struct  Structset_GlobalSet_MaxDeviceNum1_1;//���ܺ�
extern Set_Struct  Structset_GlobalSet_minDeviceNum2_1;//��С�ܺ�
extern Set_Struct  Structset_GlobalSet_Settingtime3_1;//����ʱ��
extern Set_Struct  Structset_GlobalSet_Increase4_4;//��ַ����
extern Set_Struct  Structset_GlobalSet_Conswitch5_2;//ת��������
extern Set_Struct  Structset_GlobalSet_Conposition6_1;//ת����λ��
extern Set_Struct  Structset_GlobalSet_PositionUpload7_2;//ú��λ���ϴ�
extern Set_Struct  Structset_GlobalSet_Adjacentcheak8_2;//�ڼܼ��
extern Set_Struct  Structset_GlobalSet_AdjacentWarring9_2;//�ڼܱ���
extern Set_Struct  Structset_GlobalSet_Adjacenttime10_1;//�ڼܼ��ʱ��
extern Set_Struct  Structset_GlobalSet_BUSworkcheak11_2;//���߼��
extern Set_Struct  Structset_GlobalSet_BUSreport12_2;//���߱���
extern Set_Struct  Structset_GlobalSet_BUScheakTime13_1;//���߼��ʱ��

/***************����*****************/
extern Set_Struct  Structset_Service_DeviceNum1_1;//�ܺ�
extern Set_Struct  Structset_Service_Adjacentcorrelation2_2;//�ڼ����
extern Set_Struct  Structset_Service_RemoteCortrol3_2;//ң�ؿ���


/*****��ʾ����*****
1  ���ֱ���
2  ��1��0
3  ����1��ֹ0
4  ��DIRECTION_LEFT��DIRECTION_RIGHT
5  ����1����0
6   ��ͣ1����0
*****************/

/***************Ĭ�ϲ���*****************/
extern Set_Struct  Structset_Default_Pressurecheak1_2;//����ѹ�����
extern Set_Struct  Structset_Default_Distancecheak2_2;//�����г̼��
extern Set_Struct  Structset_Default_Pushtime3_1;//��������ʱ��
extern Set_Struct_paralong  Structset16_Default_Pushdistance4_1;//���������г�
extern Set_Struct  Structset_Default_PressureLowlimt5_1;//��ѹ������
extern Set_Struct  Structset_Default_PressureHighlimt6_1;//��ѹ������
extern Set_Struct  Structset_Default_aimPressure7_1;//Ŀ��ѹ��
extern Set_Struct  Structset_Default_StepuppressureTime8_1;//����ʱ��
extern Set_Struct  Structset_Default_StepuppressureGap9_1;//��ѹ���
extern Set_Struct  Structset_Default_Stepuppressuretimes10_1;//��ѹ����



/***************�Ƽ�*****************/
extern Set_Struct_charlong  Structset_Framemove_Forbiden1_3;//�Ƽܶ�����ֹ
extern Set_Struct  Structset_Framemove_Alarm2_2;//�Ƽܱ��� ����
extern Set_Struct  Structset_Framemove_AlarmTime3_1;//����ʱ��
extern Set_Struct  Structset_Framemove_movePressure4_1;//�Ƽ�ѹ��     ����ѹ������ж����ѹ�����£�������������Ƽܡ�
extern Set_Struct  Structset_Framemove_TransPressure5_1;//����ѹ��      ����ѹ������������ѹ�����ϣ�����������ڼ����Զ��Ƽܡ�    �ڼ����
extern Set_Struct  Structset_Framemove_SupportPressure6_1;//֧��ѹ��   Ŀ��ѹ��
extern Set_Struct  Structset_Framemove_TelescopictMove7_2;//���������� 
extern Set_Struct  Structset_Framemove_PlateMove8_2;//����嶯��
extern Set_Struct  Structset_Framemove_ColumndownDelay9_1;//������ʱ  ����������涯������ʱ����
extern Set_Struct  Structset_Framemove_ColumndownTime10_1;//����ʱ��
extern Set_Struct  Structset_Framemove_ColumndownBalanceaction11_2;//����ʱƽ������������
extern Set_Struct  Structset_Framemove_Columndownsideplate12_2;//�������ƿ���
extern Set_Struct  Structset_Framemove_DcolumnAgainFrequency13_1;//�ٽ�������
extern Set_Struct  Structset_Framemove_BmttomUP14_2;//̧�׶�������
extern Set_Struct_paralong  Structset16_Framemove_Advancaim15_1;//�Ƽ��г�Ŀ��
extern Set_Struct  Structset_Framemove_AdvancTime16_1;//�Ƽ�ʱ��   �Ƽ��������ʱ��
extern Set_Struct  Structset_Framemove_Advancsideplate17_2;//�Ƽܲ໤����
extern Set_Struct  Structset_Framemove_AdvancagainTime18_1;//���Ƽ�ʱ��
extern Set_Struct  Structset_Framemove_ColumnUPtime19_1;//����ʱ��
extern Set_Struct  Structset_Framemove_ColumnUPBalanceaction20_2;//����ƽ�⿪��
extern Set_Struct  Structset_Framemove_ColumnUPsideplatew21_2;//�����໤����
extern Set_Struct  Structset_Framemove_Spray22_2;//�Ƽ�������
extern Set_Struct  Structset_Framemove_SprayTime23_1;//����ʱ��
extern Set_Struct  Structset_Framemove_GroupSeparatedNum24_1;//������
extern Set_Struct  Structset_Framemove_GroupRange25_1;//���鷶Χ

/****************����********************/
extern Set_Struct_charlong  Structset_ChutePush_Forbiden1_3;//���ﶯ����ֹ
extern Set_Struct  Structset_ChutePush_time2_1;//����ʱ��
extern Set_Struct_paralong  Structset16_ChutePush_aimDistance3_1;//Ŀ���г�
extern Set_Struct  Structset_ChutePush_Pressure4_1;//������Сѹ��
extern Set_Struct_paralong  Structset16_ChutePush_Maximumdifference5_1;//����ֵ
extern Set_Struct  Structset_ChutePush_GroupSeparatedNum6_1;//������
extern Set_Struct  Structset_ChutePush_GroupRange7_1;//���ﷶΧ


/****************����������********************/

extern Set_Struct_charlong  Structset_Telescopict_Forbiden1_3;
extern Set_Struct  Structset_Telescopict_StretchTime2_1;
extern Set_Struct  Structset_Telescopict_ShrinkTime3_1;
extern Set_Struct  Structset_Telescopict_DcolumnTime4_1;//����������ʱ�Ľ���ʱ��
extern Set_Struct  Structset_Telescopict_Separatednum5_1;
extern Set_Struct  Structset_Telescopict_Range6_1;
extern Set_Struct_charlong  Structset_PLATE_Forbiden7_3;
extern Set_Struct  Structset_PLATE_StretchTime8_1;
extern Set_Struct  Structset_PLATE_ShrinkTime9_1;


/****************���������********************/
extern Set_Struct_charlong  Structset_ChutePull_Forbiden1_3;//���ﶯ����ֹ
extern Set_Struct  Structset_ChutePull_time2_1;//����ʱ��
extern Set_Struct  Structset_ChutePull_aimDistance3_1;//Ŀ���г�
extern Set_Struct  Structset_ChutePull_Pressure4_1;//������Сѹ��
extern Set_Struct  Structset_ChutePull_GroupSeparatedNum5_1;//������
extern Set_Struct  Structset_ChutePull_GroupRange6_1;//���ﷶΧ
extern Set_Struct_charlong  Structset_Mistspray_Forbiden7_3;//������
extern Set_Struct  Structset_Mistspray_SeparatedNum8_1;//������
extern Set_Struct  Structset_Mistspray_Range9_1;//��Χ
extern Set_Struct  Structset_Mistspray_Time10_1;//ʱ��

/****************�����б�********************/
extern Set_Struct_charlong  Structset_Fault_Lcommunicate1_5;//���ڼ�ͨ�Ź���
extern Set_Struct_charlong  Structset_Fault_Rcommunicate2_5;//���ڼ�ͨ�Ź���
extern Set_Struct_charlong  Structset_Fault_BUScommunicate3_5;//����ͨ�Ź���
extern Set_Struct_charlong  Structset_Fault_PressureStepup4_5;//��ѹ����
extern Set_Struct_charlong  Structset_Fault_Sensorpressure5_5;//ѹ������������
extern Set_Struct_charlong  Structset_Fault_Sensordistance6_5;//�г̴���������
extern Set_Struct_charlong  Structset_Fault_Sensorangel7_5;//��Ǵ���������
extern Set_Struct_charlong  Structset_Fault_Receiver8_5;//ң��������������
extern Set_Struct_charlong  Structset_Fault_Driver9_5;//����������

/*******************����******************/
extern Set_Struct  Structset_Follow_Head1_1;//��Χ
extern Set_Struct  Structset_Follow_Tail2_1;//ʱ��


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



