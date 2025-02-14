#include "drv_usart.h"
#include "bsp_485.h"
#include "interface.h"
#include "sensor.h"
#include "main_logic.h"
#include "set.h"
#include "canreceive.h"
#include "bsp_hd7279.h"
#define RX_MAX_NUM 20

struct _hw_com_port com[2] = {0};

uint8_t array_drvusart_bufR[20];
uint8_t array_drvusart_bufT[20] = {0x66,0 ,0,0,0,0,0,0,0,0xaa};
uint8_t FLAG_drvusart_romatecontrol;//电液控被遥控选中标志位


static void hw_usart_callback(void *func);
/**************************************************************************
函数名   : 
功能     : 串口接口初始化
参数     : 
返回值   : 
***************************************************************************/ 
void hw_usart_register(UART_HandleTypeDef *uart)
{
		if(uart->Instance == huart6.Instance){

		com[0].rx_over_flag = 0;
		com[0].rx_count = 0;
		com[0].rx_ptr = array_drvusart_bufR;
		com[0].tx_ptr = array_drvusart_bufT;
		com[0].uartx = &huart6;
		com[0].cb = hw_usart_callback;
		com[0].send = uart_send;				
#ifdef UART_USING_DMA	
		com[0].hdma_rx = &hdma_usart6_rx;
		com[0].hdma_tx = &hdma_usart6_tx;
		__HAL_UART_ENABLE_IT(&huart6, UART_IT_IDLE);
		HAL_UART_Receive_DMA(&huart6, com[0].rx_ptr, RX_MAX_NUM);
#else
		HAL_UART_Receive_IT(&huart6, &com[0].singlechar, 1);	
#endif		
		}
		
		else if(uart->Instance == huart2.Instance)
		{

		com[1].rx_over_flag = 0;
		com[1].rx_count = 0;
		com[1].rx_ptr = variant_Controled_driveR;
		com[1].tx_ptr = variant_Controled_driveCMD;
		com[1].uartx = &huart2;
		com[1].cb = hw_usart_callback;
		com[1].send = uart_send;				
#ifdef UART_USING_DMA	
		com[1].hdma_rx = &hdma_usart2_rx;
		com[1].hdma_tx = &hdma_usart2_tx;
		__HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);
		HAL_UART_Receive_DMA(&huart2, com[1].rx_ptr, 10);
#else
		HAL_UART_Receive_IT(&huart2, &com[1].singlechar, 1);	
#endif		
		}	
			return;
}

void hw_usart_callback(void *func)
{
		struct _hw_com_port* ptr = (struct _hw_com_port*)func;
		
}

/**************************************************************************
函数名   : 
功能     : 串口数据发送
参数     : 
返回值   : 
***************************************************************************/ 
HAL_StatusTypeDef uart_send(struct _hw_com_port* port, uint8_t *buffer, uint32_t length)
{
#ifdef UART_USING_DMA	
		while(HAL_DMA_GetState(port->hdma_tx) != HAL_DMA_STATE_READY)
    __HAL_DMA_DISABLE(port->hdma_tx);
    HAL_UART_Transmit_DMA(port->uartx, buffer, length);
#else
		HAL_UART_Transmit(port->uartx, buffer, length, 0xffff);
#endif
		
		return HAL_OK;
}

/**************************************************************************
函数名   : 
功能     : 串口接收数据
参数     : 
返回值   : 
***************************************************************************/ 
void uart_rx_callback(struct _hw_com_port* port)
{
		 port->rx_ptr[port->rx_count ++] = port->singlechar; 
		 if(port->rx_count >= RX_MAX_NUM)	port->rx_count = 0;
		 port->rx_tim_count = 0;
		 HAL_UART_Receive_IT(port->uartx, &port->singlechar, 1);
}
/**************************************************************************
函数名   : 
功能     : 串口发送完成处理 置485接收
参数     : 
返回值   : 
***************************************************************************/
void uart_tx_callback(struct _hw_com_port* port)
{

}
/**************************************************************************
函数名   : 
功能     : 定时器超时判断一帧数据接收完成
参数     : 
返回值   : 
***************************************************************************/ 
void uart_tm_callback(struct _hw_com_port* port)
{
		if(port->rx_sta != port->rx_count)
		{
			if(port->rx_tim_count ++ > 5){
				port->rx_tim_count = 0;
				port->rx_sta = port->rx_count;
				port->rx_over_flag = 1;}
		}	
}

