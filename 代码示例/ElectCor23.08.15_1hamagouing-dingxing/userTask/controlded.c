#include "controlded.h"
#include "canreceive.h"
#include "main.h"
#include "main_logic.h"
#include "interface.h"
#include "stm32f4xx_hal.h"
#include "main_logic.h"
#include "bsp_485.h"
#include <stdio.h>
#include <stdlib.h>
#include "drv_usart.h"
#include "bsp_mcz33996.h"
#include "set.h"
#include "group.h"
#include "interface.h"
#include "bsp_hd7279.h"
//#define   SINGAL_CANCEI    0x00
//#define   SINGAL_PITCH_ON     0x01
//#define   SINGAL_MOVE_START       0x02
//#define   SINGAL_MOVE_STOP       0x03
//typedef struct _variant_mainlogicCMD1{
//typedef struct _variant_controldedCMD1{
//	u8 type;		
//	u8 direction;		//单动方向
//	u8 movetype;		//单动动作类型
//	u8 num;		//  控制到第几架单动
//	u8 back_Flag;  //
//	u8 moveCMD;		//单动动作命令码
//	u8 nc1;
//	u8 nc2;
//}_variant_controldedCMD1;
//}_variant_mainlogicCMD1
//#define   URGENCY_STOP    0x00
//#define   NORMOL_IDLE     0x01
//#define   AUTO_IDLE       0x02
//#define   SETTING       0x03
//#define   CONTROLED       0x04
//#define   FREEZE       0x05
//#define   MOVE       0x06
//#define   URGENCY_LOCK       0x0a
/*******************_variant_Mainlogic_singleCMD.type用，为单动动作类型******************/
//#define   TPYE_SINGAL    0x01
//#define   TPYE_GROUP   0x02
//*****************************************************************************************


struct _variant_controldedCMD1 Controled_singalCMD;
struct _variant_controldedCMD2 Controled_GroupCMD;
union _unit_Controled_CMDR uintCMD;
u8 array_Controled_Reply[8]={0};
u8 const_Controled_InterfaceFlag =1;
/********************************确定是否动作、动作是否一致、动作是否继续的标志位，非常重要**********************************/
u8 state_Controled_MoveFlag = CONTROLED_MOVE_OFF;
u8 state_Controled_ContiueFlag = 0;
u8 variant_Controled_Movevaluecheak =0;//查看动作号是否一致，防止误动作
uint32_t Time_Controled_SingalmoveCheak;
u8 count_Controled_D_R;
u32 time_controled_BOTTOM=0;
u32 test_time =0;
u8 bottomFlag = 0;




/********************************添加人任晓宇 添加日期2023/12/24**********************************/
uint8_t state_controlded_movetype;//动作来源  00,临架,01操作台单动,02跟机    添加人任晓宇 添加日期2023/12/24
uint8_t upper_message[8];
uint8_t up_send_flag;

/********************************确定是否动作、动作是否一致、动作是否继续的标志位，非常重要**********************************/


