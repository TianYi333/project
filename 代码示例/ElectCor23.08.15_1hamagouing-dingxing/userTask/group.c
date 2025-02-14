#include "group.h"
#include "sensor.h"
#include "main.h"
#include "main_logic.h"
#include "bsp_oled19264.h"
#include "interface.h"
#include "set.h"
#include "controlded.h"
/**********************************************���鶨��*******************************************************/

uint8_t  Flag_busmanage_Infrared = 0;//ú���Ƿ񵽴�  1Ϊ����

uint8_t *Display_Group_Movedirectionjudge(uint8_t direction_flag,uint8_t Separated_Num,uint8_t Action_Num)
{
		static uint8_t Group_Num[4];//Group_Num[0]==Group_Unite_Move_num,Group_Num[1]==Device_Num_Group_limit��Group_Num[2]== direction_flag,Group_Num[3] == �Ƿ��޿ɳ���� .
	  Group_Num[3] = 0;
		if((direction_flag&0xf) != Structset_GlobalSet_Increase4_4.Setting_Para)//
		{
				
						
				Group_Num[2] = Structset_Service_DeviceNum1_1.Setting_Para-Separated_Num-1;
				if((Structset_Service_DeviceNum1_1.Setting_Para-Separated_Num-1>=1)&&(Structset_Service_DeviceNum1_1.Setting_Para<=(Action_Num + Separated_Num+1)))//����Ҳ��мܵ��������鶯����
				{					
						Group_Num[0]  = Structset_Service_DeviceNum1_1.Setting_Para-Separated_Num-1;    //ȷ�����Ƽ��ܣ�1���ǿ���һ��
						Group_Num[1] = 1;
				}					
				else if(Structset_Service_DeviceNum1_1.Setting_Para >(Action_Num+Separated_Num+1))//����Ҳ��м����㹻���鶯����
				{
						Group_Num[0]  = Action_Num;          		//ȷ�����Ƽ��ܣ�1���ǿ���һ��
						Group_Num[1] = Structset_Service_DeviceNum1_1.Setting_Para-Separated_Num-Action_Num;
				}
				else if(((Structset_Service_DeviceNum1_1.Setting_Para-Separated_Num-1) == 0)||(Structset_Service_DeviceNum1_1.Setting_Para==0))
				{
						Group_Num[3] = 1;
				}					
		}	
		else if((direction_flag&0xf) == Structset_GlobalSet_Increase4_4.Setting_Para)
		{

						
				Group_Num[2] = Structset_Service_DeviceNum1_1.Setting_Para+Separated_Num+1;
				if(((Structset_GlobalSet_MaxDeviceNum1_1.Setting_Para-Structset_Service_DeviceNum1_1.Setting_Para-Separated_Num)>=1)&&((Structset_GlobalSet_MaxDeviceNum1_1.Setting_Para-Structset_Service_DeviceNum1_1.Setting_Para)<=(Action_Num+Separated_Num)))
				{
						Group_Num[0]  = (Structset_GlobalSet_MaxDeviceNum1_1.Setting_Para-Structset_Service_DeviceNum1_1.Setting_Para)-Separated_Num;    //ȷ�����Ƽ��ܣ�1���ǿ���һ��
						Group_Num[1] = Structset_GlobalSet_MaxDeviceNum1_1.Setting_Para;
				}
				else if((Structset_GlobalSet_MaxDeviceNum1_1.Setting_Para-Structset_Service_DeviceNum1_1.Setting_Para)>(Action_Num+Separated_Num))
				{
						Group_Num[0]  = Action_Num;    //ȷ�����ƹ涨�������
						Group_Num[1] = Structset_Service_DeviceNum1_1.Setting_Para+Separated_Num+Action_Num;
				}
				else if((Structset_GlobalSet_MaxDeviceNum1_1.Setting_Para-Structset_Service_DeviceNum1_1.Setting_Para-Separated_Num) == 0)
				{
						Group_Num[3] = 1;					
				}		
		}
		return Group_Num;
}




