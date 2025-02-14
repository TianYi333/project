#include "set.h"
#include "stm32f4xx_hal.h"
#include "bsp_w25q128.h"
#include "string.h"
#include "main.h"
#include "bsp_ParaSyn.h"
#include "time.h"
#include "main_logic.h"
#include "bsp_oled19264.h"
#include "bsp_hd7279.h"
#include "main_logic.h"
#include "interface.h"
#include <stdio.h>
#include <stdlib.h>
//#define   CURSOR_TYPE_SETTING_COMMUNICATION_RAGE_LIMT   4
//#define   CURSOR_TYPE_SETTING_GLOBAL_SET_LIMT   10
//#define   CURSOR_TYPE_SETTING_SERVICE_LIMT   5
//#define   CURSOR_TYPE_SETTING_DEFAULT_PARAMETERS_LIMT   4
//#define   CURSOR_TYPE_SETTING_GROUP_ADVANCING_LIMT   4
//#define   CURSOR_TYPE_SETTING_GROUP_CHUTE_PUSH_LIMT   4
//#define   CURSOR_TYPE_SETTING_GROUP_BALANCE_LIMT   4
//#define   CURSOR_TYPE_SETTING_GROUP_TELESCOPIC_LIMT   4
//#define   CURSOR_TYPE_SETTING_GROUP_NC_LIMT   4
//#define   CURSOR_TYPE_SETTING_AUTOMATION_LIMT   4
//#define   CURSOR_TYPE_SETTING_FAULT_LIMT   4	

uint8_t Flag_Set_Passwd = 0;
uint8_t Flag_Set_Save = 0;
uint16_t const_Set_Passwd = 8888;
uint8_t variant_Set_Qreturn;//��ʱ����������  �����˽��� ��������2023/01/15


void Set_Para_to_char(uint8_t parameter,uint8_t veiw_type,char *Set_char)
{
	/*****��ʾ����*****
1  ���ֱ���
2  ��1��0
3  ����1��ֹ0
4  ��DIRECTION_LEFT��DIRECTION_RIGHT
5  ����1����0
6   ��ͣ1����0
*****************/
	switch(veiw_type)
	{
		case 1://���ֱ���
			if((parameter%1000)/100 == 0)
			{
				*Set_char = 0x20;
				Set_char = Set_char+1;
				if((parameter%100)/10 == 0)
				{
					*Set_char = 0x20;
				}
				else if((parameter%100)/10 != 0)
				{
					*Set_char = ((parameter%100)/10)+48;
				}
				
			}
			else if((parameter%1000)/100 != 0)
			{
				*Set_char = ((parameter%1000)/100)+48;
				Set_char = Set_char+1;
				*Set_char = ((parameter%100)/10)+48;

			}
			Set_char = Set_char+1;
			*Set_char = (parameter%10)+48;
			Set_char = Set_char+1;
			*Set_char = '\0';
		break;
		
		case 2:
			if(parameter >= 1)//��GB2312��BFAA
			{
				*Set_char = 0xBF;
				*(Set_char+1) = 0xAA;
				*(Set_char+2) = '\0';
				*(Set_char+3) = '\0';
			}
			else if(parameter == 0)//��GB2312��B9D8
			{
				*Set_char = 0xB9;
				*(Set_char+1) = 0xD8;
				*(Set_char+2) = '\0';
				*(Set_char+3) = '\0';
			}
		break;
		
		case 3:
			if(parameter >= 1)//��GB2312��D4CA//��GB2312��D0ED
			{
				*Set_char = 0xD4;
				*(Set_char+1) = 0xCA;
				*(Set_char+2) = 0xD0;
				*(Set_char+3) = 0xED;
				*(Set_char+4) = '\0';
			}
			else if(parameter == 0)//��GB2312��BDFB//ֹGB2312��D6B9
			{
				*Set_char = 0xBD;
				*(Set_char+1) = 0xFB;
				*(Set_char+2) = 0xD6;
				*(Set_char+3) = 0xB9;
				*(Set_char+4) = '\0';
			}
		break;
		
		case 4:
			if(parameter == DIRECTION_LEFT)//��GB2312��D7F3
			{
				*Set_char = 0xD7;
				*(Set_char+1) = 0xF3;
				*(Set_char+2) = '\0';
				*(Set_char+3) = '\0';
			}
			else if(parameter == DIRECTION_RIGHT) //��GB2312��D3D2
			{
				*Set_char = 0xD3;
				*(Set_char+1) = 0xD2;
				*(Set_char+2) = '\0';
				*(Set_char+3) = '\0';
			}
		break;
		
		case 5:
			if(parameter >= 1)//��GB2312��D5FD//��GB2312��B3A3
			{
				*Set_char = 0xD5;
				*(Set_char+1) = 0xFD;
				*(Set_char+2) = 0xB3;
				*(Set_char+3) = 0xA3;
				*(Set_char+4) = '\0';
			}
			else if(parameter == 0)//��GB2312��B9CA//��GB2312��D5CF
			{
				*Set_char = 0xB9;
				*(Set_char+1) = 0xCA;
				*(Set_char+2) = 0xD5;
				*(Set_char+3) = 0xCF;
				*(Set_char+4) = '\0';
			}
		break;
		

		
		default:
		break;
		
	
	
	}

}
void Set_Para_to_char16(uint16_t parameter,uint8_t veiw_type,char *Set_char)
{
	 if(veiw_type  == 1)
	 {
			if((parameter%1000)/100 == 0)
			{
				*Set_char = 0x20;
				Set_char = Set_char+1;
				if((parameter%100)/10 == 0)
				{
					*Set_char = 0x20;
				}
				else if((parameter%100)/10 != 0)
				{
					*Set_char = ((parameter%100)/10)+48;
				}				
			}
			else if((parameter%1000)/100 != 0)
			{
				*Set_char = ((parameter%1000)/100)+48;
				Set_char = Set_char+1;
				*Set_char = ((parameter%100)/10)+48;
			}
			Set_char = Set_char+1;
			*Set_char = (parameter%10)+48;
			Set_char = Set_char+1;
			*Set_char = '\0';
	 
	 }
}
uint16_t Set_Prar_Change_Input(uint8_t veiw_type,uint8_t Previous_Para)
{
	uint16_t Setting_Prar = 0;//���ڷ��صı���ֵ
	uint16_t Get_Key_Value;
	uint8_t const_Set_InterfaceFlag =1;
	char Num_Str[5];//�������ֵת�ַ���
	uint8_t Setting_Bool = 0;
	variant_Set_Qreturn =1;
	Clear_Screen();
	Setting_Bool = Previous_Para;
	while(variant_Set_Qreturn==pdTRUE)
	{
		switch(veiw_type)
		{
			case 1:
				Display_String(1,0,"�������²���");
			break;
			
			case 2:
				Display_String(1,0,"��ѡ��");
				if(Setting_Bool == 0)
				{
					Display_String(3,0,"            ��");
					Display_String_Reverse(5,0,"            ��");
				}
				else if(Setting_Bool >= 1)
				{
					Display_String_Reverse(3,0,"            ��");
					Display_String(5,0,"            ��");
				}
			break;
			
			case 3:
				Display_String(1,0,"��ѡ��");
				if(Setting_Bool == 0)
				{
					Display_String(3,0,"          ����");
					Display_String_Reverse(5,0,"          ��ֹ");
				}
				else if(Setting_Bool >= 1)
				{
					Display_String_Reverse(3,0,"          ����");
					Display_String(5,0,"          ��ֹ");
				}
						
			break;
			
			case 4:
				Display_String(1,0,"��ѡ��");
				if(Setting_Bool == DIRECTION_LEFT)
				{
					Display_String_Reverse(3,0,"          ��");
					Display_String(5,0,"          ��");
				}
				else if(Setting_Bool == DIRECTION_RIGHT)
				{
					Display_String(3,0,"              ��");
					Display_String_Reverse(5,0,"              ��");
				}
			break;
			
			case 5:
				Display_String(1,0,"��ѡ��");
				if(Setting_Bool == 0)
				{
					Display_String(3,0,"          ����");
					Display_String_Reverse(5,0,"          ����");
				}
				else if(Setting_Bool >= 1)
				{
					Display_String_Reverse(3,0,"          ����");
					Display_String(5,0,"          ����");
				}
			break;
			
			default:
			break;
		}
		variant_Set_Qreturn = xQueueReceive(Key_Value_ISR_Queue,&Get_Key_Value,Structset_GlobalSet_Settingtime3_1.Setting_Para*2000);//
		if(variant_Set_Qreturn == pdFALSE)
		{
			variant_Mainlogic_KeyValue = 0;//��Ҫ���������switch�е��κ�һ��case	
			variant_Set_Qreturn = 0;
			LED_Management_Fanction(LED_NORMOL);
			return Previous_Para;
					
		}
		else
		{
				State_main_TrumpetStatus = 1;
				switch(veiw_type)
				{
					case 1:
						switch(Get_Key_Value)
						{
							case KEY13_VALUE:
								Setting_Prar = 1 + Setting_Prar * 10;
								break;
							case KEY9_VALUE:
								Setting_Prar = 2 + Setting_Prar * 10;
								break;
							case KEY5_VALUE:
								Setting_Prar = 3 + Setting_Prar * 10;
								break;
							case KEY1_VALUE:
								Setting_Prar = 4 + Setting_Prar * 10;
								break;
							case KEY14_VALUE:
								Setting_Prar = 5 + Setting_Prar * 10;
								break;
							case KEY10_VALUE:
								Setting_Prar = 6 + Setting_Prar * 10;
								break;
							case KEY6_VALUE:
								Setting_Prar = 7 + Setting_Prar * 10;
								break;
							case KEY2_VALUE:
								Setting_Prar = 8 + Setting_Prar * 10;
								break;
							case KEY15_VALUE:
								Setting_Prar = 9 + Setting_Prar * 10;
								break;
							case KEY11_VALUE:
								Setting_Prar = 0 + Setting_Prar * 10;
								break;
							case KEY12_VALUE://ֹͣ��
								variant_Set_Qreturn = 1;
								return Previous_Para;
								
							
							case KEY26_VALUE://ȷ�ϼ�
								variant_Set_Qreturn = 1;

								return Setting_Prar;
						
							
							default:
								break;
						}
						sprintf(Num_Str, "%d", Setting_Prar);//����ֵת�ַ���
						Display_String(3,64,Num_Str);
					break;
				
					case 2:
						
						switch(Get_Key_Value)
						{
								case KEY20_VALUE://��
								case KEY24_VALUE://��
									if(Setting_Bool == 0)
									{
										Setting_Bool = 1;
									}
									else if(Setting_Bool >= 1)
									{
										Setting_Bool = 0;
									}
									break;
									
								case KEY12_VALUE://ֹͣ��
									variant_Set_Qreturn = 1;
									return Previous_Para;
							
								
								case KEY26_VALUE://ȷ�ϼ�
									variant_Set_Qreturn = 1;
									return Setting_Bool;
								
							
								default:
								break;
						}
				
				
					break;
						
					case 3:
						
						
						switch(Get_Key_Value)
						{
								case KEY20_VALUE://��
								case KEY24_VALUE://��
									if(Setting_Bool == 0)
									{
										Setting_Bool = 1;
									}
									else if(Setting_Bool >= 1)
									{
										Setting_Bool = 0;
									}
									break;
								case KEY12_VALUE://ֹͣ��
									return Previous_Para;
							
								
								case KEY26_VALUE://ȷ�ϼ�
									return Setting_Bool;
						
								
								default:
									break;
						}
				
				
					break;
						
					case 4:
						
						
						
						switch(Get_Key_Value)
						{
								case KEY20_VALUE://��
								case KEY24_VALUE://��
									if(Setting_Bool == DIRECTION_LEFT)
									{
										Setting_Bool = DIRECTION_RIGHT;
									}
									else if(Setting_Bool == DIRECTION_RIGHT)
									{
										Setting_Bool = DIRECTION_LEFT;
									}
									break;
								case KEY12_VALUE://ֹͣ��
									return Previous_Para;
								
								case KEY26_VALUE://ȷ�ϼ�
									return Setting_Bool;
							
								
								default:
								break;
							
						}
				
				
					break;
						
					case 5:
						
						switch(Get_Key_Value)
						{
								case KEY20_VALUE://��
								case KEY24_VALUE://��
									if(Setting_Bool == 0)
									{
										Setting_Bool = 1;
									}
									else if(Setting_Bool >= 1)
									{
										Setting_Bool = 0;
									}
									break;
								case KEY12_VALUE://ֹͣ��
									return Previous_Para;
								
								
								case KEY26_VALUE://ȷ�ϼ�
									return Setting_Bool;
								
								
								default:
								break;
							
						}
				
				
					break;
				
				
				
				
				
				
				}
				if((Setting_Bool != Previous_Para)||(Setting_Prar != Previous_Para))
						Flag_Set_Save = 1;
		}
	}
}
void  Init_Set_Vlaue(void)
{
	Init_set_typeFuction();
	SaveParaToFlash_ReadingFun();
	Set_Init_Para();
	SaveParaToFlash_WritingFun();
	Structset_Service_RemoteCortrol3_2.Setting_Para = 0;
	 Structset_Fault_Lcommunicate1_5.Setting_Para = 1;//���ڼ�ͨ�Ź���
  Structset_Fault_Rcommunicate2_5.Setting_Para = 1;//���ڼ�ͨ�Ź���
  Structset_Fault_BUScommunicate3_5.Setting_Para = 1;//����ͨ�Ź���
  Structset_Fault_PressureStepup4_5.Setting_Para = 1;//��ѹ����
  Structset_Fault_Sensorpressure5_5.Setting_Para = 1;//ѹ������������
  Structset_Fault_Sensordistance6_5.Setting_Para = 1;//�г̴���������
  Structset_Fault_Sensorangel7_5.Setting_Para = 1;//��Ǵ���������
  Structset_Fault_Receiver8_5.Setting_Para = 1;//ң��������������
  Structset_Fault_Driver9_5.Setting_Para = 1;//����������
	Set_Init_char();
}


