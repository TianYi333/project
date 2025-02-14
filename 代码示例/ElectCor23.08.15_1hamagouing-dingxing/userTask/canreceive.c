#include "canreceive.h"
#include "main.h"
#include "main_logic.h"
#include "parameter_operation.h"
#include "bsp_oled19264.h"
#include "controlded.h"
#include "interface.h"
#include "set.h"
#include "bsp_ncv7356.h"
#include "cansend.h"
#include "drv_usart.h"
#include "interface.h"
#include "sensor.h"
#include "follow.h"
#include "controlded.h"
#include "bsp_hd7279.h"
#include "follow.h"
/**********************************************数组定义*******************************************************/
uint8_t array_Canreceive_CanLRcmdR[8];//邻架接收 传递命令变量  创建人蒋博 创建日期2023/02/3
uint8_t array_Canreceive_CanBUScmdR[8];//总线接受传递命令变量  创建人蒋博 创建日期2023/02/3
uint8_t array_Canreceive_CanLRinnerT[8];//邻架内部发送传递命令变量  创建人蒋博 创建日期2023/02/3
uint8_t array_Canreceive_CanBUSinnerT[8];//总线内部发送    传递命令变量  创建人蒋博 创建日期2023/02/3

uint8_t array_Canreceive_CanBUSreturnT[8];//总线内部发送    传递命令变量  创建人蒋博 创建日期2023/02/3

uint8_t variant_BUS_continue = 0;//反控动作    传递命令变量  创建人蒋博 创建日期2023/04/18
uint8_t variant_Canreceive_STATE = 0x10;//控制器状态   传递命令变量  添加人任晓宇 添加日期2023/12/24
//struct _variant_BUScheak1 Canreceive_cheakM;


