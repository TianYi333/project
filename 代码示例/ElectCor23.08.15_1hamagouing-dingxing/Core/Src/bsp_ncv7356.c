#include "bsp_ncv7356.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "bsp_oled19264.h"
#include "define.h"
#include "parameter_operation.h"
#include "main.h"
#include "set.h"
CAN_TxHeaderTypeDef CAN_TxHeader;

CAN_RxHeaderTypeDef CAN1_RxHeader;
CAN_RxHeaderTypeDef CAN2_RxHeader;
CAN_RxHeaderTypeDef CAN3_RxHeader;
uint8_t Flag_ncv7356_cansendeorr = 0;
uint8_t can1_recv_buf[8];
uint8_t can2_recv_buf[8];
uint8_t can3_recv_buf[8];
uint32_t rel_can;

extern QueueHandle_t CAN_Receive_ISR_Queue;
//extern QueueHandle_t CAN1_Receive_ISR_Queue;
extern QueueHandle_t CAN2_Receive_ISR_Queue;
//extern QueueHandle_t CAN3_Receive_ISR_Queue;
extern QueueHandle_t CAN_Bus_Receive_Queue;
BaseType_t xHigherPriorityTaskWoken;

void CAN_Config(void)
{
	CAN_FilterTypeDef CAN_FilterInitStructure;

	/*CAN 筛选器初始化*/
	
	/*
	使能筛选器，按照标志的内容进行比对筛选，扩展ID 
	不是如下的就抛弃掉，是的话，会存入 FIFO0。 */
	//使能筛选器
	CAN_FilterInitStructure.FilterActivation=ENABLE;
	//设置初始化的过滤器组
	CAN_FilterInitStructure.FilterBank = 0;
	//筛选器被关联到 FIFO0
	CAN_FilterInitStructure.FilterFIFOAssignment=CAN_FILTER_FIFO0;
	//要筛选的 ID 高位
	CAN_FilterInitStructure.FilterIdHigh= 0x10<<5;
	//要筛选的 ID 低位
	CAN_FilterInitStructure.FilterIdLow= 0x0000<<5;
	//筛选器高 16 位每位必须匹配
	CAN_FilterInitStructure.FilterMaskIdHigh= 0X0000<<5;
	//筛选器低 16 位每位必须匹配
	CAN_FilterInitStructure.FilterMaskIdLow= 0X0000<<5;
	//工作模式
	/* 列表模式 CAN_FILTERMODE_IDLIST 和标识符屏蔽位模式 CAN_FILTERMODE_IDMASK */
	CAN_FilterInitStructure.FilterMode=CAN_FILTERMODE_IDLIST;
	//筛选器位宽为单个 16 位。
	CAN_FilterInitStructure.FilterScale=CAN_FILTERSCALE_16BIT;
	//设置 CAN 起始存储区
	CAN_FilterInitStructure.SlaveStartFilterBank = 0;
	
	HAL_CAN_ConfigFilter(&hcan1,&CAN_FilterInitStructure);
	
	//can3是单独的一个can
	HAL_CAN_ConfigFilter(&hcan3,&CAN_FilterInitStructure);
	
	
	CAN_FilterInitStructure.FilterBank = 14;//过滤器组
	CAN_FilterInitStructure.FilterFIFOAssignment=CAN_FILTER_FIFO1;
	CAN_FilterInitStructure.FilterIdHigh= Structset_Service_DeviceNum1_1.Setting_Para <<5;
	//要筛选的 ID 低位
	CAN_FilterInitStructure.FilterIdLow= 0x00ff<<5;
	//筛选器高 16 位每位必须匹配
	CAN_FilterInitStructure.FilterMaskIdHigh= 0X00a0<<5;
	//筛选器低 16 位每位必须匹配
	CAN_FilterInitStructure.FilterMaskIdLow= 0X00ef<<5;
	CAN_FilterInitStructure.SlaveStartFilterBank = 14;
	HAL_CAN_ConfigFilter(&hcan2,&CAN_FilterInitStructure);
	
	/* 开始CAN模式 */
	HAL_CAN_Start(&hcan1);
	HAL_CAN_Start(&hcan2);
	HAL_CAN_Start(&hcan3);
//	
	 
	/* 打开CAN接收中断 */
	HAL_CAN_ActivateNotification(&hcan1,CAN_IT_RX_FIFO0_MSG_PENDING);
	HAL_CAN_ActivateNotification(&hcan2,CAN_IT_RX_FIFO1_MSG_PENDING);
	HAL_CAN_ActivateNotification(&hcan3,CAN_IT_RX_FIFO0_MSG_PENDING);
	
			xQueueReset(CAN2_Receive_ISR_Queue);
	

}