uint8_t PasswdInput_Set_Fun(void)
{
BaseType_t xReturn = pdTRUE;
	uint8_t Get_Key_Value;
	uint16_t Input_Passwd = 0;
	uint8_t Input_Passwd_Temp = 0;
	uint8_t Passwordcount = 0;
	Clear_Screen();
	
	Display_String(1,0,"   ����������   ");
	
	for(Passwordcount=0;Passwordcount<4;Passwordcount++)
	{
		xReturn = xQueueReceive(Key_Value_ISR_Queue,&Get_Key_Value,Structset_GlobalSet_Settingtime3_1.Setting_Para*1000*2);
		if(xReturn == pdFALSE)
		{
			return 0;
		}
		else
		{
			State_main_TrumpetStatus = 1;
		}
		
		switch(Get_Key_Value)
		{
			case KEY13_VALUE:
				Input_Passwd_Temp = 1;				
				break;
			case KEY9_VALUE:
				Input_Passwd_Temp = 2;
				break;
			case KEY5_VALUE:
				Input_Passwd_Temp = 3;
				break;
			case KEY1_VALUE:
				Input_Passwd_Temp = 4;
				break;
			case KEY14_VALUE:
				Input_Passwd_Temp = 5;
				break;
			case KEY10_VALUE:
				Input_Passwd_Temp = 6;
				break;
			case KEY6_VALUE:
				Input_Passwd_Temp = 7;
				break;
			case KEY2_VALUE:
				Input_Passwd_Temp = 8;
				break;
			case KEY15_VALUE:
				Input_Passwd_Temp = 9;
				break;
			case KEY11_VALUE:
				Input_Passwd_Temp = 0;
				break;
		}
		switch(Passwordcount)
		{
			case 0:
				Display_String(5,48,"*");
				break;
			case 1:
				Display_String(5,48,"**");
				break;
			case 2:
				Display_String(5,48,"***");
				break;
			case 3:
				Display_String(5,48,"****");
				break;
		}
		
		Input_Passwd = Input_Passwd_Temp + Input_Passwd*10;
	}
	if(Input_Passwd == const_Set_Passwd)
	{
		return 1;
	}
	else
	{
		Display_String(1,0,"    �������    ");
		vTaskDelay(500);
		return 0;
	}






}
/*****��ʾ����*****

1  ���ֱ���
2  ��1��0
3  ����1��ֹ0
4  ��DIRECTION_LEFT��DIRECTION_RIGHT
5  ����1����0
6   ��ͣ1����0
*****************/
/***************��������*****************/