void LRcan_Reveice_Fuction(void)
{
	uint8_t const_Controled_InterfaceFlag = 1;
	uint8_t array_Canreceive_CanLRreturnT[8];//邻架内部发送传递命令变量  创建人蒋博 创建日期2023/02/3
	switch(array_Canreceive_CanLRcmdR[0])
	{
		case	TPYE_SINGAL:
			state_controlded_movetype = 0;//邻架单动
//			if(_variant_Mainlogic_statement.PRESENTstate_value == State_Mainlogic_DWFAULTstate)
			xQueueSend(Unite_Action_Queue,&array_Canreceive_CanLRcmdR,0);			
             break;

		case	TPYE_GROUP:
		case	TPYE_GROUP_ALARM:
			if(array_Canreceive_CanLRcmdR[6] == 0)
			{
				xQueueSend(Unite_Action_Queue,&array_Canreceive_CanLRcmdR,0);	
			}
			else if(array_Canreceive_CanLRcmdR[6] > 0)
			{
				array_Canreceive_CanLRcmdR[6]=array_Canreceive_CanLRcmdR[6]-1;
				xQueueSend(CAN_Send_Queue,&array_Canreceive_CanLRcmdR,0);
			}				
            break;
		
		case	TPYE_SET_GLOBALPAR:
			Structset_GlobalSet_SynchronizationFun_R(array_Canreceive_CanLRcmdR);
             break;
		
		case	TPYE_SERIALNUMBER_L:
				if(array_Canreceive_CanLRcmdR[1] != Structset_GlobalSet_Increase4_4.Setting_Para)
				{
					 Structset_GlobalSet_Increase4_4.Setting_Para = array_Canreceive_CanLRcmdR[1];//若地址增向与上级控制器不一致，则进行更改
					Set_Para_to_char(Structset_GlobalSet_Increase4_4.Setting_Para,Structset_GlobalSet_Increase4_4.veiw_type,Structset_GlobalSet_Increase4_4.Setting_Char);
				}
				
				if(Structset_GlobalSet_Increase4_4.Setting_Para  == DIRECTION_LEFT)
				{
					Structset_Service_DeviceNum1_1.Setting_Para = array_Canreceive_CanLRcmdR[2] + 1;
					if(Structset_Service_DeviceNum1_1.Setting_Para >= Structset_GlobalSet_MaxDeviceNum1_1.Setting_Para)
					{
						Structset_Service_DeviceNum1_1.Setting_Para = Structset_GlobalSet_MaxDeviceNum1_1.Setting_Para;
					}
				}
				else if(Structset_GlobalSet_Increase4_4.Setting_Para  == DIRECTION_RIGHT)
				{
					Structset_Service_DeviceNum1_1.Setting_Para = array_Canreceive_CanLRcmdR[2] - 1;
					if(Structset_Service_DeviceNum1_1.Setting_Para == 0)
					{
						Structset_Service_DeviceNum1_1.Setting_Para = 0x01;
					}
				}
				Set_Para_to_char(Structset_Service_DeviceNum1_1.Setting_Para,Structset_Service_DeviceNum1_1.veiw_type,Structset_Service_DeviceNum1_1.Setting_Char);
				array_Canreceive_CanLRcmdR[2] = Structset_Service_DeviceNum1_1.Setting_Para;
				CANx_SendNormalData(&LEFT_CAN,COMMUN_ID,array_Canreceive_CanLRcmdR,8);		
				variant_Service_DeviceNum = Structset_Service_DeviceNum1_1.Setting_Para;

				DeviceNum[0] = (variant_Service_DeviceNum/100)+48;
				DeviceNum[1] = ((variant_Service_DeviceNum%100)/10)+48;
				DeviceNum[2] = (variant_Service_DeviceNum%10)+48;
				DeviceNum[3] = '\0';
				SaveParaToFlash_WritingFun();
				vTaskDelay(10);
				 CAN_Config();
				vTaskDelay(10);
             break;
			case	TPYE_SERIALNUMBER_R:
				if(array_Canreceive_CanLRcmdR[1] != Structset_GlobalSet_Increase4_4.Setting_Para)
				{
					 Structset_GlobalSet_Increase4_4.Setting_Para = array_Canreceive_CanLRcmdR[1];//若地址增向与上级控制器不一致，则进行更改
					Set_Para_to_char(Structset_GlobalSet_Increase4_4.Setting_Para,Structset_GlobalSet_Increase4_4.veiw_type,Structset_GlobalSet_Increase4_4.Setting_Char);
				}
				
				if(Structset_GlobalSet_Increase4_4.Setting_Para  == DIRECTION_LEFT)
				{
					Structset_Service_DeviceNum1_1.Setting_Para = array_Canreceive_CanLRcmdR[2] - 1;
					if(Structset_Service_DeviceNum1_1.Setting_Para == 0)
					{
						Structset_Service_DeviceNum1_1.Setting_Para = 0x01;
					}
				}
				else if(Structset_GlobalSet_Increase4_4.Setting_Para  == DIRECTION_RIGHT)
				{
					Structset_Service_DeviceNum1_1.Setting_Para = array_Canreceive_CanLRcmdR[2] + 1;
					if(Structset_Service_DeviceNum1_1.Setting_Para >= Structset_GlobalSet_MaxDeviceNum1_1.Setting_Para)
					{
						Structset_Service_DeviceNum1_1.Setting_Para = Structset_GlobalSet_MaxDeviceNum1_1.Setting_Para;
					}
				}
				Set_Para_to_char(Structset_Service_DeviceNum1_1.Setting_Para,Structset_Service_DeviceNum1_1.veiw_type,Structset_Service_DeviceNum1_1.Setting_Char);
				array_Canreceive_CanLRcmdR[2] = Structset_Service_DeviceNum1_1.Setting_Para;
				CANx_SendNormalData(&RIGHT_CAN,COMMUN_ID,array_Canreceive_CanLRcmdR,8);		
				variant_Service_DeviceNum = Structset_Service_DeviceNum1_1.Setting_Para;

				DeviceNum[0] = (variant_Service_DeviceNum/100)+48;
				DeviceNum[1] = ((variant_Service_DeviceNum%100)/10)+48;
				DeviceNum[2] = (variant_Service_DeviceNum%10)+48;
				DeviceNum[3] = '\0';
				SaveParaToFlash_WritingFun();
				vTaskDelay(10);
				 CAN_Config();
				vTaskDelay(10);
				break;

		case	TPYE_PARAMETER_L:
		case	TPYE_PARAMETER_R:
			xQueueSend(CAN_Send_Queue,&array_Canreceive_CanLRcmdR,0);
			Parameter_delivery_R(array_Canreceive_CanLRcmdR);
             break;
		case     PITCH_ON:
				if(array_Canreceive_CanLRcmdR[1]>Structset_Service_DeviceNum1_1.Setting_Para)
				{
		
					xQueueSend(CAN_Send_Queue,&array_Canreceive_CanLRcmdR,0);
					if((_variant_Mainlogic_statement.PRESENTstate_value == CONTROLED)&&(FLAG_drvusart_romatecontrol == 1))
					{
						FLAG_drvusart_romatecontrol = 0;
						State_main_TrumpetStatus = 1;
						LED_Management_Fanction(LED_NORMOL);
						/********************************改变设备受控状态---极为重要，本任务使用**********************************/
						_variant_Mainlogic_statement.PRESENTstate_value = State_Mainlogic_DWFAULTstate;
						_variant_Interface_disp.type = TYPE_DEFAULT;
						/********************************改变设备受控状态---极为重要，本任务使用**********************************/								
						xQueueSend(Interface_Mian_Logic_Queue,&const_Controled_InterfaceFlag,0);
					}
				}
				else if(array_Canreceive_CanLRcmdR[1] == Structset_Service_DeviceNum1_1.Setting_Para)
				{
						if(_variant_Mainlogic_statement.PRESENTstate_value == State_Mainlogic_DWFAULTstate)
						{
							FLAG_drvusart_romatecontrol = 1;//该电液控直接受控
							State_main_TrumpetStatus = 2;
								
							LED_Management_Fanction(LED_NORMOL);
							/********************************改变设备受控状态---极为重要，本任务使用**********************************/

								_variant_Mainlogic_statement.PRESENTstate_value = CONTROLED;
							
								_variant_Interface_disp.type = TYPE_DEFAULT;
							/********************************改变设备受控状态---极为重要，本任务使用**********************************/	
							

							
							xQueueSend(Interface_Mian_Logic_Queue,&const_Controled_InterfaceFlag,0);								

							
						}
						
						array_Canreceive_CanBUSreturnT[0] = PLICHRETURN_1;
						array_Canreceive_CanBUSreturnT[1] = variant_Service_DeviceNum;
						array_Canreceive_CanBUSreturnT[2] = 0x01;
						array_Canreceive_CanBUSreturnT[3] = (_sensor_distance.sensor_value>>8)&0xff;
						array_Canreceive_CanBUSreturnT[4] = _sensor_distance.sensor_value&0xff;
						array_Canreceive_CanBUSreturnT[5] = (_sensor_pressure.sensor_value>>8)&0xff;
						array_Canreceive_CanBUSreturnT[6] = _sensor_pressure.sensor_value&0xff;
						
						xQueueSend(CAN_Send_Queue,&array_Canreceive_CanBUSreturnT,0);
						vTaskDelay(100);
						array_Canreceive_CanBUSreturnT[0] = PLICHRETURN_2;
						array_Canreceive_CanBUSreturnT[1] = variant_Service_DeviceNum;
						array_Canreceive_CanBUSreturnT[2] = _sensor_Connectingrod_angle.sensor_view[0];
						array_Canreceive_CanBUSreturnT[3] = (_sensor_Connectingrod_angle.sensor_value/100)&0xff;
						array_Canreceive_CanBUSreturnT[4] = _sensor_canopy_angle.sensor_view[0];
						array_Canreceive_CanBUSreturnT[5] = (_sensor_canopy_angle.sensor_value/100)&0xff;
						array_Canreceive_CanBUSreturnT[6] = _sensor_Plate_angle.sensor_view[0];
						array_Canreceive_CanBUSreturnT[7] = (_sensor_Plate_angle.sensor_value/100)&0xff;
				
						xQueueSend(CAN_Send_Queue,&array_Canreceive_CanBUSreturnT,0);

					}
				break;
		case	 SINGAL_MOVE:
					if(array_Canreceive_CanLRcmdR[1]>Structset_Service_DeviceNum1_1.Setting_Para)
					{
						xQueueSend(CAN_Send_Queue,&array_Canreceive_CanLRcmdR,0);
					}
					else if(array_Canreceive_CanLRcmdR[1] == Structset_Service_DeviceNum1_1.Setting_Para)
					{
						if((FLAG_drvusart_romatecontrol == 1)&&(_variant_Mainlogic_statement.PRESENTstate_value == CONTROLED))
						{
							array_Canreceive_CanBUSinnerT[0] = TPYE_SINGAL;
							array_Canreceive_CanBUSinnerT[1] = DIRECTION_ROMATE;
							array_Canreceive_CanBUSinnerT[2] = SINGAL_MOVE_START;
							array_Canreceive_CanBUSinnerT[3] = 1;
							array_Canreceive_CanBUSinnerT[4] = 0x01;
							array_Canreceive_CanBUSinnerT[5] = array_Canreceive_CanLRcmdR[2];
							array_Canreceive_CanBUSinnerT[6] = 0;
							array_Canreceive_CanBUSinnerT[7] = 0;
						 
							xQueueSend(Unite_Action_Queue,&array_Canreceive_CanBUSinnerT,0);
						}
						
						
					}
		case	 GROUP_CMD:
				if(array_Canreceive_CanLRcmdR[1]>Structset_Service_DeviceNum1_1.Setting_Para)
				{
					xQueueSend(CAN_Send_Queue,&array_Canreceive_CanLRcmdR,0);
				}
				else if(array_Canreceive_CanLRcmdR[1] == Structset_Service_DeviceNum1_1.Setting_Para)
				{					
						if((array_Canreceive_CanLRcmdR[2] == MOVE_GROUP_ADVANCING)||(array_Canreceive_CanLRcmdR[2] == MOVE_GROUPTELESCOPICPLATE_GIRDER_WITHDRAW))
						{
							array_Canreceive_CanLRreturnT[0] = TPYE_GROUP_ALARM;
							if((array_Canreceive_CanLRcmdR[1]>=array_Canreceive_CanLRcmdR[5])&&(Structset_GlobalSet_Increase4_4.Setting_Para == DIRECTION_RIGHT))
							{
								array_Canreceive_CanLRreturnT[1]=  DIRECTION_L_LEFT;
							}
							else if((array_Canreceive_CanLRcmdR[1]>=array_Canreceive_CanLRcmdR[5])&&(Structset_GlobalSet_Increase4_4.Setting_Para == DIRECTION_LEFT))
							{
								array_Canreceive_CanLRreturnT[1]=  DIRECTION_R_RIGHT;
							}
							else if((array_Canreceive_CanLRcmdR[1]<=array_Canreceive_CanLRcmdR[5])&&(Structset_GlobalSet_Increase4_4.Setting_Para == DIRECTION_RIGHT))
							{
								array_Canreceive_CanLRreturnT[1]=  DIRECTION_R_RIGHT;
							}
							else if((array_Canreceive_CanLRcmdR[1]<=array_Canreceive_CanLRcmdR[5])&&(Structset_GlobalSet_Increase4_4.Setting_Para == DIRECTION_LEFT))
							{
								array_Canreceive_CanLRreturnT[1]=  DIRECTION_L_LEFT;
							}
							
							
							array_Canreceive_CanLRreturnT[2] = array_Canreceive_CanLRcmdR[3];//伸前梁	
							array_Canreceive_CanLRreturnT[3] = array_Canreceive_CanLRcmdR[3];
							array_Canreceive_CanLRreturnT[4]= 	array_Canreceive_CanLRcmdR[2];
							array_Canreceive_CanLRreturnT[5] = array_Canreceive_CanLRcmdR[5];
							array_Canreceive_CanLRreturnT[6] = 0;
							array_Canreceive_CanLRreturnT[7] = Structset_Service_DeviceNum1_1.Setting_Para;
				
							xQueueSend(Unite_Action_Queue,&array_Canreceive_CanLRreturnT,0);
//							if((array_Canreceive_CanLRreturnT[1] == DIRECTION_L_LEFT)||(array_Canreceive_CanLRreturnT[1] == DIRECTION_R_RIGHT))
//							{
//								vTaskDelay(2);
//								array_Canreceive_CanLRreturnT[0]= TPYE_GROUP;
//								vTaskDelay(2);
//								xQueueSend(Unite_Action_Queue,&array_Canreceive_CanLRreturnT,0);
//							}
						
						
						}
						else {
								array_Canreceive_CanLRreturnT[0] = TPYE_GROUP;
								if((array_Canreceive_CanLRcmdR[1]>=array_Canreceive_CanLRcmdR[5])&&(Structset_GlobalSet_Increase4_4.Setting_Para == DIRECTION_RIGHT))
								{
									array_Canreceive_CanLRreturnT[1]=  DIRECTION_L_LEFT;
								}
								else if((array_Canreceive_CanLRcmdR[1]>=array_Canreceive_CanLRcmdR[5])&&(Structset_GlobalSet_Increase4_4.Setting_Para == DIRECTION_LEFT))
								{
									array_Canreceive_CanLRreturnT[1]=  DIRECTION_R_RIGHT;
								}
								else if((array_Canreceive_CanLRcmdR[1]<=array_Canreceive_CanLRcmdR[5])&&(Structset_GlobalSet_Increase4_4.Setting_Para == DIRECTION_RIGHT))
								{
									array_Canreceive_CanLRreturnT[1]=  DIRECTION_R_RIGHT;
								}
								else if((array_Canreceive_CanLRcmdR[1]<=array_Canreceive_CanLRcmdR[5])&&(Structset_GlobalSet_Increase4_4.Setting_Para == DIRECTION_LEFT))
								{
									array_Canreceive_CanLRreturnT[1]=  DIRECTION_L_LEFT;
								}
								array_Canreceive_CanLRreturnT[2] = array_Canreceive_CanLRcmdR[3];//伸前梁	
								array_Canreceive_CanLRreturnT[3] = array_Canreceive_CanLRcmdR[3];
								array_Canreceive_CanLRreturnT[4]=  array_Canreceive_CanLRcmdR[2];
								array_Canreceive_CanLRreturnT[5] = array_Canreceive_CanLRcmdR[5];
								array_Canreceive_CanLRreturnT[6] = 0;
								array_Canreceive_CanLRreturnT[7] = Structset_Service_DeviceNum1_1.Setting_Para;

							xQueueSend(Unite_Action_Queue,&array_Canreceive_CanLRreturnT,0);	
						
						
						
						
						}
				}
			break;
		case	 CANCEL:
					if(array_Canreceive_CanLRcmdR[1]>Structset_Service_DeviceNum1_1.Setting_Para)
					{
						xQueueSend(CAN_Send_Queue,&array_Canreceive_CanLRcmdR,0);
					}
					if((_variant_Mainlogic_statement.PRESENTstate_value == CONTROLED)&&(FLAG_drvusart_romatecontrol == 1))
					{
						FLAG_drvusart_romatecontrol = 0;
						State_main_TrumpetStatus = 1;
						LED_Management_Fanction(LED_NORMOL);
						/********************************改变设备受控状态---极为重要，本任务使用**********************************/
						_variant_Mainlogic_statement.PRESENTstate_value = State_Mainlogic_DWFAULTstate;
						_variant_Interface_disp.type = TYPE_DEFAULT;
						/********************************改变设备受控状态---极为重要，本任务使用**********************************/								
						xQueueSend(Interface_Mian_Logic_Queue,&const_Controled_InterfaceFlag,0);

						
					}
            break;
		case PLICHRETURN_1:
		case CANCELRETURN_1:
		
					if(Structset_Service_RemoteCortrol3_2.Setting_Para == 0)
					{
						xQueueSend(CAN_Send_Queue,&array_Canreceive_CanLRcmdR,0);
					}
					else if(Structset_Service_RemoteCortrol3_2.Setting_Para == 1)
					{
								array_drvusart_bufT[0] = 0x66;
								array_drvusart_bufT[1] = array_Canreceive_CanLRcmdR[0];
								array_drvusart_bufT[2] = array_Canreceive_CanLRcmdR[1];
								array_drvusart_bufT[3] = array_Canreceive_CanLRcmdR[2];
								array_drvusart_bufT[4] = array_Canreceive_CanLRcmdR[3];
								array_drvusart_bufT[5] = array_Canreceive_CanLRcmdR[4];
								array_drvusart_bufT[6] = array_Canreceive_CanLRcmdR[5];
								array_drvusart_bufT[7] = array_Canreceive_CanLRcmdR[6];
								array_drvusart_bufT[8] = array_Canreceive_CanLRcmdR[7];
								array_drvusart_bufT[9] = 0xaa;	
								uart_send(&com[0], array_drvusart_bufT, 20);	
                                vTaskDelay(20);					
					}
			break;
			case CANCELRETURN_2:
			case PLICHRETURN_2:
					if(Structset_Service_RemoteCortrol3_2.Setting_Para == 0)
					{
						xQueueSend(CAN_Send_Queue,&array_Canreceive_CanLRcmdR,0);
					}
					else if(Structset_Service_RemoteCortrol3_2.Setting_Para == 1)
					{

								array_drvusart_bufT[10] = 0x66;
								array_drvusart_bufT[11] = array_Canreceive_CanLRcmdR[0];
								array_drvusart_bufT[12] = array_Canreceive_CanLRcmdR[1];
								array_drvusart_bufT[15] = array_Canreceive_CanLRcmdR[2];
								array_drvusart_bufT[16] = array_Canreceive_CanLRcmdR[3];
								array_drvusart_bufT[17] = array_Canreceive_CanLRcmdR[4];
								array_drvusart_bufT[18] = array_Canreceive_CanLRcmdR[5];
								array_drvusart_bufT[13] = array_Canreceive_CanLRcmdR[6];
								array_drvusart_bufT[14] = array_Canreceive_CanLRcmdR[7];
								array_drvusart_bufT[19] = 0xaa;
								uart_send(&com[0], array_drvusart_bufT, 20);
								vTaskDelay(20);
					}
			
				break;
					
					
					case TPYE_FOLLOW_STRAT:
						
//					if(_variant_Mainlogic_statement.PRESENTstate_value == State_Mainlogic_DWFAULTstate)//默认状态下改变设备状态为被控
//					{
						State_Mainlogic_DWFAULTstate = AUTO_IDLE;
						_variant_Mainlogic_statement.PRESENTstate_value = State_Mainlogic_DWFAULTstate;
						xQueueSend(Interface_Mian_Logic_Queue,&const_Mainlogic_InterfaceFlag,0);
						array_Canreceive_CanBUSreturnT[0] = BUS_FOLLOWSTART_R_SEND;
						array_Canreceive_CanBUSreturnT[1] = Structset_Service_DeviceNum1_1.Setting_Para;
						CANx_SendNormalData(&BUS_CAN,0xee,array_Canreceive_CanBUSreturnT,2);
						array_Canreceive_CanLRcmdR[2] = array_Canreceive_CanLRcmdR[2] - 1;
						if(array_Canreceive_CanLRcmdR[2]>=1)
						{		
							xQueueSend(CAN_Send_Queue,&array_Canreceive_CanLRcmdR,0);								
						}
//					}
						
						break;
						
			case   TPYE_GROUP_CANAEL:
				array_Canreceive_CanLRcmdR[3] = array_Canreceive_CanLRcmdR[3]-1;
				if(array_Canreceive_CanLRcmdR[3] != 0)
				{
					xQueueSend(CAN_Send_Queue,array_Canreceive_CanLRcmdR,0);
				}

					
				switch(array_Canreceive_CanLRcmdR[2])
				{
					case 0x01://暂停
						if(_variant_Mainlogic_statement.PRESENTstate_value == MOVE)
							_variant_Mainlogic_statement.PRESENTstate_value = FREEZE;
					break;
						
					case 0x02://取消
						if((_variant_Mainlogic_statement.PRESENTstate_value == FREEZE)||(_variant_Mainlogic_statement.PRESENTstate_value == MOVE))
						{
							_variant_Mainlogic_statement.PRESENTstate_value = State_Mainlogic_DWFAULTstate;
						}
						else if(_variant_Mainlogic_statement.PRESENTstate_value == GROUP_CONTROLED)
						{
							_variant_Mainlogic_statement.PRESENTstate_value = State_Mainlogic_DWFAULTstate;
							_variant_Interface_disp.type = TYPE_DEFAULT;
							LED_Management_Fanction(LED_NORMOL);
							State_main_TrumpetStatus = 0;
							xQueueSend(Interface_Mian_Logic_Queue,&const_Mainlogic_InterfaceFlag,0);
						}
//						else if(_variant_Mainlogic_statement.PRESENTstate_value == MOVE)
//						{
//						
//							_variant_Mainlogic_statement.PRESENTstate_value = State_Mainlogic_DWFAULTstate;
//							_variant_Interface_disp.type = TYPE_DEFAULT;
//							LED_Management_Fanction(LED_NORMOL);
//							State_main_TrumpetStatus = 0;
//						}
							
						
						if(array_Canreceive_CanLRcmdR[3] == 0)
						{
							array_Canreceive_CanLRreturnT[0] = BUS_GROUP_STOP;
							CANx_SendNormalData(&BUS_CAN,array_Canreceive_CanLRcmdR[4],array_Canreceive_CanLRreturnT,1);
						}
					break;
						
					case 0x03://开始
						if(_variant_Mainlogic_statement.PRESENTstate_value == FREEZE)
							_variant_Mainlogic_statement.PRESENTstate_value = MOVE;
					break;
						
					default:
					break;
				}					
			break;
        
			case TPYE_RESET:
				Clear_Screen();
				Display_String(3,32,"设备重启");
				__disable_irq(); //关闭所有中断
				NVIC_SystemReset(); //进行软件复位
				HAL_Delay(1000);
					
			break;
        
        
//      case  TPYE_NEAR_TEST:
//        if(array_Canreceive_CanLRcmdR[1] == 0x11 && array_Canreceive_CanLRcmdR[2] == 0x33)
//        {
//          //右临架信息
//          near_flag[1]++;
//          near_pre[1] = array_Canreceive_CanLRcmdR[4] << 8 | array_Canreceive_CanLRcmdR[5];
//        }
//        else if(array_Canreceive_CanLRcmdR[1] == 0x22 && array_Canreceive_CanLRcmdR[2] == 0x33)
//        {
//          //左临架信息
//          near_flag[0]++;
//          near_pre[0] = array_Canreceive_CanLRcmdR[4] << 8 | array_Canreceive_CanLRcmdR[5];
//        }
//      
//      break;

		default:
			break;

	}
	




}
void BUScan_Reveice_Fuction(void)
{
	uint8_t innerarray_BUS[8] = {0};
	
	  if(xQueueReceive(CAN_Bus_Receive_Queue,array_Canreceive_CanBUScmdR,5000) == pdFAIL)
	  {
			return;
	  }
	switch(array_Canreceive_CanBUScmdR[0])
	{
		case BUS_R_PROGRAM:

			Clear_Screen();
			Display_String(3,32,"程序更新");
			__disable_irq(); //关闭所有中断
			NVIC_SystemReset(); //进行软件复位
			HAL_Delay(1000);			

			break;
	
		case BUS_STOP:
			if(array_Canreceive_CanBUScmdR[1] == 0x01)
			{

				Flag_main_STOP= 1;
				Flag_main_STOPcontitue= 1;
				variant_main_STOPnum = array_Canreceive_CanBUScmdR[2];
			}
			else if(array_Canreceive_CanBUScmdR[1] == 0x02)
			{
				Flag_main_STOP= 2;					
			}
			memset(array_Canreceive_CanBUScmdR,0,8);
			break;

		case BUS_R_CHEAK_R://上位机要求上报参数
		
				Function_sonser_pcReported(0x01);
				vTaskDelay(10);
				Function_sonser_pcReported(0x02);
		break;
		
		case BUS_R_POSITION://煤机位置
			variant_Follow_Cutterposition = array_Canreceive_CanBUScmdR[1];
			variant_Follow_Cutterderction = array_Canreceive_CanBUScmdR[2];
			array_Follow_Cutterposition[0] = ((variant_Follow_Cutterposition%1000)/100)+48;
			array_Follow_Cutterposition[1] = ((variant_Follow_Cutterposition%100)/10)+48;
			array_Follow_Cutterposition[2] = (variant_Follow_Cutterposition%10)+48;
			array_Follow_Cutterposition[3] = '\0';
			if(variant_Follow_Cutterderction == 1)
			{
				array_Follow_Cutterderction[0] = 0xC9;
				array_Follow_Cutterderction[1] = 0xCF;
				array_Follow_Cutterderction[2] = 0xD0;
				array_Follow_Cutterderction[3] = 0xD0;
				array_Follow_Cutterderction[4] = '\0';
			
			}
			else if(variant_Follow_Cutterderction == 0)
			{
				array_Follow_Cutterderction[0] = 0xCF;
				array_Follow_Cutterderction[1] = 0xC2;
				array_Follow_Cutterderction[2] = 0xD0;
				array_Follow_Cutterderction[3] = 0xD0;
				array_Follow_Cutterderction[4] = '\0';
			
			}
		break;
	
		case BUS_FOLLOW_STARTCHEAK_R:            //已改为0x3B
//			if(_variant_Mainlogic_statement.PRESENTstate_value == State_Mainlogic_DWFAULTstate)//默认状态下改变设备状态为被控
//			{
					State_Mainlogic_DWFAULTstate = AUTO_IDLE;
					_variant_Mainlogic_statement.PRESENTstate_value = State_Mainlogic_DWFAULTstate;
//					xQueueSend(Interface_Mian_Logic_Queue,&const_Mainlogic_InterfaceFlag,0);
//					array_Canreceive_CanBUSreturnT[0] = BUS_FOLLOWSTART_R_SEND;
//					array_Canreceive_CanBUSreturnT[1] = Structset_Service_DeviceNum1_1.Setting_Para;
//					CANx_SendNormalData(&BUS_CAN,0xee,array_Canreceive_CanBUSreturnT,2);
    
    
					innerarray_BUS[0] = BUS_FOLLOW_STARTCHEAK_T;                                 //回复相同标志位
                    innerarray_BUS[1] = Structset_Service_DeviceNum1_1.Setting_Para;      //架号
                    CANx_SendNormalData(&BUS_CAN,0xee,innerarray_BUS,8);
          

//			}				
		
			break;
	
		case 	BUS_FOLLOW_FAIL_R:
		case   BUS_FOLLOW_STOP_R:
					Display_String(page_Third_Line,32,"跟机失败");
					vTaskDelay(500);
					State_Mainlogic_DWFAULTstate = NORMOL_IDLE;
					_variant_Mainlogic_statement.PRESENTstate_value = State_Mainlogic_DWFAULTstate;
					xQueueSend(Interface_Mian_Logic_Queue,&const_Mainlogic_InterfaceFlag,0);
			break;
		
		case   BUS_FOLLOW_MOVE_R:
				if(state_Follow_movememory==0)
				{
					xQueueSend(Follow_Automation_Queue,array_Canreceive_CanBUScmdR,0);
				}
				else 
				{
					Faction_Follow_UP(state_Follow_movememory,0,0,0x01);

				}
			break;
		
		case   BUS_GROUP_STOP://成组停止记忆结束
				Flag_Mainlogic_Groupcontorl = 0;
				variant_Mainlogic_GroupMovenum = 0;
				Flag_Mainlogic_croupmovecancel = 0;
		
			break;
		
		case   BUS_COMPUTER_MOVE:
			if( _variant_Mainlogic_statement.PRESENTstate_value == State_Mainlogic_DWFAULTstate)
			{
				_variant_Mainlogic_statement.PRESENTstate_value = CONTROLED;
        _variant_Interface_disp.type = TYPE_DEFAULT;              //增加控制器状态显示(被控)    添加人任晓宇 添加日期2023/12/25
        state_controlded_movetype = 1;//控制台单动
        
				variant_BUS_continue = 1;
				State_main_TrumpetStatus = 2;
				LED_Management_Fanction(LED_NORMOL);
				xQueueSend(Interface_Mian_Logic_Queue,&const_Mainlogic_InterfaceFlag,0);
			}
			if(array_Canreceive_CanBUScmdR[1] == Structset_Service_DeviceNum1_1.Setting_Para)
			{
				array_Canreceive_CanBUSinnerT[0] = TPYE_SINGAL;
				array_Canreceive_CanBUSinnerT[1] = DIRECTION_ROMATE;
				array_Canreceive_CanBUSinnerT[2] = SINGAL_MOVE_START;
				array_Canreceive_CanBUSinnerT[3] = 1;
				array_Canreceive_CanBUSinnerT[4] = 0x01;
				array_Canreceive_CanBUSinnerT[5] = array_Canreceive_CanBUScmdR[3];
				array_Canreceive_CanBUSinnerT[6] = 0;
				array_Canreceive_CanBUSinnerT[7] = 0;
        
				xQueueSend(Unite_Action_Queue,&array_Canreceive_CanBUSinnerT,0);
			
			}
			break;
//		case BUS_FOLLOW_START_R:
//			
//			Structset_Follow_Head1_1.veiw_type = array_Canreceive_CanBUScmdR[2];//跟机机头
//			Structset_Follow_Tail2_1.veiw_type = array_Canreceive_CanBUScmdR[3];//跟机机尾
//			Structset_Follow_craft3_6.veiw_type= array_Canreceive_CanBUScmdR[1];//工艺
//			Set_Para_to_char(Structset_Follow_Head1_1.Setting_Para,Structset_Follow_Head1_1.veiw_type,Structset_Follow_Head1_1.Setting_Char);
//			Set_Para_to_char(Structset_Follow_Tail2_1.Setting_Para,Structset_Follow_Tail2_1.veiw_type,Structset_Follow_Tail2_1.Setting_Char);
//			Set_Para_to_char(Structset_Follow_craft3_6.Setting_Para,Structset_Follow_craft3_6.veiw_type,Structset_Follow_craft3_6.Setting_Char);
//		
//		
//			break;
	}



}




