void CMD_Control_Faction(void)
{
	if(xQueueReceive(Unite_Action_Queue,&uintCMD.arrayCMD,10000) == pdFAIL)
	  {
		return;
	  }
	switch(uintCMD.Controled_singalCMD.type)
	{
	
		case TPYE_SINGAL:
			if(uintCMD.Controled_singalCMD.num  == 1)
			{
				switch(uintCMD.Controled_singalCMD.movetype)
				{
					
					case SINGAL_CANCEI:
							State_main_TrumpetStatus = 0;
							LED_Management_Fanction(LED_NORMOL);
							if(_variant_Mainlogic_statement.PRESENTstate_value == CONTROLED)
							{
									

								/********************************改变设备受控状态---极为重要，本任务使用**********************************/
								_variant_Mainlogic_statement.PRESENTstate_value = State_Mainlogic_DWFAULTstate;
								_variant_Interface_disp.type = TYPE_DEFAULT;
								/********************************改变设备受控状态---极为重要，本任务使用**********************************/

								xQueueSend(Interface_Mian_Logic_Queue,&const_Controled_InterfaceFlag,0);
							}
						break;
					case SINGAL_PITCH_ON:
							if(_variant_Mainlogic_statement.PRESENTstate_value == State_Mainlogic_DWFAULTstate)//默认状态下改变设备状态为被控
							{
								State_main_TrumpetStatus = 2;
								LED_Management_Fanction(LED_BLINK);
								/********************************改变设备受控状态---极为重要，本任务使用**********************************/
								_variant_Mainlogic_statement.PRESENTstate_value = CONTROLED;
								_variant_Interface_disp.type = TYPE_DEFAULT;
			                    /********************************改变设备受控状态---极为重要，本任务使用**********************************/								
								xQueueSend(Interface_Mian_Logic_Queue,&const_Controled_InterfaceFlag,0);						
							}
							
							if(uintCMD.Controled_singalCMD.back_Flag == 0x01)
							{
									array_Controled_Reply[0] = TPYE_SINGAL;
									array_Controled_Reply[1] = uintCMD.Controled_singalCMD.direction;
									array_Controled_Reply[2] = 0;
									array_Controled_Reply[3] = 1;
									array_Controled_Reply[4] = 0;
									array_Controled_Reply[5] = 0;
									xQueueSend(CAN_Send_Queue,&array_Controled_Reply,0);//取消上一台支架的控制状态		
							}
							
						/********************************改变设备状态**********************************/
						break;
					case SINGAL_MOVE_START:
						if(_variant_Mainlogic_statement.PRESENTstate_value == CONTROLED)
						{
							if(state_Controled_MoveFlag == CONTROLED_MOVE_OFF)
							{
								state_Controled_MoveFlag = CONTROLED_MOVE_ON;
								variant_Controled_Movevaluecheak = uintCMD.Controled_singalCMD.moveCMD;
								state_Controled_ContiueFlag = CONTROLED_MOVE_CONTINUE;  //STOP不在此函数中出现，而是在检测函数中出现
								_variant_Interface_disp.type = TYPE_SINGLE_MOVEED;
								Time_Controled_SingalmoveCheak = xTaskGetTickCount();
								variant_Controled_MoveCMD = 0;
								switch(uintCMD.Controled_singalCMD.moveCMD)
								{
		
									case MOVE_COLUMN_UP://升立柱
																		   //0123456789abcdef/
										strcpy(variant_interface_SingalMove,"     升立柱     ");
										variant_Controled_MoveCMD = MOVE_COLUMN_UP_OUT;
										break;
									case MOVE_COLUMN_DOWN: //降立柱
										strcpy(variant_interface_SingalMove,"     降立柱     ");
									variant_Controled_MoveCMD = MOVE_COLUMN_DOWN_OUT;
										break;	
										case MOVE_HYDRAULICFRAME_ADVANCING: //移架
											strcpy(variant_interface_SingalMove,"      移架      ");
										variant_Controled_MoveCMD = MOVE_HYDRAULICFRAME_ADVANCING_OUT;
									break;
										case MOVE_CHUTE_PUSH:  //推溜
											strcpy(variant_interface_SingalMove,"      推溜      ");
										variant_Controled_MoveCMD = MOVE_CHUTE_PUSH_OUT;
									break;
										case MOVE_BOTTOM_UP: //抬底
											strcpy(variant_interface_SingalMove,"      抬底      ");
										variant_Controled_MoveCMD = MOVE_BOTTOM_UP_OUT;
									break;
										case MOVE_UPLIFT_BOTTOM_COLUMN_DOWN:  //抬底降柱
											strcpy(variant_interface_SingalMove,"    抬底降柱    ");
										variant_Controled_MoveCMD = MOVE_COLUMN_DOWN_OUT|MOVE_BOTTOM_UP_OUT;
									break;
										case MOVE_UPLIFT_BOTTOM_HYDRAULIC_ADVANCING: //抬底移架
											strcpy(variant_interface_SingalMove,"    抬底移架    ");
											variant_Controled_MoveCMD = MOVE_BOTTOM_UP_OUT;
											time_controled_BOTTOM =  xTaskGetTickCount();
											bottomFlag = 1;
									break;
										case MOVE_BALANCE_BEAM_STRETCH: //伸平衡梁
											strcpy(variant_interface_SingalMove,"    伸平衡梁    ");
										variant_Controled_MoveCMD = MOVE_BALANCE_BEAM_STRETCH_OUT;
									break;
										case MOVE_BALANCE_BEAM_WITHDRAW://收平衡梁
											strcpy(variant_interface_SingalMove,"    收平衡梁    ");
										variant_Controled_MoveCMD = MOVE_BALANCE_BEAM_WITHDRAW_OUT;
									break;
										case MOVE_SIDE_PLATE_STRETCH://伸侧推
											strcpy(variant_interface_SingalMove,"     伸侧护     ");
										variant_Controled_MoveCMD = MOVE_SIDE_PLATE_STRETCH_OUT;
									break;
										case MOVE_SIDE_PLATE_WITHDRAW:    //收侧推
											strcpy(variant_interface_SingalMove,"     收侧护     ");
										variant_Controled_MoveCMD = MOVE_SIDE_PLATE_WITHDRAW_OUT;
									break;
										case MOVE_TELESCOPIC_GIRDER_STRETCH://伸伸缩梁
											strcpy(variant_interface_SingalMove,"      伸前梁    ");
										variant_Controled_MoveCMD = MOVE_TELESCOPIC_GIRDER_STRETCH_OUT;
									break;
										case MOVE_TELESCOPIC_GIRDER_WITHDRAW:  //收伸缩梁
											strcpy(variant_interface_SingalMove,"      收前梁    ");
										variant_Controled_MoveCMD = MOVE_TELESCOPIC_GIRDER_WITHDRAW_OUT;
									break;
										case MOVE_FUEL_SPRAY:  //喷雾
											strcpy(variant_interface_SingalMove,"      喷雾      ");
										variant_Controled_MoveCMD = MOVE_FUEL_SPRAY_OUT;
									break;
										case MOVE_PLATE_STRETCH: //伸护帮板
											strcpy(variant_interface_SingalMove,"    伸护帮板    ");
										variant_Controled_MoveCMD = MOVE_PLATE_STRETCH_OUT;
									break;
										case MOVE_PLATE_WITHDRAW: //收护帮板
											strcpy(variant_interface_SingalMove,"    收护帮板    ");
										variant_Controled_MoveCMD = MOVE_PLATE_WITHDRAW_OUT;
									break;
										case MOVE_HYDRAULIC_BOTTOM_STRETCH:  //伸底调
											strcpy(variant_interface_SingalMove,"     伸底调     ");
										variant_Controled_MoveCMD = MOVE_HYDRAULIC_BOTTOM_STRETCH_OUT;
									break;
										case MOVE_HYDRAULIC_BOTTOM_WITHDRAW:   //收底调
											strcpy(variant_interface_SingalMove,"     收底调     ");
										variant_Controled_MoveCMD = MOVE_HYDRAULIC_BOTTOM_WITHDRAW_OUT;
									break;
//									case MOVE_DROP_AD_UP: //降移升
//											strcpy(variant_interface_SingalMove,"     降移升     ");

//									break;
									
									case MOVE_STANDBY_APPLICATION1: //备用动作1
											strcpy(variant_interface_SingalMove,"    备用动作1   ");
											variant_Controled_MoveCMD = MOVE_STANDBY_APPLICATION1_OUT;
									break;
									
									case MOVE_STANDBY_APPLICATION2: //备用动作2
											strcpy(variant_interface_SingalMove,"    备用动作2   ");
											variant_Controled_MoveCMD = MOVE_STANDBY_APPLICATION2_OUT;
									break;

										case MOVE_BACK_WASH2: //反冲洗2
											strcpy(variant_interface_SingalMove,"     反冲洗1     ");
											OPEN_MCZ33996_OUT(OUT1);

									break;
										
										case MOVE_BACK_WASH1: //反冲洗1
											strcpy(variant_interface_SingalMove,"     反冲洗2     ");
											OPEN_MCZ33996_OUT(OUT0);

									break;
										
									default:
									break;
								
								}
								MOVE_controled_Driver(variant_Controled_MoveCMD);
								strcpy(variant_interface_Dive,"  ");
								count_Controled_D_R = 1;
								xQueueSend(Interface_Mian_Logic_Queue,&const_Controled_InterfaceFlag,0);
								
								if(up_send_flag == 0)//动作上报   添加人任晓宇 添加日期2023/12/24
								{
								  up_send_flag = 1;
								  upper_message[0] = 0x21;
								  upper_message[1] = Structset_Service_DeviceNum1_1.Setting_Para;
								  upper_message[2] = uintCMD.Controled_singalCMD.moveCMD;
								  upper_message[3] = 0x01;
								  upper_message[4] = state_controlded_movetype;								  
								  CANx_SendNormalData(&BUS_CAN, 0xee, upper_message, 8);

								}
															
								
							}
							else if(state_Controled_MoveFlag == CONTROLED_MOVE_ON)
							{
									if(variant_Controled_Movevaluecheak == uintCMD.Controled_singalCMD.moveCMD)
									{
										state_Controled_ContiueFlag = CONTROLED_MOVE_CONTINUE; 
										test_time = xTaskGetTickCount()-time_controled_BOTTOM;
										if((test_time>=1500)&&(variant_Controled_Movevaluecheak==MOVE_UPLIFT_BOTTOM_HYDRAULIC_ADVANCING)&&(bottomFlag == 1))
										{
											variant_Controled_MoveCMD = MOVE_HYDRAULICFRAME_ADVANCING_OUT|MOVE_BOTTOM_UP_OUT;
											bottomFlag = 0;
										}
										MOVE_controled_Driver(variant_Controled_MoveCMD);
										
										if((variant_Controled_driveR[0] != 0)||(variant_Controled_driveR[3] != 0)||(variant_Controled_driveR[7] != 0)||(variant_Controled_driveR[8] != 0))
										{
											Flag_Controled_driveR = 0;
											variant_interface_Dive[0] = 0xD5;
											variant_interface_Dive[1] = 0xFD;
											variant_interface_Dive[2] = 0xB3;
											variant_interface_Dive[3] = 0xA3;
											variant_interface_Dive[4] = 0x20;
											variant_interface_Dive[5] = 0x20;
											variant_interface_Dive[6] = '\0';
											memset(variant_Controled_driveR,0,10);
										}									
										else if((variant_Controled_driveR[0] == 0)&&(variant_Controled_driveR[3] == 0)&&(variant_Controled_driveR[7] == 0)&&(variant_Controled_driveR[8] == 0))
										{
											if(count_Controled_D_R<=5)
											{
											  count_Controled_D_R++;
												
											}
											else 
											{
												count_Controled_D_R = 1;

											}
									
										
										}
										
										
									}
									xQueueSend(Interface_Mian_Logic_Queue,&const_Controled_InterfaceFlag,0);
							
							}
						}	
						break;
					default:
						break;
				}
			
			}
			else if(uintCMD.Controled_singalCMD.num  != 1)
			{
				switch(uintCMD.Controled_singalCMD.movetype)
				{
					case SINGAL_CANCEI:
							uintCMD.Controled_singalCMD.num = uintCMD.Controled_singalCMD.num - 1;
							xQueueSend(CAN_Send_Queue,&uintCMD.arrayCMD,0); 
             if(((_variant_Mainlogic_statement.PRESENTstate_value == CONTROLED)&&(state_Controled_MoveFlag == CONTROLED_MOVE_OFF)&&(_variant_Interface_disp.type == TYPE_DEFAULT)))	
							{
								/********************************改变设备受控状态---极为重要，本任务使用**********************************/
								_variant_Mainlogic_statement.PRESENTstate_value = State_Mainlogic_DWFAULTstate;
								/********************************改变设备受控状态---极为重要，本任务使用**********************************/
								xQueueSend(Interface_Mian_Logic_Queue,&const_Controled_InterfaceFlag,0);						
							}								
					break;	
 
					case SINGAL_PITCH_ON:
							State_main_TrumpetStatus = 0;
							LED_Management_Fanction(LED_BLINK);
							if((_variant_Mainlogic_statement.PRESENTstate_value == CONTROLED)&&(state_Controled_MoveFlag == CONTROLED_MOVE_OFF))
							{
								/********************************改变设备受控状态---极为重要，本任务使用**********************************/
								_variant_Mainlogic_statement.PRESENTstate_value = State_Mainlogic_DWFAULTstate;
								_variant_Interface_disp.type = TYPE_DEFAULT;
								/********************************改变设备受控状态---极为重要，本任务使用**********************************/
								xQueueSend(Interface_Mian_Logic_Queue,&const_Controled_InterfaceFlag,0);
							}
							uintCMD.Controled_singalCMD.num = uintCMD.Controled_singalCMD.num - 1;
							xQueueSend(CAN_Send_Queue,&uintCMD.arrayCMD,0); 					
					break;	

					case SINGAL_MOVE_START:
							uintCMD.Controled_singalCMD.num = uintCMD.Controled_singalCMD.num - 1;
							xQueueSend(CAN_Send_Queue,&uintCMD.arrayCMD,0); 
					break;	
						
					default:
					break;	
						
				}
			
			}
			

			break;
		case TPYE_GROUP_ALARM:
			if((uintCMD.Controled_GroupCMD.direction1 == DIRECTION_L_LEFT)||(uintCMD.Controled_GroupCMD.direction1 == DIRECTION_R_RIGHT))
			{
				if(((uintCMD.Controled_GroupCMD.limit5>=Structset_Service_DeviceNum1_1.Setting_Para)&&((uintCMD.Controled_GroupCMD.direction1&0xf)==Structset_GlobalSet_Increase4_4.Setting_Para))||
				((uintCMD.Controled_GroupCMD.limit5<=Structset_Service_DeviceNum1_1.Setting_Para)&&((uintCMD.Controled_GroupCMD.direction1&0xf)!=Structset_GlobalSet_Increase4_4.Setting_Para)))
				{
					
					
						uintCMD.Controled_GroupCMD.alarmnum2 = uintCMD.Controled_GroupCMD.alarmnum2-1;//报警号减一，激活警报
						if(uintCMD.Controled_GroupCMD.alarmnum2 >= 1)//不为零
						{
								
								xQueueSend(CAN_Send_Queue,&uintCMD.arrayCMD,0);
						}
						State_main_TrumpetStatus = 2;
						/********************************改变设备受控状态---极为重要，本任务使用**********************************/
						_variant_Mainlogic_statement.PRESENTstate_value = GROUP_CONTROLED;
						/********************************改变设备受控状态---极为重要，本任务使用**********************************/
						_variant_Interface_disp.type = TYPE_GROUP_ALARM;					
						xQueueSend(Interface_Mian_Logic_Queue,&const_Controled_InterfaceFlag,0);
						
						
						if((uintCMD.Controled_GroupCMD.alarmnum2+1) == uintCMD.Controled_GroupCMD.movenum3)
						{
							_variant_Interface_disp.type = TYPE_GROUP_MOVE;					
							_variant_Mainlogic_statement.PRESENTstate_value = MOVE;
							xQueueSend(Interface_Mian_Logic_Queue,&const_Controled_InterfaceFlag,0);
							Group_Move_Control(uintCMD.Controled_GroupCMD.moveCMD4);

							if(_variant_Mainlogic_statement.PRESENTstate_value == MOVE)
							{
								uintCMD.Controled_GroupCMD.movenum3 = uintCMD.Controled_GroupCMD.movenum3 - 1;
								uintCMD.Controled_GroupCMD.type0 = TPYE_GROUP;
								if(uintCMD.Controled_GroupCMD.movenum3 >= 1)
								{
									xQueueSend(CAN_Send_Queue,&uintCMD.arrayCMD,0);									
								}
								
							}
							variant_Mainlogic_KeyValue = 0;//不要进入下面的switch中的任何一个case
							/********************************改变设备状态---极为重要，本任务只有一对**********************************/
							_variant_Mainlogic_statement.PRESENTstate_value = State_Mainlogic_DWFAULTstate;
							/********************************改变设备状态---极为重要，本任务只有一对**********************************/
							_variant_Interface_disp.type = TYPE_DEFAULT;
							State_main_TrumpetStatus = 0;
							xQueueSend(Interface_Mian_Logic_Queue,&const_Mainlogic_InterfaceFlag,0);
							
							
							
							
						}
				}
				
			}
			else if((uintCMD.Controled_GroupCMD.direction1 == DIRECTION_R_LEFT)||(uintCMD.Controled_GroupCMD.direction1 == DIRECTION_L_RIGHT))
			{
			
				if(((uintCMD.Controled_GroupCMD.limit5>=Structset_Service_DeviceNum1_1.Setting_Para)&&((uintCMD.Controled_GroupCMD.direction1&0xf)==Structset_GlobalSet_Increase4_4.Setting_Para))||
				((uintCMD.Controled_GroupCMD.limit5<=Structset_Service_DeviceNum1_1.Setting_Para)&&((uintCMD.Controled_GroupCMD.direction1&0xf)!=Structset_GlobalSet_Increase4_4.Setting_Para)))
				{
					uintCMD.Controled_GroupCMD.alarmnum2 = uintCMD.Controled_GroupCMD.alarmnum2-1;//报警号减一，激活警报
					if(uintCMD.Controled_GroupCMD.alarmnum2 >= 1)//不为零
					{
							
							xQueueSend(CAN_Send_Queue,&uintCMD.arrayCMD,0);
					}
					State_main_TrumpetStatus = 2;
					/********************************改变设备受控状态---极为重要，本任务使用**********************************/
					_variant_Mainlogic_statement.PRESENTstate_value = GROUP_CONTROLED;
					/********************************改变设备受控状态---极为重要，本任务使用**********************************/
					_variant_Interface_disp.type = TYPE_GROUP_ALARM;					
					xQueueSend(Interface_Mian_Logic_Queue,&const_Controled_InterfaceFlag,0);
					
					if(uintCMD.Controled_GroupCMD.alarmnum2 == 0)
					{
						_variant_Interface_disp.type = TYPE_GROUP_MOVE;					
						
						_variant_Mainlogic_statement.PRESENTstate_value = MOVE;
						xQueueSend(Interface_Mian_Logic_Queue,&const_Controled_InterfaceFlag,0);
						Group_Move_Control(uintCMD.Controled_GroupCMD.moveCMD4);
						uintCMD.Controled_GroupCMD.movenum3 = uintCMD.Controled_GroupCMD.movenum3 - 1;
						uintCMD.Controled_GroupCMD.type0 = TPYE_GROUP;
						
						if(uintCMD.Controled_GroupCMD.movenum3 >= 1)
						{
							xQueueSend(CAN_Send_Queue,&uintCMD.arrayCMD,0);	
						}
						variant_Mainlogic_KeyValue = 0;//不要进入下面的switch中的任何一个case
						/********************************改变设备状态---极为重要，本任务只有一对**********************************/
						_variant_Mainlogic_statement.PRESENTstate_value = State_Mainlogic_DWFAULTstate;
						/********************************改变设备状态---极为重要，本任务只有一对**********************************/
						_variant_Interface_disp.type = TYPE_DEFAULT;
						State_main_TrumpetStatus = 0;
						xQueueSend(Interface_Mian_Logic_Queue,&const_Mainlogic_InterfaceFlag,0);
					}
				}
				else
				{
					uintCMD.Controled_GroupCMD.movenum3 = uintCMD.Controled_GroupCMD.movenum3 - uintCMD.Controled_GroupCMD.alarmnum2;
					uintCMD.Controled_GroupCMD.alarmnum2 = 0;
					uintCMD.Controled_GroupCMD.type0 = TPYE_GROUP;
					xQueueSend(CAN_Send_Queue,&uintCMD.arrayCMD,0);					
				}
	
			
			}
			break;	
	
		case TPYE_GROUP:
				switch(uintCMD.Controled_GroupCMD.moveCMD4)
				{
					case MOVE_GROUP_ADVANCING:
					case MOVE_GROUPTELESCOPICPLATE_GIRDER_WITHDRAW://收伸缩梁护帮板
						if(_variant_Mainlogic_statement.PRESENTstate_value == GROUP_CONTROLED)
						{
							_variant_Interface_disp.type = TYPE_GROUP_MOVE;					
							_variant_Mainlogic_statement.PRESENTstate_value = MOVE;
							xQueueSend(Interface_Mian_Logic_Queue,&const_Controled_InterfaceFlag,0);
							Group_Move_Control(uintCMD.Controled_GroupCMD.moveCMD4);

							if(_variant_Mainlogic_statement.PRESENTstate_value == MOVE)
							{
								uintCMD.Controled_GroupCMD.movenum3 = uintCMD.Controled_GroupCMD.movenum3 - 1;
								uintCMD.Controled_GroupCMD.type0 = TPYE_GROUP;
								if(uintCMD.Controled_GroupCMD.movenum3 >= 1)
								{
									xQueueSend(CAN_Send_Queue,&uintCMD.arrayCMD,0);									
								}
								
							}
							variant_Mainlogic_KeyValue = 0;//不要进入下面的switch中的任何一个case
								/********************************改变设备状态---极为重要，本任务只有一对**********************************/
								_variant_Mainlogic_statement.PRESENTstate_value = State_Mainlogic_DWFAULTstate;
								/********************************改变设备状态---极为重要，本任务只有一对**********************************/
								_variant_Interface_disp.type = TYPE_DEFAULT;

								State_main_TrumpetStatus = 0;
								xQueueSend(Interface_Mian_Logic_Queue,&const_Mainlogic_InterfaceFlag,0);
						}
					break;
							
							
					case MOVE_GROUP_CHUTE_PUSH:
					case MOVE_GROUP_CHUTE_PULL:	
					case MOVE_GROUP_TELESCOPIC_GIRDER_STRETCH:		
					case MOVE_GROUP_PLATE_STRETCH:
					case MOVE_GROUP_PLATE_WITHDRAW:						
							if(_variant_Mainlogic_statement.PRESENTstate_value == State_Mainlogic_DWFAULTstate)
							{
								if(((uintCMD.Controled_GroupCMD.limit5>=Structset_Service_DeviceNum1_1.Setting_Para)&&((uintCMD.Controled_GroupCMD.direction1&0xf)==Structset_GlobalSet_Increase4_4.Setting_Para))||
								((uintCMD.Controled_GroupCMD.limit5<=Structset_Service_DeviceNum1_1.Setting_Para)&&((uintCMD.Controled_GroupCMD.direction1&0xf)!=Structset_GlobalSet_Increase4_4.Setting_Para)))
								{
									uintCMD.Controled_GroupCMD.movenum3 = uintCMD.Controled_GroupCMD.movenum3-1;
									if(uintCMD.Controled_GroupCMD.movenum3 >= 1)
									{
//										
										xQueueSend(CAN_Send_Queue,&uintCMD.arrayCMD,0); 
									}
									State_main_TrumpetStatus = 2;
									/********************************改变设备受控状态---极为重要，本任务使用**********************************/
									_variant_Mainlogic_statement.PRESENTstate_value = MOVE;
									_variant_Interface_disp.type = TYPE_GROUP_MOVE;
									/********************************改变设备受控状态---极为重要，本任务使用**********************************/								
									xQueueSend(Interface_Mian_Logic_Queue,&const_Controled_InterfaceFlag,0);				
									
									Group_Move_Control(uintCMD.Controled_GroupCMD.moveCMD4);
									
									variant_Mainlogic_KeyValue = 0;//不要进入下面的switch中的任何一个case

									/********************************改变设备状态---极为重要，本任务只有一对**********************************/
									_variant_Mainlogic_statement.PRESENTstate_value = State_Mainlogic_DWFAULTstate;
									/********************************改变设备状态---极为重要，本任务只有一对**********************************/
									_variant_Interface_disp.type = TYPE_DEFAULT;
									State_main_TrumpetStatus = 0;
									xQueueSend(Interface_Mian_Logic_Queue,&const_Mainlogic_InterfaceFlag,0);\
								}
							}
					
					break;	

					default:
						
						break;
				}
		
		
		
			break;	
	
	


		default:
			break;
	}
}