Set_Struct  Structset_GlobalSet_MaxDeviceNum1_1;//���ܺ�
Set_Struct  Structset_GlobalSet_minDeviceNum2_1;//��С�ܺ�
Set_Struct  Structset_GlobalSet_Settingtime3_1;//����ʱ��
Set_Struct  Structset_GlobalSet_Increase4_4;//��ַ����
Set_Struct  Structset_GlobalSet_Conswitch5_2;//ת��������
Set_Struct  Structset_GlobalSet_Conposition6_1;//ת����λ��
Set_Struct  Structset_GlobalSet_PositionUpload7_2;//ú��λ���ϴ�
Set_Struct  Structset_GlobalSet_Adjacentcheak8_2;//�ڼܼ��
Set_Struct  Structset_GlobalSet_AdjacentWarring9_2;//�ڼܱ���
Set_Struct  Structset_GlobalSet_Adjacenttime10_1;//�ڼܼ��ʱ��
Set_Struct  Structset_GlobalSet_BUSworkcheak11_2;//���߼��
Set_Struct  Structset_GlobalSet_BUSreport12_2;//���߱���
Set_Struct  Structset_GlobalSet_BUScheakTime13_1;//���߼��ʱ��

/***************����*****************/
Set_Struct  Structset_Service_DeviceNum1_1;//�ܺ�
Set_Struct  Structset_Service_Adjacentcorrelation2_2;//�ڼ����
Set_Struct  Structset_Service_RemoteCortrol3_2;//ң�ؿ���


/*****��ʾ����*****
1  ���ֱ���
2  ��1��0
3  ����1��ֹ0
4  ��DIRECTION_LEFT��DIRECTION_RIGHT
5  ����1����0
6   ��ͣ1����0
*****************/

/***************Ĭ�ϲ���*****************/
Set_Struct  Structset_Default_Pressurecheak1_2;//����ѹ�����
Set_Struct  Structset_Default_Distancecheak2_2;//�����г̼��
Set_Struct  Structset_Default_Pushtime3_1;//��������ʱ��
Set_Struct_paralong  Structset16_Default_Pushdistance4_1;//���������г�                                         //16λһ
Set_Struct  Structset_Default_PressureLowlimt5_1;//��ѹ������  
Set_Struct  Structset_Default_PressureHighlimt6_1;//��ѹ������
Set_Struct  Structset_Default_aimPressure7_1;//Ŀ��ѹ��			
Set_Struct  Structset_Default_StepuppressureTime8_1;//����ʱ��
Set_Struct  Structset_Default_StepuppressureGap9_1;//��ѹ���
Set_Struct  Structset_Default_Stepuppressuretimes10_1;//��ѹ����



/***************�Ƽ�*****************/
Set_Struct_charlong  Structset_Framemove_Forbiden1_3;//�Ƽܶ�����ֹ
Set_Struct  Structset_Framemove_Alarm2_2;//�Ƽܱ��� ����
Set_Struct  Structset_Framemove_AlarmTime3_1;//����ʱ��
Set_Struct  Structset_Framemove_movePressure4_1;//�Ƽ�ѹ��     ����ѹ������ж����ѹ�����£�������������Ƽܡ�
Set_Struct  Structset_Framemove_TransPressure5_1;//����ѹ��      ����ѹ������������ѹ�����ϣ�����������ڼ����Զ��Ƽܡ�    �ڼ����
Set_Struct  Structset_Framemove_SupportPressure6_1;//֧��ѹ��   Ŀ��ѹ��
Set_Struct  Structset_Framemove_TelescopictMove7_2;//���������� 
Set_Struct  Structset_Framemove_PlateMove8_2;//����嶯��
Set_Struct  Structset_Framemove_ColumndownDelay9_1;//������ʱ  ����������涯������ʱ����
Set_Struct  Structset_Framemove_ColumndownTime10_1;//����ʱ��
Set_Struct  Structset_Framemove_ColumndownBalanceaction11_2;//����ʱƽ������������
Set_Struct  Structset_Framemove_Columndownsideplate12_2;//�������ƿ���
Set_Struct  Structset_Framemove_DcolumnAgainFrequency13_1;//�ٽ�������
Set_Struct  Structset_Framemove_BmttomUP14_2;//̧�׶�������
Set_Struct_paralong  Structset16_Framemove_Advancaim15_1;//�Ƽ��г�Ŀ��                                         //16λ��
Set_Struct  Structset_Framemove_AdvancTime16_1;//�Ƽ�ʱ��   �Ƽ��������ʱ��
Set_Struct  Structset_Framemove_Advancsideplate17_2;//�Ƽܲ໤����
Set_Struct  Structset_Framemove_AdvancagainTime18_1;//���Ƽ�ʱ��
Set_Struct  Structset_Framemove_ColumnUPtime19_1;//����ʱ��
Set_Struct  Structset_Framemove_ColumnUPBalanceaction20_2;//����ƽ�⿪��
Set_Struct  Structset_Framemove_ColumnUPsideplatew21_2;//�����໤����
Set_Struct  Structset_Framemove_Spray22_2;//�Ƽ�������
Set_Struct  Structset_Framemove_SprayTime23_1;//����ʱ��
Set_Struct  Structset_Framemove_GroupSeparatedNum24_1;//������
Set_Struct  Structset_Framemove_GroupRange25_1;//���鷶Χ

/****************����********************/
Set_Struct_charlong  Structset_ChutePush_Forbiden1_3;//���ﶯ����ֹ
Set_Struct  Structset_ChutePush_time2_1;//����ʱ��
Set_Struct_paralong  Structset16_ChutePush_aimDistance3_1;//Ŀ���г�                                         //16λ��
Set_Struct  Structset_ChutePush_Pressure4_1;//������Сѹ��
Set_Struct_paralong  Structset16_ChutePush_Maximumdifference5_1;//����ֵ                                   //16λ��
Set_Struct  Structset_ChutePush_GroupSeparatedNum6_1;//������
Set_Struct  Structset_ChutePush_GroupRange7_1;//���ﷶΧ


/****************����������********************/

Set_Struct_charlong  Structset_Telescopict_Forbiden1_3;//����������
Set_Struct  Structset_Telescopict_StretchTime2_1;
Set_Struct  Structset_Telescopict_ShrinkTime3_1;
Set_Struct  Structset_Telescopict_DcolumnTime4_1;//����������ʱ�Ľ���ʱ��
Set_Struct  Structset_Telescopict_Separatednum5_1;
Set_Struct  Structset_Telescopict_Range6_1;
Set_Struct_charlong  Structset_PLATE_Forbiden7_3;
Set_Struct  Structset_PLATE_StretchTime8_1;
Set_Struct  Structset_PLATE_ShrinkTime9_1;


/****************���������********************/
Set_Struct_charlong  Structset_ChutePull_Forbiden1_3;//���ﶯ����ֹ
Set_Struct  Structset_ChutePull_time2_1;//����ʱ��
Set_Struct  Structset_ChutePull_aimDistance3_1;//Ŀ���г�
Set_Struct  Structset_ChutePull_Pressure4_1;//������Сѹ��
Set_Struct  Structset_ChutePull_GroupSeparatedNum5_1;//������
Set_Struct  Structset_ChutePull_GroupRange6_1;//���ﷶΧ
Set_Struct_charlong  Structset_Mistspray_Forbiden7_3;//������
Set_Struct  Structset_Mistspray_SeparatedNum8_1;//������
Set_Struct  Structset_Mistspray_Range9_1;//��Χ
Set_Struct  Structset_Mistspray_Time10_1;//ʱ��

