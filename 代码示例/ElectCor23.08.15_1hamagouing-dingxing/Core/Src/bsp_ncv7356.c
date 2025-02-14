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

	/*CAN ɸѡ����ʼ��*/
	
	/*
	ʹ��ɸѡ�������ձ�־�����ݽ��бȶ�ɸѡ����չID 
	�������µľ����������ǵĻ�������� FIFO0�� */
	//ʹ��ɸѡ��
	CAN_FilterInitStructure.FilterActivation=ENABLE;
	//���ó�ʼ���Ĺ�������
	CAN_FilterInitStructure.FilterBank = 0;
	//ɸѡ���������� FIFO0
	CAN_FilterInitStructure.FilterFIFOAssignment=CAN_FILTER_FIFO0;
	//Ҫɸѡ�� ID ��λ
	CAN_FilterInitStructure.FilterIdHigh= 0x10<<5;
	//Ҫɸѡ�� ID ��λ
	CAN_FilterInitStructure.FilterIdLow= 0x0000<<5;
	//ɸѡ���� 16 λÿλ����ƥ��
	CAN_FilterInitStructure.FilterMaskIdHigh= 0X0000<<5;
	//ɸѡ���� 16 λÿλ����ƥ��
	CAN_FilterInitStructure.FilterMaskIdLow= 0X0000<<5;
	//����ģʽ
	/* �б�ģʽ CAN_FILTERMODE_IDLIST �ͱ�ʶ������λģʽ CAN_FILTERMODE_IDMASK */
	CAN_FilterInitStructure.FilterMode=CAN_FILTERMODE_IDLIST;
	//ɸѡ��λ��Ϊ���� 16 λ��
	CAN_FilterInitStructure.FilterScale=CAN_FILTERSCALE_16BIT;
	//���� CAN ��ʼ�洢��
	CAN_FilterInitStructure.SlaveStartFilterBank = 0;
	
	HAL_CAN_ConfigFilter(&hcan1,&CAN_FilterInitStructure);
	
	//can3�ǵ�����һ��can
	HAL_CAN_ConfigFilter(&hcan3,&CAN_FilterInitStructure);
	
	
	CAN_FilterInitStructure.FilterBank = 14;//��������
	CAN_FilterInitStructure.FilterFIFOAssignment=CAN_FILTER_FIFO1;
	CAN_FilterInitStructure.FilterIdHigh= Structset_Service_DeviceNum1_1.Setting_Para <<5;
	//Ҫɸѡ�� ID ��λ
	CAN_FilterInitStructure.FilterIdLow= 0x00ff<<5;
	//ɸѡ���� 16 λÿλ����ƥ��
	CAN_FilterInitStructure.FilterMaskIdHigh= 0X00a0<<5;
	//ɸѡ���� 16 λÿλ����ƥ��
	CAN_FilterInitStructure.FilterMaskIdLow= 0X00ef<<5;
	CAN_FilterInitStructure.SlaveStartFilterBank = 14;
	HAL_CAN_ConfigFilter(&hcan2,&CAN_FilterInitStructure);
	
	/* ��ʼCANģʽ */
	HAL_CAN_Start(&hcan1);
	HAL_CAN_Start(&hcan2);
	HAL_CAN_Start(&hcan3);
//	
	 
	/* ��CAN�����ж� */
	HAL_CAN_ActivateNotification(&hcan1,CAN_IT_RX_FIFO0_MSG_PENDING);
	HAL_CAN_ActivateNotification(&hcan2,CAN_IT_RX_FIFO1_MSG_PENDING);
	HAL_CAN_ActivateNotification(&hcan3,CAN_IT_RX_FIFO0_MSG_PENDING);
	
			xQueueReset(CAN2_Receive_ISR_Queue);
	

}

/**
 * @brief CAN����һ������
 * @param	CAN���
 * @param	֡ID
 * @param ����
 * @param ���ݳ���(���Ϊ 8����λ�ֽ�)
 * @retval 0,�ɹ�
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
		HAL_CAN_GetRxMessage(&hcan1,CAN_FILTER_FIFO0,&CAN1_RxHeader,can1_recv_buf);//��ȡ����
		xQueueSendFromISR(CAN_Receive_ISR_Queue,can1_recv_buf,&xHigherPriorityTaskWoken);
	}
	if(hcan->Instance==CAN3)
	{
		HAL_CAN_GetRxMessage(&hcan3,CAN_FILTER_FIFO0,&CAN3_RxHeader,can3_recv_buf);//��ȡ����
		xQueueSendFromISR(CAN_Receive_ISR_Queue,can3_recv_buf,&xHigherPriorityTaskWoken);
	}
}

void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	if(hcan->Instance==CAN2)
	{
		HAL_CAN_GetRxMessage(&hcan2,CAN_FILTER_FIFO1,&CAN2_RxHeader,can2_recv_buf);//��ȡ����
		xQueueSendFromISR(CAN_Bus_Receive_Queue,can2_recv_buf,&xHigherPriorityTaskWoken);
			if( can2_recv_buf[0] == 0xAA)
			{

				Clear_Screen();
				Display_String(3,32,"�������");
				__disable_irq(); //�ر������ж�
				NVIC_SystemReset(); //���������λ
				HAL_Delay(1000);		
			}

	}
	
}

