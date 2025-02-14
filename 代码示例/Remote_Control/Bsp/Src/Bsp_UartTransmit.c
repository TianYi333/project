#include "Bsp_UartTransmit.h"

uint8_t battert_test[12] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
void Uart_Transmit_Function(void)
{
  uint8_t i;
  while(1)
  {
    vTaskDelay(300);
    HAL_UART_Transmit(&huart3, battert_test, 12, 10);
    i++;
    
    if(i == 5)
    {
      battert_test[0] = DIS_JUMP_MAIN;
      xQueueSend(Queue_Analyze_To_Display_Handle, battert_test, 10);
      i = 0;
    }
  }
}
