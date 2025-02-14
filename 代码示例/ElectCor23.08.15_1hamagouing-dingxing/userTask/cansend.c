#include "cansend.h"
#include "interface.h"
#include "stm32f4xx_hal.h"
#include "main.h"
#include "sensor.h"
#include "bsp_oled19264.h"
#include "main_logic.h"
#include "bsp_ncv7356.h"
#include "set.h"
#include "canreceive.h"
/**********************************************数组定义*******************************************************/
uint8_t array_Cansend_commandR[8];//传递命令变量  创建人蒋博 创建日期2023/01/8
uint8_t array_Cansend_commandTLR[8];//传递命令变量  创建人蒋博 创建日期2023/01/8

///*******************_variant_Mainlogic_singleCMD.type用，为单动动作类型******************/
//#define   SINGAL_CANCEI    0x00
//#define   SINGAL_PITCH_ON     0x01
//#define   SINGAL_MOVE_START       0x02
//#define   SINGAL_MOVE_STOP       0x03
//typedef struct _variant_mainlogicCMD1{
//	u8 type;		
//	u8 direction;		//单动方向
//	u8 movetype;		//单动动作类型
//	u8 num;		//  控制到第几架单动
//	u8 back_Flag;  //
//	u8 moveCMD;		//单动动作命令码
//}_variant_mainlogicCMD1;
void  Cansendfaction(void)
{
	uint8_t count_i=0;
		switch(array_Cansend_commandR[0])
		{
		
			case   TPYE_SINGAL:
				for(count_i = 0;count_i<6;count_i++)
				{
					array_Cansend_commandTLR[count_i] = array_Cansend_commandR[count_i];
				}
				if(array_Cansend_commandTLR[1] == DIRECTION_LEFT)
				    CANx_SendNormalData(&LEFT_CAN,COMMUN_ID,array_Cansend_commandR,8);
				else if(array_Cansend_commandTLR[1] == DIRECTION_RIGHT)
					CANx_SendNormalData(&RIGHT_CAN,COMMUN_ID,array_Cansend_commandR,8);
				break;
				
			case   0x02:
				if((array_Cansend_commandR[1] == DIRECTION_L_LEFT)||(array_Cansend_commandR[1] == DIRECTION_L_RIGHT))
				{
					CANx_SendNormalData(&LEFT_CAN,COMMUN_ID,array_Cansend_commandR,8);		
				}
				else if((array_Cansend_commandR[1] == DIRECTION_R_LEFT)||(array_Cansend_commandR[1] == DIRECTION_R_RIGHT))
				{
					CANx_SendNormalData(&RIGHT_CAN,COMMUN_ID,array_Cansend_commandR,8);				
				}			
				break;
			
			case   TPYE_SET_GLOBALPAR:
				array_Cansend_commandTLR[0] = array_Cansend_commandR[0];
				array_Cansend_commandTLR[1] = array_Cansend_commandR[1];		
				array_Cansend_commandTLR[2] = array_Cansend_commandR[2];
				array_Cansend_commandTLR[3] = array_Cansend_commandR[3];
				if(array_Cansend_commandR[1] == 0xff)
				{
					array_Cansend_commandTLR[1] =  DIRECTION_LEFT;
					CANx_SendNormalData(&LEFT_CAN,COMMUN_ID,array_Cansend_commandTLR,8);
					array_Cansend_commandTLR[1] =  DIRECTION_RIGHT;
					CANx_SendNormalData(&RIGHT_CAN,COMMUN_ID,array_Cansend_commandTLR,8);
				}
				
			break;	

//			case  TPYE_SERIALNUMBER_L:
//				array_Cansend_commandTLR[0] = TPYE_SERIALNUMBER_L;
//				array_Cansend_commandTLR[1] = Structset_GlobalSet_Increase4_4.Setting_Para;
//				array_Cansend_commandTLR[2] = Structset_Service_DeviceNum1_1.Setting_Para;
//				CANx_SendNormalData(&LEFT_CAN,COMMUN_ID,array_Cansend_commandTLR,8);			
//			break;
				
//			case  TPYE_SERIALNUMBER_R:
//				array_Cansend_commandTLR[0] = TPYE_SERIALNUMBER_R;
//				array_Cansend_commandTLR[1] = Structset_GlobalSet_Increase4_4.Setting_Para;
//				array_Cansend_commandTLR[2] = Structset_Service_DeviceNum1_1.Setting_Para;
//				CANx_SendNormalData(&RIGHT_CAN,COMMUN_ID,array_Cansend_commandTLR,8);

				
//			break;
				
			case  TPYE_PARAMETER:
					for(count_i = 0;count_i<8;count_i++)
					{
						array_Cansend_commandTLR[count_i] = array_Cansend_commandR[count_i];
					}
					array_Cansend_commandTLR[0] =  TPYE_PARAMETER_L;
					CANx_SendNormalData(&LEFT_CAN,COMMUN_ID,array_Cansend_commandTLR,8);
					array_Cansend_commandTLR[0] =  TPYE_PARAMETER_R;
					CANx_SendNormalData(&RIGHT_CAN,COMMUN_ID,array_Cansend_commandTLR,8);				
			break;
			case TPYE_PARAMETER_L:
					CANx_SendNormalData(&LEFT_CAN,COMMUN_ID,array_Cansend_commandR,8);
				break;
			case TPYE_PARAMETER_R:
					CANx_SendNormalData(&RIGHT_CAN,COMMUN_ID,array_Cansend_commandR,8);
				break;
			case     PITCH_ON:
			case	 SINGAL_MOVE:
			case	 GROUP_CMD:
			case	 CANCEL:
					if(Structset_GlobalSet_Increase4_4.Setting_Para == DIRECTION_RIGHT)
					{
						CANx_SendNormalData(&RIGHT_CAN,COMMUN_ID,array_Cansend_commandR,8);
				
					}
					else if(Structset_GlobalSet_Increase4_4.Setting_Para == DIRECTION_LEFT)
					{
						
						CANx_SendNormalData(&LEFT_CAN,COMMUN_ID,array_Cansend_commandR,8);
					}
				break;
			case     PLICHRETURN_1:
			case	 PLICHRETURN_2:
			case     CANCELRETURN_1:
			case	 CANCELRETURN_2:
						if(Structset_GlobalSet_Increase4_4.Setting_Para == DIRECTION_RIGHT)
						{
							CANx_SendNormalData(&LEFT_CAN,COMMUN_ID,array_Cansend_commandR,8);
					
						}
						else if(Structset_GlobalSet_Increase4_4.Setting_Para == DIRECTION_LEFT)
						{
							
							CANx_SendNormalData(&RIGHT_CAN,COMMUN_ID,array_Cansend_commandR,8);
						}
			
			
			break;
						
			case   TPYE_GROUP_ALARM:
				switch(array_Cansend_commandR[1])
				{
					case DIRECTION_L_LEFT:
					case	DIRECTION_R_LEFT:
							CANx_SendNormalData(&LEFT_CAN,COMMUN_ID,array_Cansend_commandR,8);
						break;
					
					case DIRECTION_L_RIGHT:
					case	DIRECTION_R_RIGHT:
							CANx_SendNormalData(&RIGHT_CAN,COMMUN_ID,array_Cansend_commandR,8);
						break;
					default:
						break;
				
				
				}

				
				break;
			case   TPYE_FOLLOW_STRAT:
				if(array_Cansend_commandR[1] == DIRECTION_LEFT)
				{
					CANx_SendNormalData(&LEFT_CAN,COMMUN_ID,array_Cansend_commandR,8);		
				}
				else if(array_Cansend_commandR[1] == DIRECTION_RIGHT)
				{
					CANx_SendNormalData(&RIGHT_CAN,COMMUN_ID,array_Cansend_commandR,8);				
				}			
				break;
			case   TPYE_GROUP_CANAEL:
				if(array_Cansend_commandR[1] == DIRECTION_LEFT)
				{
					CANx_SendNormalData(&LEFT_CAN,COMMUN_ID,array_Cansend_commandR,5);		
				}
				else if(array_Cansend_commandR[1] == DIRECTION_RIGHT)
				{
					CANx_SendNormalData(&RIGHT_CAN,COMMUN_ID,array_Cansend_commandR,5);				
				}			
			break;
					
			default:
			break;
		}
}
	


























