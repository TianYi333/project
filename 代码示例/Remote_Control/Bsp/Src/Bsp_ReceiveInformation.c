#include "Bsp_ReceiveInformation.h"

QueueHandle_t Queue_Zigbee_Receive_Handle;    //zigbee接收队列

BaseType_t xHigherPriorityTaskWoken = pdFALSE;

uint16_t m_crc;                 //计算出的crc

uint8_t msg_recv[12];           //暂存DMA数据
uint8_t zigbee_recv[8];         //保存去掉头尾的数据

/*zigbee接收任务*/
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
        //CRC错误
        memset(&Rx_uart3_buf[1] ,0, 11);
        continue;
      }
      memcpy(zigbee_recv, &msg_recv[1], 8);
////      if((zigbee_recv[0] & 0xF0) == 0xC0 && zigbee_recv[0] != 0xCC)
////      {
////        if(current_state == STATE_SELECTION || current_state == STATE_GROUP)
////        {
////          //参数刷新直接发向屏显任务
////          xQueueSend(Queue_Analyze_To_Display_Handle, zigbee_recv, 10);
////          memset(&Rx_uart3_buf[1] ,0, 11);
////          continue;
////        }
////      }
      
      if(zigbee_recv[0] == 0xCD)
      {
        if(current_state == STATE_SELECTION || current_state == STATE_GROUP)
        {
          //参数刷新直接发向屏显任务
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
      //错位时重启DMA
      HAL_UART_DMAStop(&huart3);
      HAL_UART_Receive_DMA(&huart3, Rx_uart3_buf, sizeof(Rx_uart3_buf)); 
    }
    vTaskDelay(10);
  }
}