/****************�����б�********************/
Set_Struct_charlong  Structset_Fault_Lcommunicate1_5;//���ڼ�ͨ�Ź���
Set_Struct_charlong  Structset_Fault_Rcommunicate2_5;//���ڼ�ͨ�Ź���
Set_Struct_charlong  Structset_Fault_BUScommunicate3_5;//����ͨ�Ź���
Set_Struct_charlong  Structset_Fault_PressureStepup4_5;//��ѹ����
Set_Struct_charlong  Structset_Fault_Sensorpressure5_5;//ѹ������������
Set_Struct_charlong  Structset_Fault_Sensordistance6_5;//�г̴���������
Set_Struct_charlong  Structset_Fault_Sensorangel7_5;//��Ǵ���������
Set_Struct_charlong  Structset_Fault_Receiver8_5;//ң��������������
Set_Struct_charlong  Structset_Fault_Driver9_5;//����������

/*******************����******************/
Set_Struct  Structset_Follow_Head1_1;//��Χ
Set_Struct  Structset_Follow_Tail2_1;//ʱ��



void Set_Init_Para(void)
{
/***************��������*****************/
  Structset_GlobalSet_MaxDeviceNum1_1.Setting_Para = 101;//���ܺ�
 Structset_GlobalSet_minDeviceNum2_1.Setting_Para = 1;//��С�ܺ�
 Structset_GlobalSet_Settingtime3_1.Setting_Para = 8;//����ʱ��
Structset_GlobalSet_Increase4_4.Setting_Para = 0x02;//��ַ����
Structset_GlobalSet_Conswitch5_2.Setting_Para = 1;//ת��������
 Structset_GlobalSet_Conposition6_1.Setting_Para = 0;//ת����λ��
 Structset_GlobalSet_PositionUpload7_2.Setting_Para = 1;//ú��λ���ϴ�
 Structset_GlobalSet_Adjacentcheak8_2.Setting_Para = 1;//�ڼܼ��
  Structset_GlobalSet_AdjacentWarring9_2.Setting_Para = 1;//�ڼܱ���
  Structset_GlobalSet_Adjacenttime10_1.Setting_Para = 60;//�ڼܼ��ʱ��
  Structset_GlobalSet_BUSworkcheak11_2.Setting_Para = 1;//���߼��
 Structset_GlobalSet_BUSreport12_2.Setting_Para = 1;//���߱���
Structset_GlobalSet_BUScheakTime13_1.Setting_Para = 10;//���߼��ʱ��
/***************����*****************/

//	Structset_Service_DeviceNum1_1.Setting_Para = 10;

 Structset_Service_Adjacentcorrelation2_2.Setting_Para = 0;//�ڼ����
 Structset_Service_RemoteCortrol3_2.Setting_Para = 0;//ң�ؿ���




/*****��ʾ����*****
1  ���ֱ���
2  ��1��0
3  ����1��ֹ0
4  ��DIRECTION_LEFT��DIRECTION_RIGHT
5  ����1����0
6   ��ͣ1����0
*****************/

/***************Ĭ�ϲ���*****************/
 Structset_Default_Pressurecheak1_2.Setting_Para = 1;//����ѹ�����
 Structset_Default_Distancecheak2_2.Setting_Para = 1;//�����г̼��
  Structset_Default_Pushtime3_1.Setting_Para = 10;//��������ʱ��
  Structset16_Default_Pushdistance4_1.Setting_Para = 100;//���������г�
  Structset_Default_PressureLowlimt5_1.Setting_Para = 0;//��ѹ������
  Structset_Default_PressureHighlimt6_1.Setting_Para = 0;//��ѹ������
  Structset_Default_aimPressure7_1.Setting_Para = 10;//Ŀ��ѹ��
  Structset_Default_StepuppressureTime8_1.Setting_Para = 2;//����ʱ��
  Structset_Default_StepuppressureGap9_1.Setting_Para = 10;//��ѹ���
  Structset_Default_Stepuppressuretimes10_1.Setting_Para = 2;//��ѹ����



/***************�Ƽ�*****************/
  Structset_Framemove_Forbiden1_3.Setting_Para = 1;//�Ƽܶ�����ֹ
 Structset_Framemove_Alarm2_2.Setting_Para = 1;//�Ƽܱ��� ����
  Structset_Framemove_AlarmTime3_1.Setting_Para = 50;//����ʱ��
 Structset_Framemove_movePressure4_1.Setting_Para = 0;//�Ƽ�ѹ��     ����ѹ������ж����ѹ�����£�������������Ƽܡ�
  Structset_Framemove_TransPressure5_1.Setting_Para = 0;//����ѹ��      ����ѹ������������ѹ�����ϣ�����������ڼ����Զ��Ƽܡ�    �ڼ����
  Structset_Framemove_SupportPressure6_1.Setting_Para = 0;//֧��ѹ��   Ŀ��ѹ��
  Structset_Framemove_TelescopictMove7_2.Setting_Para = 1;//���������� 
  Structset_Framemove_PlateMove8_2.Setting_Para = 1;//����嶯��
  Structset_Framemove_ColumndownDelay9_1.Setting_Para = 1;//������ʱ  ����������涯������ʱ����
  Structset_Framemove_ColumndownTime10_1.Setting_Para = 2;//����ʱ��
  Structset_Framemove_ColumndownBalanceaction11_2.Setting_Para = 0;//����ʱƽ������������
  Structset_Framemove_Columndownsideplate12_2.Setting_Para = 0;//�������ƿ���
  Structset_Framemove_DcolumnAgainFrequency13_1.Setting_Para = 0;//�ٽ�������
  Structset_Framemove_BmttomUP14_2.Setting_Para = 1;//̧�׶�������
  Structset16_Framemove_Advancaim15_1.Setting_Para = 0;//�Ƽ��г�Ŀ��
  Structset_Framemove_AdvancTime16_1.Setting_Para = 10;//�Ƽ�ʱ��   �Ƽ��������ʱ��
  Structset_Framemove_Advancsideplate17_2.Setting_Para = 1;//�Ƽܲ໤����
  Structset_Framemove_AdvancagainTime18_1.Setting_Para = 5;//���Ƽ�ʱ��
 Structset_Framemove_ColumnUPtime19_1.Setting_Para = 3;//����ʱ��
  Structset_Framemove_ColumnUPBalanceaction20_2.Setting_Para = 0;//����ƽ�⿪��
  Structset_Framemove_ColumnUPsideplatew21_2.Setting_Para = 0;//�����໤����
  Structset_Framemove_Spray22_2.Setting_Para = 1;//�Ƽ�������
  Structset_Framemove_SprayTime23_1.Setting_Para = 5;//����ʱ��
//  Structset_Framemove_GroupSeparatedNum24_1.Setting_Para = 0;//������
//  Structset_Framemove_GroupRange25_1.Setting_Para = 5;//���鷶Χ

/****************����********************/
  Structset_ChutePush_Forbiden1_3.Setting_Para = 1;//���ﶯ����ֹ
  Structset_ChutePush_time2_1.Setting_Para = 10;//����ʱ��
  Structset16_ChutePush_aimDistance3_1.Setting_Para = 100;//Ŀ���г�
  Structset_ChutePush_Pressure4_1.Setting_Para = 0;//������Сѹ��
  Structset16_ChutePush_Maximumdifference5_1.Setting_Para = 0;//����ֵ
//  Structset_ChutePush_GroupSeparatedNum6_1.Setting_Para = 0;//������
//  Structset_ChutePush_GroupRange7_1.Setting_Para = 5;//���ﷶΧ


/****************����������********************/

  Structset_Telescopict_Forbiden1_3.Setting_Para = 1;
  Structset_Telescopict_StretchTime2_1.Setting_Para = 3;
//  Structset_Telescopict_ShrinkTime3_1.Setting_Para = 3;
//  Structset_Telescopict_DcolumnTime4_1.Setting_Para = 2;//����������ʱ�Ľ���ʱ��
//  Structset_Telescopict_Separatednum5_1.Setting_Para = 0;
//  Structset_Telescopict_Range6_1.Setting_Para = 5;
  Structset_PLATE_Forbiden7_3.Setting_Para = 1;
//  Structset_PLATE_StretchTime8_1.Setting_Para = 3;
//  Structset_PLATE_ShrinkTime9_1.Setting_Para = 3;


/****************���������********************/
  Structset_ChutePull_Forbiden1_3.Setting_Para = 1;//���ﶯ����ֹ
  Structset_ChutePull_time2_1.Setting_Para = 10;//����ʱ��
  Structset_ChutePull_aimDistance3_1.Setting_Para = 100;//Ŀ���г�
  Structset_ChutePull_Pressure4_1.Setting_Para = 0;//������Сѹ��
//  Structset_ChutePull_GroupSeparatedNum5_1.Setting_Para = 0;//������
//  Structset_ChutePull_GroupRange6_1.Setting_Para = 5;//���ﷶΧ
//  Structset_Mistspray_Forbiden7_3.Setting_Para = 1;//������
//  Structset_Mistspray_SeparatedNum8_1.Setting_Para = 0;//������
//  Structset_Mistspray_Range9_1.Setting_Para = 5;//��Χ
//  Structset_Mistspray_Time10_1.Setting_Para = 5;//ʱ��

/****************�����б�********************/
  Structset_Fault_Lcommunicate1_5.Setting_Para = 1;//���ڼ�ͨ�Ź���
  Structset_Fault_Rcommunicate2_5.Setting_Para = 1;//���ڼ�ͨ�Ź���
  Structset_Fault_BUScommunicate3_5.Setting_Para = 1;//����ͨ�Ź���
  Structset_Fault_PressureStepup4_5.Setting_Para = 1;//��ѹ����
  Structset_Fault_Sensorpressure5_5.Setting_Para = 1;//ѹ������������
  Structset_Fault_Sensordistance6_5.Setting_Para = 1;//�г̴���������
  Structset_Fault_Sensorangel7_5.Setting_Para = 1;//��Ǵ���������
  Structset_Fault_Receiver8_5.Setting_Para = 1;//ң��������������
  Structset_Fault_Driver9_5.Setting_Para = 1;//����������

	
	
	  Structset_Follow_Head1_1.Setting_Para = 1;//������ͷ
  Structset_Follow_Tail2_1.Setting_Para = 8;//������β
	
	
	
	
}