/**
 * @brief CAN发送一组数据
 * @param	CAN编号
 * @param	帧ID
 * @param 数据
 * @param 数据长度(最大为 8，单位字节)
 * @retval 0,成功
 */ 
uint8_t CANx_SendNormalData(CAN_HandleTypeDef *hcanX, uint16_t ID, uint8_t *pData, uint16_t Len)
{
	uint16_t Can_free_time = 0;
    HAL_StatusTypeDef   HAL_RetVal;
    uint16_t SendTimes, SendCNT = 0;
    CAN_TxHeader.StdId = ID;
    SendTimes = Len / 8 + (Len % 8 ? 1 : 0);
    CAN_TxHeader.DLC = 8;
    while(SendTimes--)
		{
			if(0 == SendTimes)
			{
				if(Len % 8)
				CAN_TxHeader.DLC = Len % 8;
			}
//			FreeTxNum = HAL_CAN_GetTxMailboxesFreeLevel(hcanX);
//			while(0 == FreeTxNum){
//				HAL_Delay(1);
//				FreeTxNum = HAL_CAN_GetTxMailboxesFreeLevel(hcanX);
//				if(0 == FreeTxNum)
//				{
//					return 2;
//				}
//			}
			while(( HAL_CAN_GetTxMailboxesFreeLevel(hcanX) == 0 )&&(Can_free_time<1000))
			{
					Can_free_time++;
				
			};
			if(Can_free_time>=999)
			{
				Can_free_time = 0;
				Flag_ncv7356_cansendeorr = 1;
				
				return 3;
			
			}
//			HAL_Delay(1); 			
			HAL_RetVal = HAL_CAN_AddTxMessage(hcanX, &CAN_TxHeader, pData + SendCNT, &rel_can);
      if(HAL_RetVal != HAL_OK)
			{
				return 2;
      }
      SendCNT += 8;
    }
    return 0;
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	if(hcan->Instance==CAN1)
	{
		HAL_CAN_GetRxMessage(&hcan1,CAN_FILTER_FIFO0,&CAN1_RxHeader,can1_recv_buf);//获取数据
		xQueueSendFromISR(CAN_Receive_ISR_Queue,can1_recv_buf,&xHigherPriorityTaskWoken);
	}
	if(hcan->Instance==CAN3)
	{
		HAL_CAN_GetRxMessage(&hcan3,CAN_FILTER_FIFO0,&CAN3_RxHeader,can3_recv_buf);//获取数据
		xQueueSendFromISR(CAN_Receive_ISR_Queue,can3_recv_buf,&xHigherPriorityTaskWoken);
	}
}

void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	if(hcan->Instance==CAN2)
	{
		HAL_CAN_GetRxMessage(&hcan2,CAN_FILTER_FIFO1,&CAN2_RxHeader,can2_recv_buf);//获取数据
		xQueueSendFromISR(CAN_Bus_Receive_Queue,can2_recv_buf,&xHigherPriorityTaskWoken);
			if( can2_recv_buf[0] == 0xAA)
			{

				Clear_Screen();
				Display_String(3,32,"程序更新");
				__disable_irq(); //关闭所有中断
				NVIC_SystemReset(); //进行软件复位
				HAL_Delay(1000);		
			}

	}
	
}

