#include "Bsp_ReceiveInformation.h"

QueueHandle_t Queue_Zigbee_Receive_Handle;    //zigbee���ն���

BaseType_t xHigherPriorityTaskWoken = pdFALSE;

uint16_t m_crc;                 //�������crc

uint8_t msg_recv[12];           //�ݴ�DMA����
uint8_t zigbee_recv[8];         //����ȥ��ͷβ������

/*zigbee��������*/
void Zigbee_Receive_Function(void)
{
  while(1)
  {
    if(Rx_uart3_buf[0] == 0x66 && Rx_uart3_buf[9] == 0xAA)
    {
      memcpy(msg_recv, Rx_uart3_buf, 12);
      m_crc = CRC16(msg_recv, 10);
      if(m_crc != ((msg_recv[10] << 8) + (msg_recv[11])))
      {
        //CRC����
        memset(&Rx_uart3_buf[1] ,0, 11);
        continue;
      }
      memcpy(zigbee_recv, &msg_recv[1], 8);
////      if((zigbee_recv[0] & 0xF0) == 0xC0 && zigbee_recv[0] != 0xCC)
////      {
////        if(current_state == STATE_SELECTION || current_state == STATE_GROUP)
////        {
////          //����ˢ��ֱ�ӷ�����������
////          xQueueSend(Queue_Analyze_To_Display_Handle, zigbee_recv, 10);
////          memset(&Rx_uart3_buf[1] ,0, 11);
////          continue;
////        }
////      }
      
      if(zigbee_recv[0] == 0xCD)
      {
        if(current_state == STATE_SELECTION || current_state == STATE_GROUP)
        {
          //����ˢ��ֱ�ӷ�����������
          xQueueSend(Queue_Analyze_To_Display_Handle, zigbee_recv, 10);
          memset(&Rx_uart3_buf[1] ,0, 11);
          continue;
        }
      }
      xQueueSend(Queue_Zigbee_Receive_Handle, zigbee_recv, 100);
      memset(&Rx_uart3_buf[1] ,0, 11);
    }
    else if(Rx_uart3_buf[0] != 0x66)
    {
      //��λʱ����DMA
      HAL_UART_DMAStop(&huart3);
      HAL_UART_Receive_DMA(&huart3, Rx_uart3_buf, sizeof(Rx_uart3_buf)); 
    }
    vTaskDelay(10);
  }
}