void Init_set_typeFuction(void)
{
	/***************��������*****************/
  Structset_GlobalSet_MaxDeviceNum1_1.veiw_type = 1;//���ܺ�
 Structset_GlobalSet_minDeviceNum2_1.veiw_type = 1;//��С�ܺ�
 Structset_GlobalSet_Settingtime3_1.veiw_type = 1;//����ʱ��
Structset_GlobalSet_Increase4_4.veiw_type = 4;//��ַ����
Structset_GlobalSet_Conswitch5_2.veiw_type = 2;//ת��������
 Structset_GlobalSet_Conposition6_1.veiw_type = 1;//ת����λ��
 Structset_GlobalSet_PositionUpload7_2.veiw_type = 2;//ú��λ���ϴ�
 Structset_GlobalSet_Adjacentcheak8_2.veiw_type = 2;//�ڼܼ��
  Structset_GlobalSet_AdjacentWarring9_2.veiw_type = 2;//�ڼܱ���
  Structset_GlobalSet_Adjacenttime10_1.veiw_type = 1;//�ڼܼ��ʱ��
  Structset_GlobalSet_BUSworkcheak11_2.veiw_type = 2;//���߼��
 Structset_GlobalSet_BUSreport12_2.veiw_type = 2;//���߱���
Structset_GlobalSet_BUScheakTime13_1.veiw_type = 1;//���߼��ʱ��
/***************����*****************/

  Structset_Service_DeviceNum1_1.veiw_type = 1;//�ܺ�
 Structset_Service_Adjacentcorrelation2_2.veiw_type = 2;//�ڼ����
 Structset_Service_RemoteCortrol3_2.veiw_type = 2;//ң�ؿ���




/*****��ʾ����*****
1  ���ֱ���
2  ��1��0
3  ����1��ֹ0
4  ��DIRECTION_LEFT��DIRECTION_RIGHT
5  ����1����0
6   ��ͣ1����0
*****************/

/***************Ĭ�ϲ���*****************/
 Structset_Default_Pressurecheak1_2.veiw_type = 2;//����ѹ�����
 Structset_Default_Distancecheak2_2.veiw_type = 2;//�����г̼��
  Structset_Default_Pushtime3_1.veiw_type = 1;//��������ʱ��
  Structset16_Default_Pushdistance4_1.veiw_type = 1;//���������г�
  Structset_Default_PressureLowlimt5_1.veiw_type = 1;//��ѹ������
  Structset_Default_PressureHighlimt6_1.veiw_type = 1;//��ѹ������
  Structset_Default_aimPressure7_1.veiw_type = 1;//Ŀ��ѹ��
  Structset_Default_StepuppressureTime8_1.veiw_type = 1;//����ʱ��
  Structset_Default_StepuppressureGap9_1.veiw_type = 1;//��ѹ���
  Structset_Default_Stepuppressuretimes10_1.veiw_type = 1;//��ѹ����



/***************�Ƽ�*****************/
  Structset_Framemove_Forbiden1_3.veiw_type = 3;//�Ƽܶ�����ֹ
 Structset_Framemove_Alarm2_2.veiw_type = 2;//�Ƽܱ��� ����
  Structset_Framemove_AlarmTime3_1.veiw_type = 1;//����ʱ��
 Structset_Framemove_movePressure4_1.veiw_type = 1;//�Ƽ�ѹ��     ����ѹ������ж����ѹ�����£�������������Ƽܡ�
  Structset_Framemove_TransPressure5_1.veiw_type = 1;//����ѹ��      ����ѹ������������ѹ�����ϣ�����������ڼ����Զ��Ƽܡ�    �ڼ����
  Structset_Framemove_SupportPressure6_1.veiw_type = 1;//֧��ѹ��   Ŀ��ѹ��
  Structset_Framemove_TelescopictMove7_2.veiw_type = 2;//���������� 
  Structset_Framemove_PlateMove8_2.veiw_type = 2;//����嶯��
  Structset_Framemove_ColumndownDelay9_1.veiw_type = 1;//������ʱ  ����������涯������ʱ����
  Structset_Framemove_ColumndownTime10_1.veiw_type = 1;//����ʱ��
  Structset_Framemove_ColumndownBalanceaction11_2.veiw_type = 2;//����ʱƽ������������
  Structset_Framemove_Columndownsideplate12_2.veiw_type = 2;//�������ƿ���
  Structset_Framemove_DcolumnAgainFrequency13_1.veiw_type = 1;//�ٽ�������
  Structset_Framemove_BmttomUP14_2.veiw_type = 2;//̧�׶�������
  Structset16_Framemove_Advancaim15_1.veiw_type = 1;//�Ƽ��г�Ŀ��
  Structset_Framemove_AdvancTime16_1.veiw_type = 1;//�Ƽ�ʱ��   �Ƽ��������ʱ��
  Structset_Framemove_Advancsideplate17_2.veiw_type = 2;//�Ƽܲ໤����
  Structset_Framemove_AdvancagainTime18_1.veiw_type = 1;//���Ƽ�ʱ��
 Structset_Framemove_ColumnUPtime19_1.veiw_type = 1;//����ʱ��
  Structset_Framemove_ColumnUPBalanceaction20_2.veiw_type = 2;//����ƽ�⿪��
  Structset_Framemove_ColumnUPsideplatew21_2.veiw_type = 2;//�����໤����
  Structset_Framemove_Spray22_2.veiw_type = 2;//�Ƽ�������
  Structset_Framemove_SprayTime23_1.veiw_type = 1;//����ʱ��
  Structset_Framemove_GroupSeparatedNum24_1.veiw_type = 1;//������
  Structset_Framemove_GroupRange25_1.veiw_type = 1;//���鷶Χ

/****************����********************/
  Structset_ChutePush_Forbiden1_3.veiw_type = 3;//���ﶯ����ֹ
  Structset_ChutePush_time2_1.veiw_type = 1;//����ʱ��
  Structset16_ChutePush_aimDistance3_1.veiw_type = 1;//Ŀ���г�
  Structset_ChutePush_Pressure4_1.veiw_type = 1;//������Сѹ��
  Structset16_ChutePush_Maximumdifference5_1.veiw_type = 1;//����ֵ
  Structset_ChutePush_GroupSeparatedNum6_1.veiw_type = 1;//������
  Structset_ChutePush_GroupRange7_1.veiw_type = 1;//���ﷶΧ


/****************����������********************/

  Structset_Telescopict_Forbiden1_3.veiw_type = 3;
  Structset_Telescopict_StretchTime2_1.veiw_type = 1;
  Structset_Telescopict_ShrinkTime3_1.veiw_type = 1;
  Structset_Telescopict_DcolumnTime4_1.veiw_type = 1;//����������ʱ�Ľ���ʱ��
  Structset_Telescopict_Separatednum5_1.veiw_type = 1;
  Structset_Telescopict_Range6_1.veiw_type = 1;
  Structset_PLATE_Forbiden7_3.veiw_type = 3;
  Structset_PLATE_StretchTime8_1.veiw_type = 1;
  Structset_PLATE_ShrinkTime9_1.veiw_type = 1;


/****************���������********************/
  Structset_ChutePull_Forbiden1_3.veiw_type = 3;//���ﶯ����ֹ
  Structset_ChutePull_time2_1.veiw_type = 1;//����ʱ��
  Structset_ChutePull_aimDistance3_1.veiw_type = 1;//Ŀ���г�
  Structset_ChutePull_Pressure4_1.veiw_type = 1;//������Сѹ��
  Structset_ChutePull_GroupSeparatedNum5_1.veiw_type = 1;//������
  Structset_ChutePull_GroupRange6_1.veiw_type = 1;//���ﷶΧ
  Structset_Mistspray_Forbiden7_3.veiw_type = 3;//������
  Structset_Mistspray_SeparatedNum8_1.veiw_type = 1;//������
  Structset_Mistspray_Range9_1.veiw_type = 1;//��Χ
  Structset_Mistspray_Time10_1.veiw_type = 1;//ʱ��

/****************�����б�********************/
  Structset_Fault_Lcommunicate1_5.veiw_type = 5;//���ڼ�ͨ�Ź���
  Structset_Fault_Rcommunicate2_5.veiw_type = 5;//���ڼ�ͨ�Ź���
  Structset_Fault_BUScommunicate3_5.veiw_type = 5;//����ͨ�Ź���
  Structset_Fault_PressureStepup4_5.veiw_type = 5;//��ѹ����
  Structset_Fault_Sensorpressure5_5.veiw_type = 5;//ѹ������������
  Structset_Fault_Sensordistance6_5.veiw_type = 5;//�г̴���������
  Structset_Fault_Sensorangel7_5.veiw_type = 5;//��Ǵ���������
  Structset_Fault_Receiver8_5.veiw_type = 5;//ң��������������
  Structset_Fault_Driver9_5.veiw_type = 5;//����������

  Structset_Follow_Head1_1.veiw_type = 1;//������ͷ
  Structset_Follow_Tail2_1.veiw_type = 1;//������β
  
}