void Group_Move_Control(uint8_t num)
{
	
	
	uint16_t i=0;
			uint32_t contorl_Valve = 0;
			uint16_t Distance_M;//����ǰ�г�ֵ	
			uint8_t i_DcolumnAgainFrequency;

			switch(num)
			{
						
				case MOVE_GROUP_ADVANCING:

					if(Structset_Framemove_Forbiden1_3.Setting_Para == 1)//�Ƽ�����
					{
							
								i_DcolumnAgainFrequency =Structset_Framemove_DcolumnAgainFrequency13_1.Setting_Para;
									contorl_Valve = 0;//�������ʼ�����������
									/**********���********/
									if(Structset_Framemove_Alarm2_2.Setting_Para == 0)//����Ƽܱ����رգ���رձ�������
										State_main_TrumpetStatus = 0;
									
									

						
//									if(Structset_Framemove_TelescopictMove7_2.Setting_Para ==1)//�ж��Ƿ���������
//									{									
//											contorl_Valve = contorl_Valve|MOVE_TELESCOPIC_GIRDER_WITHDRAW_OUT;
//									}
//									if(Structset_Framemove_PlateMove8_2.Setting_Para ==1)//�ж��Ƿ��ջ���
//									{									
//											contorl_Valve = contorl_Valve|MOVE_PLATE_WITHDRAW_OUT;
//									}
									vTaskDelay(Structset_Framemove_ColumndownDelay9_1.Setting_Para*1000);//������ʱ
									contorl_Valve = contorl_Valve|MOVE_COLUMN_DOWN_OUT;//����
									if(Structset_Framemove_Spray22_2.Setting_Para==1)
									{
										contorl_Valve = contorl_Valve|MOVE_FUEL_SPRAY_OUT;//����
									}
									while((i<Structset_Framemove_ColumndownTime10_1.Setting_Para*10)&&((_variant_Mainlogic_statement.PRESENTstate_value == MOVE)||(_variant_Mainlogic_statement.PRESENTstate_value == FREEZE)))//����ʱ��
									{
											MOVE_controled_Driver(contorl_Valve);//����
											Group_Pause_Count();
											i++;
											vTaskDelay(100);
											if((((Structset_Framemove_ColumndownTime10_1.Setting_Para*10)/i)<=5)&&(Structset_Framemove_Columndownsideplate12_2.Setting_Para==1))//����ʱ�໤����-�ղ໤
											{
													contorl_Valve = contorl_Valve|MOVE_SIDE_PLATE_WITHDRAW_OUT;//�ղ໤							
											}
											if((((Structset_Framemove_ColumndownTime10_1.Setting_Para*10)/i)<=2)&&(Structset_Framemove_ColumndownBalanceaction11_2.Setting_Para==1))//����ʱƽ��������
											{
													contorl_Valve = contorl_Valve&(~MOVE_SIDE_PLATE_WITHDRAW_OUT);//ֹͣ�ղ໤
													contorl_Valve = contorl_Valve|MOVE_BALANCE_BEAM_WITHDRAW_OUT;	//��ƽ����					
											}
											
									}
									
									if(_sensor_pressure.sensor_value<Structset_Framemove_movePressure4_1.Setting_Para)
									{
										if(i_DcolumnAgainFrequency>0)
										{
											
											i_DcolumnAgainFrequency--;
											if(i_DcolumnAgainFrequency != 0)
											{
												contorl_Valve = MOVE_COLUMN_DOWN_OUT;
												i=0;
												while((i<Structset_Framemove_ColumndownTime10_1.Setting_Para*10)&&((_variant_Mainlogic_statement.PRESENTstate_value == MOVE)||(_variant_Mainlogic_statement.PRESENTstate_value == FREEZE)))//����ʱ��
												{
													Group_Pause_Count();
													MOVE_controled_Driver(contorl_Valve);//����

													i++;
													vTaskDelay(100);	
												}
											}
											
										
										}
									}
									i=0;
									contorl_Valve = 0;
									contorl_Valve = MOVE_HYDRAULICFRAME_ADVANCING_OUT;//�Ƽ�
									if(Structset_Framemove_Spray22_2.Setting_Para==1)
									{
										contorl_Valve = contorl_Valve|MOVE_FUEL_SPRAY_OUT;//����
									}
									if(Structset_Framemove_BmttomUP14_2.Setting_Para == 1)
									{
										contorl_Valve = contorl_Valve|MOVE_BOTTOM_UP_OUT;//̧��
									}
 									while((i<(Structset_Framemove_AdvancTime16_1.Setting_Para*10))&&((_variant_Mainlogic_statement.PRESENTstate_value == MOVE)||(_variant_Mainlogic_statement.PRESENTstate_value == FREEZE)))
									{
											MOVE_controled_Driver(contorl_Valve);
											Group_Pause_Count();
//											Group_Pause_Count();
//											if(((ADC_Sensor_ConvertedValue[0]-Distance_M)>=Group_Targetdistance.Setting_Para)&&(Default_Para_Distance.Setting_Para==1))//�ﵽĿ���г�
//											{
//													i	=	2000;
//											}
											i++;
											vTaskDelay(100);
						
											if((_sensor_distance.sensor_value <= Structset16_Framemove_Advancaim15_1.Setting_Para)&&(Structset_Default_Distancecheak2_2.Setting_Para==1))
											{
												i=9999;
											}
									}


									
		
									i=0;
									contorl_Valve = 0;//����
									contorl_Valve = MOVE_COLUMN_UP_OUT;//����
									if(Structset_Framemove_ColumnUPBalanceaction20_2.Setting_Para==1)
									{
										contorl_Valve = contorl_Valve|MOVE_BALANCE_BEAM_STRETCH_OUT;//����
									}
									while((i<Structset_Framemove_ColumnUPtime19_1.Setting_Para*10)&&((_variant_Mainlogic_statement.PRESENTstate_value == MOVE)||(_variant_Mainlogic_statement.PRESENTstate_value == FREEZE)))//�Ƽ����ʱ��
									{
											MOVE_controled_Driver(contorl_Valve);
											Group_Pause_Count();
											i++;
											vTaskDelay(100);
											
//											Group_Pause_Count();
																					
									}
								
							
					}						
				break;
				
				case MOVE_GROUP_CHUTE_PUSH:
						if(Structset_ChutePush_Forbiden1_3.Setting_Para == 1)
						{

								i = 0;
							contorl_Valve = 0;
								contorl_Valve = MOVE_CHUTE_PUSH_OUT;//����

							if(_sensor_distance.sensor_value + Structset16_ChutePush_aimDistance3_1.Setting_Para >= 1000)
							{
								Distance_M = 1000;
							}
							else if(_sensor_distance.sensor_value + Structset16_ChutePush_aimDistance3_1.Setting_Para <1000)
							{
									Distance_M = _sensor_distance.sensor_value + Structset16_ChutePush_aimDistance3_1.Setting_Para;
							}
								
								while((i<Structset_ChutePush_time2_1.Setting_Para*10)&&((_variant_Mainlogic_statement.PRESENTstate_value == MOVE)||(_variant_Mainlogic_statement.PRESENTstate_value == FREEZE)))
								{

										MOVE_controled_Driver(contorl_Valve);
										Group_Pause_Count();
										vTaskDelay(100);
										i++;
										if((_sensor_distance.sensor_value>=Distance_M)&&(Structset_Default_Distancecheak2_2.Setting_Para==1))
										{
											i=9999;
										}									
								}
	
						}
				break;
				case MOVE_GROUP_CHUTE_PULL:
						if(Structset_ChutePull_Forbiden1_3.Setting_Para == 1)
						{

								i = 0;
							contorl_Valve = 0;
								contorl_Valve = MOVE_HYDRAULICFRAME_ADVANCING_OUT;//����

								while((i<Structset_ChutePull_time2_1.Setting_Para*10)&&((_variant_Mainlogic_statement.PRESENTstate_value == MOVE)||(_variant_Mainlogic_statement.PRESENTstate_value == FREEZE)))
								{
										MOVE_controled_Driver(contorl_Valve);
										Group_Pause_Count();
										vTaskDelay(100);
										i++;									
								}
	
						}
				break;
				case MOVE_GROUP_TELESCOPIC_GIRDER_STRETCH:
						if(Structset_Telescopict_Forbiden1_3.Setting_Para == 1)
						{

								i = 0;
							contorl_Valve = 0;
								contorl_Valve = MOVE_TELESCOPIC_GIRDER_STRETCH_OUT;//��ǰ��

								while((i<Structset_Telescopict_StretchTime2_1.Setting_Para*10)&&((_variant_Mainlogic_statement.PRESENTstate_value == MOVE)||(_variant_Mainlogic_statement.PRESENTstate_value == FREEZE)))
								{

										MOVE_controled_Driver(contorl_Valve);
										Group_Pause_Count();
										vTaskDelay(100);
										i++;									
								}
	
						}
				break;
				case MOVE_GROUPTELESCOPICPLATE_GIRDER_WITHDRAW:
						if(Structset_Telescopict_Forbiden1_3.Setting_Para == 1)
						{
								i = 0;
								contorl_Valve = 0;
								contorl_Valve = MOVE_COLUMN_DOWN_OUT;//����
								while((i<Structset_Telescopict_DcolumnTime4_1.Setting_Para*10)&&((_variant_Mainlogic_statement.PRESENTstate_value == MOVE)||(_variant_Mainlogic_statement.PRESENTstate_value == FREEZE)))//��������ʱ����ʱ��
								{
										MOVE_controled_Driver(contorl_Valve);
										Group_Pause_Count();
										vTaskDelay(100);
										i++;									
								}
								i = 0;
								
								
								contorl_Valve = 0;
								contorl_Valve = MOVE_TELESCOPIC_GIRDER_WITHDRAW_OUT;//��ǰ��
								while((i<Structset_Telescopict_ShrinkTime3_1.Setting_Para*10)&&((_variant_Mainlogic_statement.PRESENTstate_value == MOVE)||(_variant_Mainlogic_statement.PRESENTstate_value == FREEZE)))//��������ʱ����ʱ��
								{
										MOVE_controled_Driver(contorl_Valve);
										Group_Pause_Count();
										vTaskDelay(100);
										i++;									
								}
								i = 0;
							
								if(Structset_PLATE_Forbiden7_3.Setting_Para == 1)
								{
									contorl_Valve = 0;
									contorl_Valve = MOVE_PLATE_WITHDRAW_OUT;//�ջ���
									while((i<Structset_PLATE_ShrinkTime9_1.Setting_Para*10)&&((_variant_Mainlogic_statement.PRESENTstate_value == MOVE)||(_variant_Mainlogic_statement.PRESENTstate_value == FREEZE)))//��������ʱ����ʱ��
									{
											MOVE_controled_Driver(contorl_Valve);
											Group_Pause_Count();
											vTaskDelay(100);
											i++;									
									}
								}
								i = 0;
								
								contorl_Valve = 0;
								contorl_Valve = MOVE_COLUMN_UP_OUT;//����
								while((i<Structset_Telescopict_DcolumnTime4_1.Setting_Para*15)&&((_variant_Mainlogic_statement.PRESENTstate_value == MOVE)||(_variant_Mainlogic_statement.PRESENTstate_value == FREEZE)))//��������ʱ����ʱ��
								{
										MOVE_controled_Driver(contorl_Valve);
										Group_Pause_Count();
										vTaskDelay(100);
										i++;									
								}
								
						}
				break;
				case MOVE_GROUP_PLATE_STRETCH:
						if(Structset_PLATE_Forbiden7_3.Setting_Para == 1)
						{

								i = 0;
							contorl_Valve = 0;
								contorl_Valve = MOVE_PLATE_STRETCH_OUT;//�커��

								while((i<Structset_PLATE_StretchTime8_1.Setting_Para*10)&&((_variant_Mainlogic_statement.PRESENTstate_value == MOVE)||(_variant_Mainlogic_statement.PRESENTstate_value == FREEZE)))
								{

										MOVE_controled_Driver(contorl_Valve);
										vTaskDelay(100);
										i++;
										Group_Pause_Count();									
								}
	
						}
				break;
						
			case MOVE_GROUP_PLATE_WITHDRAW:
						if(Structset_PLATE_Forbiden7_3.Setting_Para == 1)
						{

								i = 0;
							contorl_Valve = 0;
								contorl_Valve = MOVE_PLATE_WITHDRAW_OUT;//�ջ���

								while((i<Structset_PLATE_ShrinkTime9_1.Setting_Para*10)&&((_variant_Mainlogic_statement.PRESENTstate_value == MOVE)||(_variant_Mainlogic_statement.PRESENTstate_value == FREEZE)))
								{

										MOVE_controled_Driver(contorl_Valve);
										vTaskDelay(100);
										i++;
										Group_Pause_Count();									
								}
	
						}
				break;


     }
}




void Group_Pause_Count(void)
{
			uint16_t i_pause=0;
			if(_variant_Mainlogic_statement.PRESENTstate_value == FREEZE)
			{
					Display_String(7,0,"��ͣ");
					while((i_pause<1200)&&(_variant_Mainlogic_statement.PRESENTstate_value == FREEZE))
					{
							i_pause++;
							vTaskDelay(100);						
					}
					i_pause=0;
					
			}
			if(_variant_Mainlogic_statement.PRESENTstate_value == FREEZE)
			{
				_variant_Mainlogic_statement.PRESENTstate_value = MOVE;
			}
			Display_String(7,0,"    ");


}