void Security_controled_Single(void)
{ 
  upper_message[0] = 0x21;                                            //上报标志位   添加人任晓宇 添加日期2023/12/24
  upper_message[1] = Structset_Service_DeviceNum1_1.Setting_Para;     //本机架号   添加人任晓宇 添加日期2023/12/24

				if(state_Controled_MoveFlag == CONTROLED_MOVE_ON)
				{
					if(xTaskGetTickCount()-Time_Controled_SingalmoveCheak>=200)
					{
						Time_Controled_SingalmoveCheak = xTaskGetTickCount();
						if(state_Controled_ContiueFlag == CONTROLED_MOVE_CONTINUE)
						{
							state_Controled_ContiueFlag = CONTROLED_MOVE_STOP;//动作持续标志位						
						}
						else if(state_Controled_ContiueFlag == CONTROLED_MOVE_STOP)
						{
								OPEN_MCZ33996_OUT(0x0000);
								state_Controled_MoveFlag = CONTROLED_MOVE_OFF;//动作标志位 1正在动作 0不动作
								_variant_Interface_disp.type = TYPE_DEFAULT; //界面显示切回默认
								if(variant_BUS_continue == 1)
								{
									variant_BUS_continue = 0;
									_variant_Mainlogic_statement.PRESENTstate_value = State_Mainlogic_DWFAULTstate;
									State_main_TrumpetStatus = 0;
									LED_Management_Fanction(LED_NORMOL);
								
								}
								xQueueSend(Interface_Mian_Logic_Queue,&const_Controled_InterfaceFlag,0);
								
								if(up_send_flag == 1)
								{								  
								  up_send_flag = 0;								  
								  state_controlded_movetype = 0;//上报动作控制来源								  
								  upper_message[0] = 0x21;                                              //上报标志位
								  upper_message[1] = Structset_Service_DeviceNum1_1.Setting_Para;       //本机架号																										//[2]沿用之前的标志位
								  upper_message[3] = 0x00;                                              //结束标志位
								  upper_message[4] = state_controlded_movetype;                                   //总线/临架标志位								  
								  CANx_SendNormalData(&BUS_CAN, 0xee, upper_message, 8);
								}
								
						}
	
					}
				}

}


