///***************��������*****************/


//Set_Struct  Structset_GlobalSet_MaxDeviceNum1_1;//���ܺ�
//Set_Struct  Structset_GlobalSet_minDeviceNum2_1;//��С�ܺ�
//Set_Struct  Structset_GlobalSet_Settingtime3_1;//����ʱ��
//Set_Struct  Structset_GlobalSet_Increase4_4;//��ַ����
//Set_Struct  Structset_GlobalSet_Conswitch5_2;//ת��������
//Set_Struct  Structset_GlobalSet_Conposition6_1;//ת����λ��
//Set_Struct  Structset_GlobalSet_PositionUpload7_2;//ú��λ���ϴ�
//Set_Struct  Structset_GlobalSet_Adjacentcheak8_2;//�ڼܼ��
//Set_Struct  Structset_GlobalSet_AdjacentWarring9_2;//�ڼܱ���
//Set_Struct  Structset_GlobalSet_Adjacenttime10_1;//�ڼܼ��ʱ��
//Set_Struct  Structset_GlobalSet_BUSworkcheak11_2;//���߼��
//Set_Struct  Structset_GlobalSet_BUSreport12_2;//���߱���
//Set_Struct  Structset_GlobalSet_BUScheakTime13_1;//���߼��ʱ��

///***************����*****************/
//Set_Struct  Structset_Service_DeviceNum1_1;//�ܺ�
//Set_Struct  Structset_Service_Adjacentcorrelation2_2;//�ڼ����
//Set_Struct  Structset_Service_RemoteCortrol3_2;//ң�ؿ���

