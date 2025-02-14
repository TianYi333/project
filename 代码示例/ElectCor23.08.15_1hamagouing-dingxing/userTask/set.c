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
uint8_t variant_Set_Qreturn;//延时按键检测变量  创建人蒋博 创建日期2023/01/15


void Set_Para_to_char(uint8_t parameter,uint8_t veiw_type,char *Set_char)
{
	/*****显示类型*****
1  数字变量
2  开1关0
3  允许1禁止0
4  左DIRECTION_LEFT右DIRECTION_RIGHT
5  故障1正常0
6   急停1正常0
*****************/
	switch(veiw_type)
	{
		case 1://数字变量
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
			if(parameter >= 1)//开GB2312：BFAA
			{
				*Set_char = 0xBF;
				*(Set_char+1) = 0xAA;
				*(Set_char+2) = '\0';
				*(Set_char+3) = '\0';
			}
			else if(parameter == 0)//关GB2312：B9D8
			{
				*Set_char = 0xB9;
				*(Set_char+1) = 0xD8;
				*(Set_char+2) = '\0';
				*(Set_char+3) = '\0';
			}
		break;
		
		case 3:
			if(parameter >= 1)//允GB2312：D4CA//许GB2312：D0ED
			{
				*Set_char = 0xD4;
				*(Set_char+1) = 0xCA;
				*(Set_char+2) = 0xD0;
				*(Set_char+3) = 0xED;
				*(Set_char+4) = '\0';
			}
			else if(parameter == 0)//禁GB2312：BDFB//止GB2312：D6B9
			{
				*Set_char = 0xBD;
				*(Set_char+1) = 0xFB;
				*(Set_char+2) = 0xD6;
				*(Set_char+3) = 0xB9;
				*(Set_char+4) = '\0';
			}
		break;
		
		case 4:
			if(parameter == DIRECTION_LEFT)//左GB2312：D7F3
			{
				*Set_char = 0xD7;
				*(Set_char+1) = 0xF3;
				*(Set_char+2) = '\0';
				*(Set_char+3) = '\0';
			}
			else if(parameter == DIRECTION_RIGHT) //右GB2312：D3D2
			{
				*Set_char = 0xD3;
				*(Set_char+1) = 0xD2;
				*(Set_char+2) = '\0';
				*(Set_char+3) = '\0';
			}
		break;
		
		case 5:
			if(parameter >= 1)//正GB2312：D5FD//常GB2312：B3A3
			{
				*Set_char = 0xD5;
				*(Set_char+1) = 0xFD;
				*(Set_char+2) = 0xB3;
				*(Set_char+3) = 0xA3;
				*(Set_char+4) = '\0';
			}
			else if(parameter == 0)//故GB2312：B9CA//障GB2312：D5CF
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
	uint16_t Setting_Prar = 0;//用于返回的变量值
	uint16_t Get_Key_Value;
	uint8_t const_Set_InterfaceFlag =1;
	char Num_Str[5];//保存变量值转字符串
	uint8_t Setting_Bool = 0;
	variant_Set_Qreturn =1;
	Clear_Screen();
	Setting_Bool = Previous_Para;
	while(variant_Set_Qreturn==pdTRUE)
	{
		switch(veiw_type)
		{
			case 1:
				Display_String(1,0,"请输入新参数");
			break;
			
			case 2:
				Display_String(1,0,"请选择");
				if(Setting_Bool == 0)
				{
					Display_String(3,0,"            开");
					Display_String_Reverse(5,0,"            关");
				}
				else if(Setting_Bool >= 1)
				{
					Display_String_Reverse(3,0,"            开");
					Display_String(5,0,"            关");
				}
			break;
			
			case 3:
				Display_String(1,0,"请选择");
				if(Setting_Bool == 0)
				{
					Display_String(3,0,"          允许");
					Display_String_Reverse(5,0,"          禁止");
				}
				else if(Setting_Bool >= 1)
				{
					Display_String_Reverse(3,0,"          允许");
					Display_String(5,0,"          禁止");
				}
						
			break;
			
			case 4:
				Display_String(1,0,"请选择");
				if(Setting_Bool == DIRECTION_LEFT)
				{
					Display_String_Reverse(3,0,"          左");
					Display_String(5,0,"          右");
				}
				else if(Setting_Bool == DIRECTION_RIGHT)
				{
					Display_String(3,0,"              左");
					Display_String_Reverse(5,0,"              右");
				}
			break;
			
			case 5:
				Display_String(1,0,"请选择");
				if(Setting_Bool == 0)
				{
					Display_String(3,0,"          正常");
					Display_String_Reverse(5,0,"          故障");
				}
				else if(Setting_Bool >= 1)
				{
					Display_String_Reverse(3,0,"          正常");
					Display_String(5,0,"          故障");
				}
			break;
			
			default:
			break;
		}
		variant_Set_Qreturn = xQueueReceive(Key_Value_ISR_Queue,&Get_Key_Value,Structset_GlobalSet_Settingtime3_1.Setting_Para*2000);//
		if(variant_Set_Qreturn == pdFALSE)
		{
			variant_Mainlogic_KeyValue = 0;//不要进入下面的switch中的任何一个case	
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
							case KEY12_VALUE://停止键
								variant_Set_Qreturn = 1;
								return Previous_Para;
								
							
							case KEY26_VALUE://确认键
								variant_Set_Qreturn = 1;

								return Setting_Prar;
						
							
							default:
								break;
						}
						sprintf(Num_Str, "%d", Setting_Prar);//变量值转字符串
						Display_String(3,64,Num_Str);
					break;
				
					case 2:
						
						switch(Get_Key_Value)
						{
								case KEY20_VALUE://下
								case KEY24_VALUE://上
									if(Setting_Bool == 0)
									{
										Setting_Bool = 1;
									}
									else if(Setting_Bool >= 1)
									{
										Setting_Bool = 0;
									}
									break;
									
								case KEY12_VALUE://停止键
									variant_Set_Qreturn = 1;
									return Previous_Para;
							
								
								case KEY26_VALUE://确认键
									variant_Set_Qreturn = 1;
									return Setting_Bool;
								
							
								default:
								break;
						}
				
				
					break;
						
					case 3:
						
						
						switch(Get_Key_Value)
						{
								case KEY20_VALUE://下
								case KEY24_VALUE://上
									if(Setting_Bool == 0)
									{
										Setting_Bool = 1;
									}
									else if(Setting_Bool >= 1)
									{
										Setting_Bool = 0;
									}
									break;
								case KEY12_VALUE://停止键
									return Previous_Para;
							
								
								case KEY26_VALUE://确认键
									return Setting_Bool;
						
								
								default:
									break;
						}
				
				
					break;
						
					case 4:
						
						
						
						switch(Get_Key_Value)
						{
								case KEY20_VALUE://下
								case KEY24_VALUE://上
									if(Setting_Bool == DIRECTION_LEFT)
									{
										Setting_Bool = DIRECTION_RIGHT;
									}
									else if(Setting_Bool == DIRECTION_RIGHT)
									{
										Setting_Bool = DIRECTION_LEFT;
									}
									break;
								case KEY12_VALUE://停止键
									return Previous_Para;
								
								case KEY26_VALUE://确认键
									return Setting_Bool;
							
								
								default:
								break;
							
						}
				
				
					break;
						
					case 5:
						
						switch(Get_Key_Value)
						{
								case KEY20_VALUE://下
								case KEY24_VALUE://上
									if(Setting_Bool == 0)
									{
										Setting_Bool = 1;
									}
									else if(Setting_Bool >= 1)
									{
										Setting_Bool = 0;
									}
									break;
								case KEY12_VALUE://停止键
									return Previous_Para;
								
								
								case KEY26_VALUE://确认键
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
	 Structset_Fault_Lcommunicate1_5.Setting_Para = 1;//左邻架通信故障
  Structset_Fault_Rcommunicate2_5.Setting_Para = 1;//右邻架通信故障
  Structset_Fault_BUScommunicate3_5.Setting_Para = 1;//总线通信故障
  Structset_Fault_PressureStepup4_5.Setting_Para = 1;//补压故障
  Structset_Fault_Sensorpressure5_5.Setting_Para = 1;//压力传感器故障
  Structset_Fault_Sensordistance6_5.Setting_Para = 1;//行程传感器故障
  Structset_Fault_Sensorangel7_5.Setting_Para = 1;//倾角传感器故障
  Structset_Fault_Receiver8_5.Setting_Para = 1;//遥控器接收器故障
  Structset_Fault_Driver9_5.Setting_Para = 1;//驱动器故障
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
	
	Display_String(1,0,"   请输入密码   ");
	
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
		Display_String(1,0,"    密码错误    ");
		vTaskDelay(500);
		return 0;
	}






}
/*****显示类型*****

1  数字变量
2  开1关0
3  允许1禁止0
4  左DIRECTION_LEFT右DIRECTION_RIGHT
5  故障1正常0
6   急停1正常0
*****************/
/***************整体设置*****************/


Set_Struct  Structset_GlobalSet_MaxDeviceNum1_1;//最大架号
Set_Struct  Structset_GlobalSet_minDeviceNum2_1;//最小架号
Set_Struct  Structset_GlobalSet_Settingtime3_1;//主控时间
Set_Struct  Structset_GlobalSet_Increase4_4;//地址增向
Set_Struct  Structset_GlobalSet_Conswitch5_2;//转换器开关
Set_Struct  Structset_GlobalSet_Conposition6_1;//转换器位置
Set_Struct  Structset_GlobalSet_PositionUpload7_2;//煤机位置上传
Set_Struct  Structset_GlobalSet_Adjacentcheak8_2;//邻架检测
Set_Struct  Structset_GlobalSet_AdjacentWarring9_2;//邻架报警
Set_Struct  Structset_GlobalSet_Adjacenttime10_1;//邻架检测时间
Set_Struct  Structset_GlobalSet_BUSworkcheak11_2;//总线检测
Set_Struct  Structset_GlobalSet_BUSreport12_2;//总线报送
Set_Struct  Structset_GlobalSet_BUScheakTime13_1;//总线检测时间

/***************服务*****************/
Set_Struct  Structset_Service_DeviceNum1_1;//架号
Set_Struct  Structset_Service_Adjacentcorrelation2_2;//邻架相关
Set_Struct  Structset_Service_RemoteCortrol3_2;//遥控开关


/*****显示类型*****
1  数字变量
2  开1关0
3  允许1禁止0
4  左DIRECTION_LEFT右DIRECTION_RIGHT
5  故障1正常0
6   急停1正常0
*****************/

/***************默认参数*****************/
Set_Struct  Structset_Default_Pressurecheak1_2;//立柱压力检测
Set_Struct  Structset_Default_Distancecheak2_2;//推移行程检测
Set_Struct  Structset_Default_Pushtime3_1;//本架推溜时间
Set_Struct_paralong  Structset16_Default_Pushdistance4_1;//本架推溜行程                                         //16位一
Set_Struct  Structset_Default_PressureLowlimt5_1;//补压力下限  
Set_Struct  Structset_Default_PressureHighlimt6_1;//补压力上限
Set_Struct  Structset_Default_aimPressure7_1;//目标压力			
Set_Struct  Structset_Default_StepuppressureTime8_1;//单次时间
Set_Struct  Structset_Default_StepuppressureGap9_1;//补压间隔
Set_Struct  Structset_Default_Stepuppressuretimes10_1;//补压次数



/***************移架*****************/
Set_Struct_charlong  Structset_Framemove_Forbiden1_3;//移架动作禁止
Set_Struct  Structset_Framemove_Alarm2_2;//移架报警 开关
Set_Struct  Structset_Framemove_AlarmTime3_1;//报警时间
Set_Struct  Structset_Framemove_movePressure4_1;//移架压力     立柱压力必须卸至此压力以下，程序才允许本架移架。
Set_Struct  Structset_Framemove_TransPressure5_1;//过度压力      立柱压力必须升至此压力以上，程序才允许邻架作自动移架。    邻架相关
Set_Struct  Structset_Framemove_SupportPressure6_1;//支撑压力   目标压力
Set_Struct  Structset_Framemove_TelescopictMove7_2;//伸缩梁动作 
Set_Struct  Structset_Framemove_PlateMove8_2;//护帮板动作
Set_Struct  Structset_Framemove_ColumndownDelay9_1;//降柱延时  伸缩梁护帮版动作后延时降柱
Set_Struct  Structset_Framemove_ColumndownTime10_1;//降柱时间
Set_Struct  Structset_Framemove_ColumndownBalanceaction11_2;//降柱时平衡梁动作开关
Set_Struct  Structset_Framemove_Columndownsideplate12_2;//降柱侧推开关
Set_Struct  Structset_Framemove_DcolumnAgainFrequency13_1;//再降柱次数
Set_Struct  Structset_Framemove_BmttomUP14_2;//抬底动作开关
Set_Struct_paralong  Structset16_Framemove_Advancaim15_1;//移架行程目标                                         //16位二
Set_Struct  Structset_Framemove_AdvancTime16_1;//移架时间   移架允许最大时间
Set_Struct  Structset_Framemove_Advancsideplate17_2;//移架侧护开关
Set_Struct  Structset_Framemove_AdvancagainTime18_1;//再移架时间
Set_Struct  Structset_Framemove_ColumnUPtime19_1;//升柱时间
Set_Struct  Structset_Framemove_ColumnUPBalanceaction20_2;//升柱平衡开关
Set_Struct  Structset_Framemove_ColumnUPsideplatew21_2;//升柱侧护开关
Set_Struct  Structset_Framemove_Spray22_2;//移架喷雾开关
Set_Struct  Structset_Framemove_SprayTime23_1;//喷雾时间
Set_Struct  Structset_Framemove_GroupSeparatedNum24_1;//成组间隔
Set_Struct  Structset_Framemove_GroupRange25_1;//成组范围

/****************推溜********************/
Set_Struct_charlong  Structset_ChutePush_Forbiden1_3;//推溜动作禁止
Set_Struct  Structset_ChutePush_time2_1;//推溜时间
Set_Struct_paralong  Structset16_ChutePush_aimDistance3_1;//目标行程                                         //16位三
Set_Struct  Structset_ChutePush_Pressure4_1;//推溜最小压力
Set_Struct_paralong  Structset16_ChutePush_Maximumdifference5_1;//最大差值                                   //16位四
Set_Struct  Structset_ChutePush_GroupSeparatedNum6_1;//推溜间隔
Set_Struct  Structset_ChutePush_GroupRange7_1;//推溜范围


/****************伸缩梁护帮********************/

Set_Struct_charlong  Structset_Telescopict_Forbiden1_3;//伸缩梁动作
Set_Struct  Structset_Telescopict_StretchTime2_1;
Set_Struct  Structset_Telescopict_ShrinkTime3_1;
Set_Struct  Structset_Telescopict_DcolumnTime4_1;//动作伸缩梁时的降柱时间
Set_Struct  Structset_Telescopict_Separatednum5_1;
Set_Struct  Structset_Telescopict_Range6_1;
Set_Struct_charlong  Structset_PLATE_Forbiden7_3;
Set_Struct  Structset_PLATE_StretchTime8_1;
Set_Struct  Structset_PLATE_ShrinkTime9_1;


/****************拉溜和喷雾********************/
Set_Struct_charlong  Structset_ChutePull_Forbiden1_3;//拉溜动作禁止
Set_Struct  Structset_ChutePull_time2_1;//拉溜时间
Set_Struct  Structset_ChutePull_aimDistance3_1;//目标行程
Set_Struct  Structset_ChutePull_Pressure4_1;//推溜最小压力
Set_Struct  Structset_ChutePull_GroupSeparatedNum5_1;//推溜间隔
Set_Struct  Structset_ChutePull_GroupRange6_1;//推溜范围
Set_Struct_charlong  Structset_Mistspray_Forbiden7_3;//喷雾间隔
Set_Struct  Structset_Mistspray_SeparatedNum8_1;//喷雾间隔
Set_Struct  Structset_Mistspray_Range9_1;//范围
Set_Struct  Structset_Mistspray_Time10_1;//时间

/****************故障列表********************/
Set_Struct_charlong  Structset_Fault_Lcommunicate1_5;//左邻架通信故障
Set_Struct_charlong  Structset_Fault_Rcommunicate2_5;//右邻架通信故障
Set_Struct_charlong  Structset_Fault_BUScommunicate3_5;//总线通信故障
Set_Struct_charlong  Structset_Fault_PressureStepup4_5;//补压故障
Set_Struct_charlong  Structset_Fault_Sensorpressure5_5;//压力传感器故障
Set_Struct_charlong  Structset_Fault_Sensordistance6_5;//行程传感器故障
Set_Struct_charlong  Structset_Fault_Sensorangel7_5;//倾角传感器故障
Set_Struct_charlong  Structset_Fault_Receiver8_5;//遥控器接收器故障
Set_Struct_charlong  Structset_Fault_Driver9_5;//驱动器故障

/*******************跟机******************/
Set_Struct  Structset_Follow_Head1_1;//范围
Set_Struct  Structset_Follow_Tail2_1;//时间



void Set_Init_Para(void)
{
/***************整体设置*****************/
  Structset_GlobalSet_MaxDeviceNum1_1.Setting_Para = 101;//最大架号
 Structset_GlobalSet_minDeviceNum2_1.Setting_Para = 1;//最小架号
 Structset_GlobalSet_Settingtime3_1.Setting_Para = 8;//主控时间
Structset_GlobalSet_Increase4_4.Setting_Para = 0x02;//地址增向
Structset_GlobalSet_Conswitch5_2.Setting_Para = 1;//转换器开关
 Structset_GlobalSet_Conposition6_1.Setting_Para = 0;//转换器位置
 Structset_GlobalSet_PositionUpload7_2.Setting_Para = 1;//煤机位置上传
 Structset_GlobalSet_Adjacentcheak8_2.Setting_Para = 1;//邻架检测
  Structset_GlobalSet_AdjacentWarring9_2.Setting_Para = 1;//邻架报警
  Structset_GlobalSet_Adjacenttime10_1.Setting_Para = 60;//邻架检测时间
  Structset_GlobalSet_BUSworkcheak11_2.Setting_Para = 1;//总线检测
 Structset_GlobalSet_BUSreport12_2.Setting_Para = 1;//总线报送
Structset_GlobalSet_BUScheakTime13_1.Setting_Para = 10;//总线检测时间
/***************服务*****************/

//	Structset_Service_DeviceNum1_1.Setting_Para = 10;

 Structset_Service_Adjacentcorrelation2_2.Setting_Para = 0;//邻架相关
 Structset_Service_RemoteCortrol3_2.Setting_Para = 0;//遥控开关




/*****显示类型*****
1  数字变量
2  开1关0
3  允许1禁止0
4  左DIRECTION_LEFT右DIRECTION_RIGHT
5  故障1正常0
6   急停1正常0
*****************/

/***************默认参数*****************/
 Structset_Default_Pressurecheak1_2.Setting_Para = 1;//立柱压力检测
 Structset_Default_Distancecheak2_2.Setting_Para = 1;//推移行程检测
  Structset_Default_Pushtime3_1.Setting_Para = 10;//本架推溜时间
  Structset16_Default_Pushdistance4_1.Setting_Para = 100;//本架推溜行程
  Structset_Default_PressureLowlimt5_1.Setting_Para = 0;//补压力下限
  Structset_Default_PressureHighlimt6_1.Setting_Para = 0;//补压力上限
  Structset_Default_aimPressure7_1.Setting_Para = 10;//目标压力
  Structset_Default_StepuppressureTime8_1.Setting_Para = 2;//单次时间
  Structset_Default_StepuppressureGap9_1.Setting_Para = 10;//补压间隔
  Structset_Default_Stepuppressuretimes10_1.Setting_Para = 2;//补压次数



/***************移架*****************/
  Structset_Framemove_Forbiden1_3.Setting_Para = 1;//移架动作禁止
 Structset_Framemove_Alarm2_2.Setting_Para = 1;//移架报警 开关
  Structset_Framemove_AlarmTime3_1.Setting_Para = 50;//报警时间
 Structset_Framemove_movePressure4_1.Setting_Para = 0;//移架压力     立柱压力必须卸至此压力以下，程序才允许本架移架。
  Structset_Framemove_TransPressure5_1.Setting_Para = 0;//过度压力      立柱压力必须升至此压力以上，程序才允许邻架作自动移架。    邻架相关
  Structset_Framemove_SupportPressure6_1.Setting_Para = 0;//支撑压力   目标压力
  Structset_Framemove_TelescopictMove7_2.Setting_Para = 1;//伸缩梁动作 
  Structset_Framemove_PlateMove8_2.Setting_Para = 1;//护帮板动作
  Structset_Framemove_ColumndownDelay9_1.Setting_Para = 1;//降柱延时  伸缩梁护帮版动作后延时降柱
  Structset_Framemove_ColumndownTime10_1.Setting_Para = 2;//降柱时间
  Structset_Framemove_ColumndownBalanceaction11_2.Setting_Para = 0;//降柱时平衡梁动作开关
  Structset_Framemove_Columndownsideplate12_2.Setting_Para = 0;//降柱侧推开关
  Structset_Framemove_DcolumnAgainFrequency13_1.Setting_Para = 0;//再降柱次数
  Structset_Framemove_BmttomUP14_2.Setting_Para = 1;//抬底动作开关
  Structset16_Framemove_Advancaim15_1.Setting_Para = 0;//移架行程目标
  Structset_Framemove_AdvancTime16_1.Setting_Para = 10;//移架时间   移架允许最大时间
  Structset_Framemove_Advancsideplate17_2.Setting_Para = 1;//移架侧护开关
  Structset_Framemove_AdvancagainTime18_1.Setting_Para = 5;//再移架时间
 Structset_Framemove_ColumnUPtime19_1.Setting_Para = 3;//升柱时间
  Structset_Framemove_ColumnUPBalanceaction20_2.Setting_Para = 0;//升柱平衡开关
  Structset_Framemove_ColumnUPsideplatew21_2.Setting_Para = 0;//升柱侧护开关
  Structset_Framemove_Spray22_2.Setting_Para = 1;//移架喷雾开关
  Structset_Framemove_SprayTime23_1.Setting_Para = 5;//喷雾时间
//  Structset_Framemove_GroupSeparatedNum24_1.Setting_Para = 0;//成组间隔
//  Structset_Framemove_GroupRange25_1.Setting_Para = 5;//成组范围

/****************推溜********************/
  Structset_ChutePush_Forbiden1_3.Setting_Para = 1;//推溜动作禁止
  Structset_ChutePush_time2_1.Setting_Para = 10;//推溜时间
  Structset16_ChutePush_aimDistance3_1.Setting_Para = 100;//目标行程
  Structset_ChutePush_Pressure4_1.Setting_Para = 0;//推溜最小压力
  Structset16_ChutePush_Maximumdifference5_1.Setting_Para = 0;//最大差值
//  Structset_ChutePush_GroupSeparatedNum6_1.Setting_Para = 0;//推溜间隔
//  Structset_ChutePush_GroupRange7_1.Setting_Para = 5;//推溜范围


/****************伸缩梁护帮********************/

  Structset_Telescopict_Forbiden1_3.Setting_Para = 1;
  Structset_Telescopict_StretchTime2_1.Setting_Para = 3;
//  Structset_Telescopict_ShrinkTime3_1.Setting_Para = 3;
//  Structset_Telescopict_DcolumnTime4_1.Setting_Para = 2;//动作伸缩梁时的降柱时间
//  Structset_Telescopict_Separatednum5_1.Setting_Para = 0;
//  Structset_Telescopict_Range6_1.Setting_Para = 5;
  Structset_PLATE_Forbiden7_3.Setting_Para = 1;
//  Structset_PLATE_StretchTime8_1.Setting_Para = 3;
//  Structset_PLATE_ShrinkTime9_1.Setting_Para = 3;


/****************拉溜和喷雾********************/
  Structset_ChutePull_Forbiden1_3.Setting_Para = 1;//拉溜动作禁止
  Structset_ChutePull_time2_1.Setting_Para = 10;//拉溜时间
  Structset_ChutePull_aimDistance3_1.Setting_Para = 100;//目标行程
  Structset_ChutePull_Pressure4_1.Setting_Para = 0;//推溜最小压力
//  Structset_ChutePull_GroupSeparatedNum5_1.Setting_Para = 0;//推溜间隔
//  Structset_ChutePull_GroupRange6_1.Setting_Para = 5;//推溜范围
//  Structset_Mistspray_Forbiden7_3.Setting_Para = 1;//喷雾间隔
//  Structset_Mistspray_SeparatedNum8_1.Setting_Para = 0;//喷雾间隔
//  Structset_Mistspray_Range9_1.Setting_Para = 5;//范围
//  Structset_Mistspray_Time10_1.Setting_Para = 5;//时间

/****************故障列表********************/
  Structset_Fault_Lcommunicate1_5.Setting_Para = 1;//左邻架通信故障
  Structset_Fault_Rcommunicate2_5.Setting_Para = 1;//右邻架通信故障
  Structset_Fault_BUScommunicate3_5.Setting_Para = 1;//总线通信故障
  Structset_Fault_PressureStepup4_5.Setting_Para = 1;//补压故障
  Structset_Fault_Sensorpressure5_5.Setting_Para = 1;//压力传感器故障
  Structset_Fault_Sensordistance6_5.Setting_Para = 1;//行程传感器故障
  Structset_Fault_Sensorangel7_5.Setting_Para = 1;//倾角传感器故障
  Structset_Fault_Receiver8_5.Setting_Para = 1;//遥控器接收器故障
  Structset_Fault_Driver9_5.Setting_Para = 1;//驱动器故障

	
	
	  Structset_Follow_Head1_1.Setting_Para = 1;//跟机机头
  Structset_Follow_Tail2_1.Setting_Para = 8;//跟机机尾
	
	
	
	
}

void Init_set_typeFuction(void)
{
	/***************整体设置*****************/
  Structset_GlobalSet_MaxDeviceNum1_1.veiw_type = 1;//最大架号
 Structset_GlobalSet_minDeviceNum2_1.veiw_type = 1;//最小架号
 Structset_GlobalSet_Settingtime3_1.veiw_type = 1;//主控时间
Structset_GlobalSet_Increase4_4.veiw_type = 4;//地址增向
Structset_GlobalSet_Conswitch5_2.veiw_type = 2;//转换器开关
 Structset_GlobalSet_Conposition6_1.veiw_type = 1;//转换器位置
 Structset_GlobalSet_PositionUpload7_2.veiw_type = 2;//煤机位置上传
 Structset_GlobalSet_Adjacentcheak8_2.veiw_type = 2;//邻架检测
  Structset_GlobalSet_AdjacentWarring9_2.veiw_type = 2;//邻架报警
  Structset_GlobalSet_Adjacenttime10_1.veiw_type = 1;//邻架检测时间
  Structset_GlobalSet_BUSworkcheak11_2.veiw_type = 2;//总线检测
 Structset_GlobalSet_BUSreport12_2.veiw_type = 2;//总线报送
Structset_GlobalSet_BUScheakTime13_1.veiw_type = 1;//总线检测时间
/***************服务*****************/

  Structset_Service_DeviceNum1_1.veiw_type = 1;//架号
 Structset_Service_Adjacentcorrelation2_2.veiw_type = 2;//邻架相关
 Structset_Service_RemoteCortrol3_2.veiw_type = 2;//遥控开关




/*****显示类型*****
1  数字变量
2  开1关0
3  允许1禁止0
4  左DIRECTION_LEFT右DIRECTION_RIGHT
5  故障1正常0
6   急停1正常0
*****************/

/***************默认参数*****************/
 Structset_Default_Pressurecheak1_2.veiw_type = 2;//立柱压力检测
 Structset_Default_Distancecheak2_2.veiw_type = 2;//推移行程检测
  Structset_Default_Pushtime3_1.veiw_type = 1;//本架推溜时间
  Structset16_Default_Pushdistance4_1.veiw_type = 1;//本架推溜行程
  Structset_Default_PressureLowlimt5_1.veiw_type = 1;//补压力下限
  Structset_Default_PressureHighlimt6_1.veiw_type = 1;//补压力上限
  Structset_Default_aimPressure7_1.veiw_type = 1;//目标压力
  Structset_Default_StepuppressureTime8_1.veiw_type = 1;//单次时间
  Structset_Default_StepuppressureGap9_1.veiw_type = 1;//补压间隔
  Structset_Default_Stepuppressuretimes10_1.veiw_type = 1;//补压次数



/***************移架*****************/
  Structset_Framemove_Forbiden1_3.veiw_type = 3;//移架动作禁止
 Structset_Framemove_Alarm2_2.veiw_type = 2;//移架报警 开关
  Structset_Framemove_AlarmTime3_1.veiw_type = 1;//报警时间
 Structset_Framemove_movePressure4_1.veiw_type = 1;//移架压力     立柱压力必须卸至此压力以下，程序才允许本架移架。
  Structset_Framemove_TransPressure5_1.veiw_type = 1;//过度压力      立柱压力必须升至此压力以上，程序才允许邻架作自动移架。    邻架相关
  Structset_Framemove_SupportPressure6_1.veiw_type = 1;//支撑压力   目标压力
  Structset_Framemove_TelescopictMove7_2.veiw_type = 2;//伸缩梁动作 
  Structset_Framemove_PlateMove8_2.veiw_type = 2;//护帮板动作
  Structset_Framemove_ColumndownDelay9_1.veiw_type = 1;//降柱延时  伸缩梁护帮版动作后延时降柱
  Structset_Framemove_ColumndownTime10_1.veiw_type = 1;//降柱时间
  Structset_Framemove_ColumndownBalanceaction11_2.veiw_type = 2;//降柱时平衡梁动作开关
  Structset_Framemove_Columndownsideplate12_2.veiw_type = 2;//降柱侧推开关
  Structset_Framemove_DcolumnAgainFrequency13_1.veiw_type = 1;//再降柱次数
  Structset_Framemove_BmttomUP14_2.veiw_type = 2;//抬底动作开关
  Structset16_Framemove_Advancaim15_1.veiw_type = 1;//移架行程目标
  Structset_Framemove_AdvancTime16_1.veiw_type = 1;//移架时间   移架允许最大时间
  Structset_Framemove_Advancsideplate17_2.veiw_type = 2;//移架侧护开关
  Structset_Framemove_AdvancagainTime18_1.veiw_type = 1;//再移架时间
 Structset_Framemove_ColumnUPtime19_1.veiw_type = 1;//升柱时间
  Structset_Framemove_ColumnUPBalanceaction20_2.veiw_type = 2;//升柱平衡开关
  Structset_Framemove_ColumnUPsideplatew21_2.veiw_type = 2;//升柱侧护开关
  Structset_Framemove_Spray22_2.veiw_type = 2;//移架喷雾开关
  Structset_Framemove_SprayTime23_1.veiw_type = 1;//喷雾时间
  Structset_Framemove_GroupSeparatedNum24_1.veiw_type = 1;//成组间隔
  Structset_Framemove_GroupRange25_1.veiw_type = 1;//成组范围

/****************推溜********************/
  Structset_ChutePush_Forbiden1_3.veiw_type = 3;//推溜动作禁止
  Structset_ChutePush_time2_1.veiw_type = 1;//推溜时间
  Structset16_ChutePush_aimDistance3_1.veiw_type = 1;//目标行程
  Structset_ChutePush_Pressure4_1.veiw_type = 1;//推溜最小压力
  Structset16_ChutePush_Maximumdifference5_1.veiw_type = 1;//最大差值
  Structset_ChutePush_GroupSeparatedNum6_1.veiw_type = 1;//推溜间隔
  Structset_ChutePush_GroupRange7_1.veiw_type = 1;//推溜范围


/****************伸缩梁护帮********************/

  Structset_Telescopict_Forbiden1_3.veiw_type = 3;
  Structset_Telescopict_StretchTime2_1.veiw_type = 1;
  Structset_Telescopict_ShrinkTime3_1.veiw_type = 1;
  Structset_Telescopict_DcolumnTime4_1.veiw_type = 1;//动作伸缩梁时的降柱时间
  Structset_Telescopict_Separatednum5_1.veiw_type = 1;
  Structset_Telescopict_Range6_1.veiw_type = 1;
  Structset_PLATE_Forbiden7_3.veiw_type = 3;
  Structset_PLATE_StretchTime8_1.veiw_type = 1;
  Structset_PLATE_ShrinkTime9_1.veiw_type = 1;


/****************拉溜和喷雾********************/
  Structset_ChutePull_Forbiden1_3.veiw_type = 3;//拉溜动作禁止
  Structset_ChutePull_time2_1.veiw_type = 1;//拉溜时间
  Structset_ChutePull_aimDistance3_1.veiw_type = 1;//目标行程
  Structset_ChutePull_Pressure4_1.veiw_type = 1;//推溜最小压力
  Structset_ChutePull_GroupSeparatedNum5_1.veiw_type = 1;//推溜间隔
  Structset_ChutePull_GroupRange6_1.veiw_type = 1;//推溜范围
  Structset_Mistspray_Forbiden7_3.veiw_type = 3;//喷雾间隔
  Structset_Mistspray_SeparatedNum8_1.veiw_type = 1;//喷雾间隔
  Structset_Mistspray_Range9_1.veiw_type = 1;//范围
  Structset_Mistspray_Time10_1.veiw_type = 1;//时间

/****************故障列表********************/
  Structset_Fault_Lcommunicate1_5.veiw_type = 5;//左邻架通信故障
  Structset_Fault_Rcommunicate2_5.veiw_type = 5;//右邻架通信故障
  Structset_Fault_BUScommunicate3_5.veiw_type = 5;//总线通信故障
  Structset_Fault_PressureStepup4_5.veiw_type = 5;//补压故障
  Structset_Fault_Sensorpressure5_5.veiw_type = 5;//压力传感器故障
  Structset_Fault_Sensordistance6_5.veiw_type = 5;//行程传感器故障
  Structset_Fault_Sensorangel7_5.veiw_type = 5;//倾角传感器故障
  Structset_Fault_Receiver8_5.veiw_type = 5;//遥控器接收器故障
  Structset_Fault_Driver9_5.veiw_type = 5;//驱动器故障

  Structset_Follow_Head1_1.veiw_type = 1;//跟机机头
  Structset_Follow_Tail2_1.veiw_type = 1;//跟机机尾
  
}



///***************整体设置*****************/


//Set_Struct  Structset_GlobalSet_MaxDeviceNum1_1;//最大架号
//Set_Struct  Structset_GlobalSet_minDeviceNum2_1;//最小架号
//Set_Struct  Structset_GlobalSet_Settingtime3_1;//主控时间
//Set_Struct  Structset_GlobalSet_Increase4_4;//地址增向
//Set_Struct  Structset_GlobalSet_Conswitch5_2;//转换器开关
//Set_Struct  Structset_GlobalSet_Conposition6_1;//转换器位置
//Set_Struct  Structset_GlobalSet_PositionUpload7_2;//煤机位置上传
//Set_Struct  Structset_GlobalSet_Adjacentcheak8_2;//邻架检测
//Set_Struct  Structset_GlobalSet_AdjacentWarring9_2;//邻架报警
//Set_Struct  Structset_GlobalSet_Adjacenttime10_1;//邻架检测时间
//Set_Struct  Structset_GlobalSet_BUSworkcheak11_2;//总线检测
//Set_Struct  Structset_GlobalSet_BUSreport12_2;//总线报送
//Set_Struct  Structset_GlobalSet_BUScheakTime13_1;//总线检测时间

///***************服务*****************/
//Set_Struct  Structset_Service_DeviceNum1_1;//架号
//Set_Struct  Structset_Service_Adjacentcorrelation2_2;//邻架相关
//Set_Struct  Structset_Service_RemoteCortrol3_2;//遥控开关

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








