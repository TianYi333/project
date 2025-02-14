#include "bsp_max3232.h"

void RS232_Transmit(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
	HAL_UART_Transmit(huart, pData, Size, Timeout);
}