/**************************************************************************
函数名   : 
功能     : 串口接收回调
参数     : 
返回值   : 
***************************************************************************/
//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
void uart_callback_function(UART_HandleTypeDef *huart)
{
	uint32_t temp;
	if(huart->Instance == com[0].uartx->Instance)
	{
#ifdef UART_USING_DMA
		if(__HAL_UART_GET_FLAG(&huart6,UART_FLAG_IDLE) != RESET)
		{			
				__HAL_UART_CLEAR_IDLEFLAG(&huart6);
				temp = huart6.Instance->DR;
				HAL_UART_DMAStop(&huart6); 
				com[0].rx_count = RX_MAX_NUM - (__HAL_DMA_GET_COUNTER(&hdma_usart6_rx)); //接收个数等于接收缓冲区总大小减剩余计数		
	//			memcpy(receive_data, usart1_rx_buffer, usart1_rx_len); 				/* 搬移数据进行其他处理 */
				com[0].rx_over_flag = 1;
		}
		HAL_UART_Receive_DMA(&huart6, com[0].rx_ptr, RX_MAX_NUM);
#else
		uart_rx_callback(&com[0]);
#endif
	}
	if(huart->Instance == com[1].uartx->Instance)
	{
#ifdef UART_USING_DMA
		if(__HAL_UART_GET_FLAG(&huart2,UART_FLAG_IDLE) != RESET)
		{			
				__HAL_UART_CLEAR_IDLEFLAG(&huart2);
				temp = huart2.Instance->DR;
				HAL_UART_DMAStop(&huart2); 
				com[1].rx_count = RX_MAX_NUM - (__HAL_DMA_GET_COUNTER(&hdma_usart2_rx)); //接收个数等于接收缓冲区总大小减剩余计数		
	//			memcpy(receive_data, usart1_rx_buffer, usart1_rx_len); 				/* 搬移数据进行其他处理 */
				com[1].rx_over_flag = 1;
		}
		HAL_UART_Receive_DMA(&huart2, com[1].rx_ptr, 10);
#else
		uart_rx_callback(&com[1]);
#endif
	}

}
/******************************
遥控器接收函数
接收遥控器指令并进行发送，每四架只有一架有效调用此函数，架号为四架中的最小架号


*****************************/
void Romate_control_reception(void)
{
	uint16_t Crc;
	uint8_t array_CMD[8];
	uint8_t const_Controled_InterfaceFlag=1;
		if(com[0].rx_over_flag && (com[0].rx_count == 12))
		{
			com[0].rx_over_flag = 0;			
			com[0].rx_count = 0;		
			
			if((array_drvusart_bufR[0] == 0x55)&&(array_drvusart_bufR[9] == 0xAA))
			{
				Crc = CRC16(array_drvusart_bufR, 10);
				if((array_drvusart_bufR[10] == Crc / 256)&&(array_drvusart_bufR[11] ==Crc % 256))
				{
					if(array_drvusart_bufR[2] == variant_Service_DeviceNum)
					{
						switch(array_drvusart_bufR[1])
						{
							 case PITCH_ON:
								if(_variant_Mainlogic_statement.PRESENTstate_value == State_Mainlogic_DWFAULTstate)
								{
									array_drvusart_bufT[0] = 0x66;
									array_drvusart_bufT[1] = PLICHRETURN_1;
									array_drvusart_bufT[2] = variant_Service_DeviceNum;
									array_drvusart_bufT[3] = 0x01;
									array_drvusart_bufT[4] = (_sensor_distance.sensor_value>>8)&0xff;
									array_drvusart_bufT[5] = _sensor_distance.sensor_value&0xff;
									array_drvusart_bufT[6] = (_sensor_pressure.sensor_value>>8)&0xff;
									array_drvusart_bufT[7] = _sensor_pressure.sensor_value&0xff;
									array_drvusart_bufT[8] = 0;
									array_drvusart_bufT[9] = 0xaa;
									array_drvusart_bufT[10] = 0x66;
									array_drvusart_bufT[11] = PLICHRETURN_2;
									array_drvusart_bufT[12] = variant_Service_DeviceNum;
									array_drvusart_bufT[15] = _sensor_Connectingrod_angle.sensor_view[0];
									array_drvusart_bufT[16] = (_sensor_Connectingrod_angle.sensor_value/100)&0xff;
									array_drvusart_bufT[17] = _sensor_canopy_angle.sensor_view[0];
									array_drvusart_bufT[18] = (_sensor_canopy_angle.sensor_value/100)&0xff;
									array_drvusart_bufT[13] = _sensor_Plate_angle.sensor_view[0];
									array_drvusart_bufT[14] = (_sensor_Plate_angle.sensor_value/100)&0xff;
									array_drvusart_bufT[19] = 0xaa;
									uart_send(&com[0], array_drvusart_bufT, 20);
									vTaskDelay(1000);
									FLAG_drvusart_romatecontrol = 1;//该电液控直接受控
									State_main_TrumpetStatus = 2;
										
									LED_Management_Fanction(LED_BLINK);
									/********************************改变设备受控状态---极为重要，本任务使用**********************************/
	//								if(_variant_Mainlogic_statement.PRESENTstate_value == State_Mainlogic_DWFAULTstate)
	//								{
										_variant_Mainlogic_statement.PRESENTstate_value = CONTROLED;
									
										_variant_Interface_disp.type = TYPE_DEFAULT;
									/********************************改变设备受控状态---极为重要，本任务使用**********************************/								
										xQueueSend(Interface_Mian_Logic_Queue,&const_Controled_InterfaceFlag,0);
	//								}
								}		
								 break;
								 
								 case SINGAL_MOVE:
									 if((FLAG_drvusart_romatecontrol == 1)&&(_variant_Mainlogic_statement.PRESENTstate_value == CONTROLED))
									 {
											 array_CMD[0] = TPYE_SINGAL;
											array_CMD[1] = DIRECTION_ROMATE;
											array_CMD[2] = SINGAL_MOVE_START;
											array_CMD[3] = 1;
										 array_CMD[4] = 0x01;
											array_CMD[5] = array_drvusart_bufR[3];
										 array_CMD[6] = 0;
										 array_CMD[7] = 0;
											xQueueSend(Unite_Action_Queue,&array_CMD,0);
									 }
									 
										break;
								
								case GROUP_CMD:
										if((array_drvusart_bufR[3] == MOVE_GROUP_ADVANCING)||(array_drvusart_bufR[3] == MOVE_GROUPTELESCOPICPLATE_GIRDER_WITHDRAW))
										{
											array_CMD[0] = TPYE_GROUP_ALARM;
											if((array_drvusart_bufR[2]>=array_drvusart_bufR[6])&&(Structset_GlobalSet_Increase4_4.Setting_Para == DIRECTION_RIGHT))
											{
												array_CMD[1]=  DIRECTION_L_LEFT;
											}
											else if((array_drvusart_bufR[2]>=array_drvusart_bufR[6])&&(Structset_GlobalSet_Increase4_4.Setting_Para == DIRECTION_LEFT))
											{
												array_CMD[1]=  DIRECTION_R_RIGHT;
											}
											else if((array_drvusart_bufR[2]<=array_drvusart_bufR[6])&&(Structset_GlobalSet_Increase4_4.Setting_Para == DIRECTION_RIGHT))
											{
												array_CMD[1]=  DIRECTION_R_RIGHT;
											}
											else if((array_drvusart_bufR[2]<=array_drvusart_bufR[6])&&(Structset_GlobalSet_Increase4_4.Setting_Para == DIRECTION_LEFT))
											{
												array_CMD[1]=  DIRECTION_L_LEFT;
											}
											
											
											array_CMD[2] = array_drvusart_bufR[4];//伸前梁	
											array_CMD[3] = array_drvusart_bufR[4];
											array_CMD[4]= 	array_drvusart_bufR[3];
											array_CMD[5] = array_drvusart_bufR[6];
											array_CMD[6] = 0;
											array_CMD[7] = Structset_Service_DeviceNum1_1.Setting_Para;
								
										xQueueSend(Unite_Action_Queue,&array_CMD,0);
//										if((array_CMD[1] == DIRECTION_L_LEFT)||(array_CMD[1] == DIRECTION_R_RIGHT))
//										{
//											vTaskDelay(2);
//											array_CMD[0]= TPYE_GROUP;
//											vTaskDelay(2);
//											xQueueSend(Unite_Action_Queue,&array_CMD,0);
//										}
										
										
										}
										else {
												array_CMD[0] = TPYE_GROUP;
												if((array_drvusart_bufR[2]>=array_drvusart_bufR[6])&&(Structset_GlobalSet_Increase4_4.Setting_Para == DIRECTION_RIGHT))
												{
													array_CMD[1]=  DIRECTION_L_LEFT;
												}
												else if((array_drvusart_bufR[2]>=array_drvusart_bufR[6])&&(Structset_GlobalSet_Increase4_4.Setting_Para == DIRECTION_LEFT))
												{
													array_CMD[1]=  DIRECTION_R_RIGHT;
												}
												else if((array_drvusart_bufR[2]<=array_drvusart_bufR[6])&&(Structset_GlobalSet_Increase4_4.Setting_Para == DIRECTION_RIGHT))
												{
													array_CMD[1]=  DIRECTION_R_RIGHT;
												}
												else if((array_drvusart_bufR[2]<=array_drvusart_bufR[6])&&(Structset_GlobalSet_Increase4_4.Setting_Para == DIRECTION_LEFT))
												{
													array_CMD[1]=  DIRECTION_L_LEFT;
												}
												array_CMD[2] = array_drvusart_bufR[4];//伸前梁	
												array_CMD[3] = array_drvusart_bufR[4];
												array_CMD[4]= 	array_drvusart_bufR[3];
												array_CMD[5] = array_drvusart_bufR[6];
												array_CMD[6] = 0;
												array_CMD[7] = Structset_Service_DeviceNum1_1.Setting_Para;

											xQueueSend(Unite_Action_Queue,&array_CMD,0);	
										
										
										
										
										}

									 
								break;
									 
								 
									 
								 case CANCEL:
									 FLAG_drvusart_romatecontrol = 0;
											State_main_TrumpetStatus = 1;
									LED_Management_Fanction(LED_NORMOL);
									/********************************改变设备受控状态---极为重要，本任务使用**********************************/
									_variant_Mainlogic_statement.PRESENTstate_value = State_Mainlogic_DWFAULTstate;
									_variant_Interface_disp.type = TYPE_DEFAULT;
									/********************************改变设备受控状态---极为重要，本任务使用**********************************/								
									xQueueSend(Interface_Mian_Logic_Queue,&const_Controled_InterfaceFlag,0);

									break;
								case GROUP_MOVE_CMD:
									

									 
								break;	 
									 
						 
							}
					

				}	
				else  if(array_drvusart_bufR[2] != variant_Service_DeviceNum)
                {
					switch(array_drvusart_bufR[1])
					{
						case CANCEL:
							
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
								array_CMD[0] = array_drvusart_bufR[1];
								array_CMD[1] = variant_Service_DeviceNum+3;
								array_CMD[2] = array_drvusart_bufR[3];
								array_CMD[3] = array_drvusart_bufR[4];
								array_CMD[4] = array_drvusart_bufR[5];
								array_CMD[5] = array_drvusart_bufR[6];
								array_CMD[6] = array_drvusart_bufR[7];
								array_CMD[7] = array_drvusart_bufR[8];
								
								xQueueSend(CAN_Send_Queue,&array_CMD,0);
						break;
						
						case PITCH_ON:
							if((array_drvusart_bufR[2] >= variant_Service_DeviceNum)||(array_drvusart_bufR[2] < variant_Service_DeviceNum+4))
							{
								if(FLAG_drvusart_romatecontrol == 1)
								{
									FLAG_drvusart_romatecontrol = 0;
											State_main_TrumpetStatus = 1;
									LED_Management_Fanction(LED_BLINK);
									/********************************改变设备受控状态---极为重要，本任务使用**********************************/
									_variant_Mainlogic_statement.PRESENTstate_value = State_Mainlogic_DWFAULTstate;
									_variant_Interface_disp.type = TYPE_DEFAULT;
									/********************************改变设备受控状态---极为重要，本任务使用**********************************/								
									xQueueSend(Interface_Mian_Logic_Queue,&const_Controled_InterfaceFlag,0);

								}
								
								array_CMD[0] = array_drvusart_bufR[1];
								array_CMD[1] = array_drvusart_bufR[2];
								array_CMD[2] = array_drvusart_bufR[3];
								array_CMD[3] = array_drvusart_bufR[4];
								array_CMD[4] = array_drvusart_bufR[5];
								array_CMD[5] = array_drvusart_bufR[6];
								array_CMD[6] = array_drvusart_bufR[7];
								array_CMD[7] = array_drvusart_bufR[8];
								
								xQueueSend(CAN_Send_Queue,&array_CMD,0);
								
							}
						break;
						case SINGAL_MOVE:
								array_CMD[0] = array_drvusart_bufR[1];
								array_CMD[1] = array_drvusart_bufR[2];
								array_CMD[2] = array_drvusart_bufR[3];
								array_CMD[3] = array_drvusart_bufR[4];
								array_CMD[4] = array_drvusart_bufR[5];
								array_CMD[5] = array_drvusart_bufR[6];
								array_CMD[6] = array_drvusart_bufR[7];
								array_CMD[7] = array_drvusart_bufR[8];
								
								xQueueSend(CAN_Send_Queue,&array_CMD,0);
									 
						break;
								
						case GROUP_CMD:
								array_CMD[0] = array_drvusart_bufR[1];
								array_CMD[1] = array_drvusart_bufR[2];
								array_CMD[2] = array_drvusart_bufR[3];
								array_CMD[3] = array_drvusart_bufR[4];
								array_CMD[4] = array_drvusart_bufR[5];
								array_CMD[5] = array_drvusart_bufR[6];
								array_CMD[6] = array_drvusart_bufR[7];
								array_CMD[7] = array_drvusart_bufR[8];
								
								xQueueSend(CAN_Send_Queue,&array_CMD,0);

									 
							break;
						case DEVICE_CONNECT://遥控器连接
										Romate_connect();
										Structset_Service_RemoteCortrol3_2.Setting_Para = 1;
										Set_Para_to_char(Structset_Service_RemoteCortrol3_2.Setting_Para,Structset_Service_RemoteCortrol3_2.veiw_type,Structset_Service_RemoteCortrol3_2.Setting_Char);
								break;
					}					
				}	
				memset(array_drvusart_bufR,0,20);			
			}
				
		}

	}

}
void Romate_connect(void)
{
	Structset_Service_RemoteCortrol3_2.Setting_Para = 0;
	array_drvusart_bufT[0] = 0x66;
	array_drvusart_bufT[1] = variant_Service_DeviceNum;
	array_drvusart_bufT[2] = 0;
	array_drvusart_bufT[3] = 0;
	array_drvusart_bufT[4] = 0;
	array_drvusart_bufT[5] = 0;
	array_drvusart_bufT[6] = 0;
	array_drvusart_bufT[7] = 0;
	array_drvusart_bufT[8] = 0;
	array_drvusart_bufT[9] = 0xbb;
	array_drvusart_bufT[10] = 0x66;
	array_drvusart_bufT[11] = variant_Service_DeviceNum;
	array_drvusart_bufT[12] = 0;
	array_drvusart_bufT[15] = 0;
	array_drvusart_bufT[16] = 0;
	array_drvusart_bufT[17] = 0;
	array_drvusart_bufT[18] = 0;
	array_drvusart_bufT[13] = 0;
	array_drvusart_bufT[14] = 0;
	array_drvusart_bufT[19] = 0xbb;
	uart_send(&com[0], array_drvusart_bufT, 20);


	

}