void Set_Init_char(void)
{
	Set_Para_to_char(Structset_GlobalSet_MaxDeviceNum1_1.Setting_Para,Structset_GlobalSet_MaxDeviceNum1_1.veiw_type,Structset_GlobalSet_MaxDeviceNum1_1.Setting_Char);
	Set_Para_to_char(Structset_GlobalSet_minDeviceNum2_1.Setting_Para,Structset_GlobalSet_minDeviceNum2_1.veiw_type,Structset_GlobalSet_minDeviceNum2_1.Setting_Char);
	Set_Para_to_char(Structset_GlobalSet_Settingtime3_1.Setting_Para,Structset_GlobalSet_Settingtime3_1.veiw_type,Structset_GlobalSet_Settingtime3_1.Setting_Char);
	Set_Para_to_char(Structset_GlobalSet_Increase4_4.Setting_Para,Structset_GlobalSet_Increase4_4.veiw_type,Structset_GlobalSet_Increase4_4.Setting_Char);
	Set_Para_to_char(Structset_GlobalSet_Conswitch5_2.Setting_Para,Structset_GlobalSet_Conswitch5_2.veiw_type,Structset_GlobalSet_Conswitch5_2.Setting_Char);
	Set_Para_to_char(Structset_GlobalSet_Conposition6_1.Setting_Para,Structset_GlobalSet_Conposition6_1.veiw_type,Structset_GlobalSet_Conposition6_1.Setting_Char);
	Set_Para_to_char(Structset_GlobalSet_PositionUpload7_2.Setting_Para,Structset_GlobalSet_PositionUpload7_2.veiw_type,Structset_GlobalSet_PositionUpload7_2.Setting_Char);
	Set_Para_to_char(Structset_GlobalSet_Adjacentcheak8_2.Setting_Para,Structset_GlobalSet_Adjacentcheak8_2.veiw_type,Structset_GlobalSet_Adjacentcheak8_2.Setting_Char);
	Set_Para_to_char(Structset_GlobalSet_AdjacentWarring9_2.Setting_Para,Structset_GlobalSet_AdjacentWarring9_2.veiw_type,Structset_GlobalSet_AdjacentWarring9_2.Setting_Char);
	Set_Para_to_char(Structset_GlobalSet_Adjacenttime10_1.Setting_Para,Structset_GlobalSet_Adjacenttime10_1.veiw_type,Structset_GlobalSet_Adjacenttime10_1.Setting_Char);
	Set_Para_to_char(Structset_GlobalSet_BUSworkcheak11_2.Setting_Para,Structset_GlobalSet_BUSworkcheak11_2.veiw_type,Structset_GlobalSet_BUSworkcheak11_2.Setting_Char);
	Set_Para_to_char(Structset_GlobalSet_BUSreport12_2.Setting_Para,Structset_GlobalSet_BUSreport12_2.veiw_type,Structset_GlobalSet_BUSreport12_2.Setting_Char);	
	Set_Para_to_char(Structset_GlobalSet_BUScheakTime13_1.Setting_Para,Structset_GlobalSet_BUScheakTime13_1.veiw_type,Structset_GlobalSet_BUScheakTime13_1.Setting_Char);
	
	Set_Para_to_char(Structset_Service_DeviceNum1_1.Setting_Para,Structset_Service_DeviceNum1_1.veiw_type,Structset_Service_DeviceNum1_1.Setting_Char);
	Set_Para_to_char(Structset_Service_Adjacentcorrelation2_2.Setting_Para,Structset_Service_Adjacentcorrelation2_2.veiw_type,Structset_Service_Adjacentcorrelation2_2.Setting_Char);
	Set_Para_to_char(Structset_Service_RemoteCortrol3_2.Setting_Para,Structset_Service_RemoteCortrol3_2.veiw_type,Structset_Service_RemoteCortrol3_2.Setting_Char);
	
	Set_Para_to_char(Structset_Default_Pressurecheak1_2.Setting_Para,Structset_Default_Pressurecheak1_2.veiw_type,Structset_Default_Pressurecheak1_2.Setting_Char);
	Set_Para_to_char(Structset_Default_Distancecheak2_2.Setting_Para,Structset_Default_Distancecheak2_2.veiw_type,Structset_Default_Distancecheak2_2.Setting_Char);
	Set_Para_to_char(Structset_Default_Pushtime3_1.Setting_Para,Structset_Default_Pushtime3_1.veiw_type,Structset_Default_Pushtime3_1.Setting_Char);
	Set_Para_to_char16(Structset16_Default_Pushdistance4_1.Setting_Para,Structset16_Default_Pushdistance4_1.veiw_type,Structset16_Default_Pushdistance4_1.Setting_Char);
	Set_Para_to_char(Structset_Default_PressureLowlimt5_1.Setting_Para,Structset_Default_PressureLowlimt5_1.veiw_type,Structset_Default_PressureLowlimt5_1.Setting_Char);
	Set_Para_to_char(Structset_Default_PressureHighlimt6_1.Setting_Para,Structset_Default_PressureHighlimt6_1.veiw_type,Structset_Default_PressureHighlimt6_1.Setting_Char);
	Set_Para_to_char(Structset_Default_aimPressure7_1.Setting_Para,Structset_Default_aimPressure7_1.veiw_type,Structset_Default_aimPressure7_1.Setting_Char);
	Set_Para_to_char(Structset_Default_StepuppressureTime8_1.Setting_Para,Structset_Default_StepuppressureTime8_1.veiw_type,Structset_Default_StepuppressureTime8_1.Setting_Char);
	Set_Para_to_char(Structset_Default_StepuppressureGap9_1.Setting_Para,Structset_Default_StepuppressureGap9_1.veiw_type,Structset_Default_StepuppressureGap9_1.Setting_Char);
	Set_Para_to_char(Structset_Default_Stepuppressuretimes10_1.Setting_Para,Structset_Default_Stepuppressuretimes10_1.veiw_type,Structset_Default_Stepuppressuretimes10_1.Setting_Char);
	
	Set_Para_to_char(Structset_Framemove_Forbiden1_3.Setting_Para,Structset_Framemove_Forbiden1_3.veiw_type,Structset_Framemove_Forbiden1_3.Setting_Char);
	Set_Para_to_char(Structset_Framemove_Alarm2_2.Setting_Para,Structset_Framemove_Alarm2_2.veiw_type,Structset_Framemove_Alarm2_2.Setting_Char);
	Set_Para_to_char(Structset_Framemove_AlarmTime3_1.Setting_Para,Structset_Framemove_AlarmTime3_1.veiw_type,Structset_Framemove_AlarmTime3_1.Setting_Char);
	Set_Para_to_char(Structset_Framemove_movePressure4_1.Setting_Para,Structset_Framemove_movePressure4_1.veiw_type,Structset_Framemove_movePressure4_1.Setting_Char);
	Set_Para_to_char(Structset_Framemove_TransPressure5_1.Setting_Para,Structset_Framemove_TransPressure5_1.veiw_type,Structset_Framemove_TransPressure5_1.Setting_Char);
	Set_Para_to_char(Structset_Framemove_SupportPressure6_1.Setting_Para,Structset_Framemove_SupportPressure6_1.veiw_type,Structset_Framemove_SupportPressure6_1.Setting_Char);
	Set_Para_to_char(Structset_Framemove_TelescopictMove7_2.Setting_Para,Structset_Framemove_TelescopictMove7_2.veiw_type,Structset_Framemove_TelescopictMove7_2.Setting_Char);
	Set_Para_to_char(Structset_Framemove_PlateMove8_2.Setting_Para,Structset_Framemove_PlateMove8_2.veiw_type,Structset_Framemove_PlateMove8_2.Setting_Char);
	Set_Para_to_char(Structset_Framemove_ColumndownDelay9_1.Setting_Para,Structset_Framemove_ColumndownDelay9_1.veiw_type,Structset_Framemove_ColumndownDelay9_1.Setting_Char);
	Set_Para_to_char(Structset_Framemove_ColumndownTime10_1.Setting_Para,Structset_Framemove_ColumndownTime10_1.veiw_type,Structset_Framemove_ColumndownTime10_1.Setting_Char);
	Set_Para_to_char(Structset_Framemove_ColumndownBalanceaction11_2.Setting_Para,Structset_Framemove_ColumndownBalanceaction11_2.veiw_type,Structset_Framemove_ColumndownBalanceaction11_2.Setting_Char);
	Set_Para_to_char(Structset_Framemove_Columndownsideplate12_2.Setting_Para,Structset_Framemove_Columndownsideplate12_2.veiw_type,Structset_Framemove_Columndownsideplate12_2.Setting_Char);
	Set_Para_to_char(Structset_Framemove_DcolumnAgainFrequency13_1.Setting_Para,Structset_Framemove_DcolumnAgainFrequency13_1.veiw_type,Structset_Framemove_DcolumnAgainFrequency13_1.Setting_Char);
	Set_Para_to_char(Structset_Framemove_BmttomUP14_2.Setting_Para,Structset_Framemove_BmttomUP14_2.veiw_type,Structset_Framemove_BmttomUP14_2.Setting_Char);
	Set_Para_to_char16(Structset16_Framemove_Advancaim15_1.Setting_Para,Structset16_Framemove_Advancaim15_1.veiw_type,Structset16_Framemove_Advancaim15_1.Setting_Char);
	Set_Para_to_char(Structset_Framemove_AdvancTime16_1.Setting_Para,Structset_Framemove_AdvancTime16_1.veiw_type,Structset_Framemove_AdvancTime16_1.Setting_Char);
	Set_Para_to_char(Structset_Framemove_Advancsideplate17_2.Setting_Para,Structset_Framemove_Advancsideplate17_2.veiw_type,Structset_Framemove_Advancsideplate17_2.Setting_Char);
	Set_Para_to_char(Structset_Framemove_AdvancagainTime18_1.Setting_Para,Structset_Framemove_AdvancagainTime18_1.veiw_type,Structset_Framemove_AdvancagainTime18_1.Setting_Char);
	Set_Para_to_char(Structset_Framemove_ColumnUPtime19_1.Setting_Para,Structset_Framemove_ColumnUPtime19_1.veiw_type,Structset_Framemove_ColumnUPtime19_1.Setting_Char);
	Set_Para_to_char(Structset_Framemove_ColumnUPBalanceaction20_2.Setting_Para,Structset_Framemove_ColumnUPBalanceaction20_2.veiw_type,Structset_Framemove_ColumnUPBalanceaction20_2.Setting_Char);
	Set_Para_to_char(Structset_Framemove_ColumnUPsideplatew21_2.Setting_Para,Structset_Framemove_ColumnUPsideplatew21_2.veiw_type,Structset_Framemove_ColumnUPsideplatew21_2.Setting_Char);
	Set_Para_to_char(Structset_Framemove_Spray22_2.Setting_Para,Structset_Framemove_Spray22_2.veiw_type,Structset_Framemove_Spray22_2.Setting_Char);
	Set_Para_to_char(Structset_Framemove_SprayTime23_1.Setting_Para,Structset_Framemove_SprayTime23_1.veiw_type,Structset_Framemove_SprayTime23_1.Setting_Char);
	Set_Para_to_char(Structset_Framemove_GroupSeparatedNum24_1.Setting_Para,Structset_Framemove_GroupSeparatedNum24_1.veiw_type,Structset_Framemove_GroupSeparatedNum24_1.Setting_Char);
	Set_Para_to_char(Structset_Framemove_GroupRange25_1.Setting_Para,Structset_Framemove_GroupRange25_1.veiw_type,Structset_Framemove_GroupRange25_1.Setting_Char);
	
	Set_Para_to_char(Structset_ChutePush_Forbiden1_3.Setting_Para,Structset_ChutePush_Forbiden1_3.veiw_type,Structset_ChutePush_Forbiden1_3.Setting_Char);
	Set_Para_to_char(Structset_ChutePush_time2_1.Setting_Para,Structset_ChutePush_time2_1.veiw_type,Structset_ChutePush_time2_1.Setting_Char);
	Set_Para_to_char16(Structset16_ChutePush_aimDistance3_1.Setting_Para,Structset16_ChutePush_aimDistance3_1.veiw_type,Structset16_ChutePush_aimDistance3_1.Setting_Char);
	Set_Para_to_char(Structset_ChutePush_Pressure4_1.Setting_Para,Structset_ChutePush_Pressure4_1.veiw_type,Structset_ChutePush_Pressure4_1.Setting_Char);
	Set_Para_to_char16(Structset16_ChutePush_Maximumdifference5_1.Setting_Para,Structset16_ChutePush_Maximumdifference5_1.veiw_type,Structset16_ChutePush_Maximumdifference5_1.Setting_Char);
	Set_Para_to_char(Structset_ChutePush_GroupSeparatedNum6_1.Setting_Para,Structset_ChutePush_GroupSeparatedNum6_1.veiw_type,Structset_ChutePush_GroupSeparatedNum6_1.Setting_Char);
	Set_Para_to_char(Structset_ChutePush_GroupRange7_1.Setting_Para,Structset_ChutePush_GroupRange7_1.veiw_type,Structset_ChutePush_GroupRange7_1.Setting_Char);
	
	Set_Para_to_char(Structset_Telescopict_Forbiden1_3.Setting_Para,Structset_Telescopict_Forbiden1_3.veiw_type,Structset_Telescopict_Forbiden1_3.Setting_Char);
	Set_Para_to_char(Structset_Telescopict_StretchTime2_1.Setting_Para,Structset_Telescopict_StretchTime2_1.veiw_type,Structset_Telescopict_StretchTime2_1.Setting_Char);
	Set_Para_to_char(Structset_Telescopict_ShrinkTime3_1.Setting_Para,Structset_Telescopict_ShrinkTime3_1.veiw_type,Structset_Telescopict_ShrinkTime3_1.Setting_Char);
	Set_Para_to_char(Structset_Telescopict_DcolumnTime4_1.Setting_Para,Structset_Telescopict_DcolumnTime4_1.veiw_type,Structset_Telescopict_DcolumnTime4_1.Setting_Char);
	Set_Para_to_char(Structset_Telescopict_Separatednum5_1.Setting_Para,Structset_Telescopict_Separatednum5_1.veiw_type,Structset_Telescopict_Separatednum5_1.Setting_Char);
	Set_Para_to_char(Structset_Telescopict_Range6_1.Setting_Para,Structset_Telescopict_Range6_1.veiw_type,Structset_Telescopict_Range6_1.Setting_Char);
	Set_Para_to_char(Structset_PLATE_Forbiden7_3.Setting_Para,Structset_PLATE_Forbiden7_3.veiw_type,Structset_PLATE_Forbiden7_3.Setting_Char);
	Set_Para_to_char(Structset_PLATE_StretchTime8_1.Setting_Para,Structset_PLATE_StretchTime8_1.veiw_type,Structset_PLATE_StretchTime8_1.Setting_Char);
	Set_Para_to_char(Structset_PLATE_ShrinkTime9_1.Setting_Para,Structset_PLATE_ShrinkTime9_1.veiw_type,Structset_PLATE_ShrinkTime9_1.Setting_Char);
	
	Set_Para_to_char(Structset_ChutePull_Forbiden1_3.Setting_Para,Structset_ChutePull_Forbiden1_3.veiw_type,Structset_ChutePull_Forbiden1_3.Setting_Char);
	Set_Para_to_char(Structset_ChutePull_time2_1.Setting_Para,Structset_ChutePull_time2_1.veiw_type,Structset_ChutePull_time2_1.Setting_Char);
	Set_Para_to_char(Structset_ChutePull_aimDistance3_1.Setting_Para,Structset_ChutePull_aimDistance3_1.veiw_type,Structset_ChutePull_aimDistance3_1.Setting_Char);
	Set_Para_to_char(Structset_ChutePull_Pressure4_1.Setting_Para,Structset_ChutePull_Pressure4_1.veiw_type,Structset_ChutePull_Pressure4_1.Setting_Char);
	Set_Para_to_char(Structset_ChutePull_GroupSeparatedNum5_1.Setting_Para,Structset_ChutePull_GroupSeparatedNum5_1.veiw_type,Structset_ChutePull_GroupSeparatedNum5_1.Setting_Char);
	Set_Para_to_char(Structset_ChutePull_GroupRange6_1.Setting_Para,Structset_ChutePull_GroupRange6_1.veiw_type,Structset_ChutePull_GroupRange6_1.Setting_Char);
	Set_Para_to_char(Structset_Mistspray_Forbiden7_3.Setting_Para,Structset_Mistspray_Forbiden7_3.veiw_type,Structset_Mistspray_Forbiden7_3.Setting_Char);
	Set_Para_to_char(Structset_Mistspray_SeparatedNum8_1.Setting_Para,Structset_Mistspray_SeparatedNum8_1.veiw_type,Structset_Mistspray_SeparatedNum8_1.Setting_Char);
	Set_Para_to_char(Structset_Mistspray_Range9_1.Setting_Para,Structset_Mistspray_Range9_1.veiw_type,Structset_Mistspray_Range9_1.Setting_Char);
	Set_Para_to_char(Structset_Mistspray_Time10_1.Setting_Para,Structset_Mistspray_Time10_1.veiw_type,Structset_Mistspray_Time10_1.Setting_Char);
	
	Set_Para_to_char(Structset_Fault_Lcommunicate1_5.Setting_Para,Structset_Fault_Lcommunicate1_5.veiw_type,Structset_Fault_Lcommunicate1_5.Setting_Char);
	Set_Para_to_char(Structset_Fault_Rcommunicate2_5.Setting_Para,Structset_Fault_Rcommunicate2_5.veiw_type,Structset_Fault_Rcommunicate2_5.Setting_Char);
	Set_Para_to_char(Structset_Fault_BUScommunicate3_5.Setting_Para,Structset_Fault_BUScommunicate3_5.veiw_type,Structset_Fault_BUScommunicate3_5.Setting_Char);
	Set_Para_to_char(Structset_Fault_PressureStepup4_5.Setting_Para,Structset_Fault_PressureStepup4_5.veiw_type,Structset_Fault_PressureStepup4_5.Setting_Char);
	Set_Para_to_char(Structset_Fault_Sensorpressure5_5.Setting_Para,Structset_Fault_Sensorpressure5_5.veiw_type,Structset_Fault_Sensorpressure5_5.Setting_Char);
	Set_Para_to_char(Structset_Fault_Sensordistance6_5.Setting_Para,Structset_Fault_Sensordistance6_5.veiw_type,Structset_Fault_Sensordistance6_5.Setting_Char);
	Set_Para_to_char(Structset_Fault_Sensorangel7_5.Setting_Para,Structset_Fault_Sensorangel7_5.veiw_type,Structset_Fault_Sensorangel7_5.Setting_Char);
	Set_Para_to_char(Structset_Fault_Receiver8_5.Setting_Para,Structset_Fault_Receiver8_5.veiw_type,Structset_Fault_Receiver8_5.Setting_Char);
	Set_Para_to_char(Structset_Fault_Driver9_5.Setting_Para,Structset_Fault_Driver9_5.veiw_type,Structset_Fault_Driver9_5.Setting_Char);
	
	
}








