#ifndef __BSP_485_H
#define __BSP_485_H

#include "stm32f4xx_hal.h"


#define Tx_uart2_Size 10





extern uint16_t CRC16(uint8_t * pMsg, uint16_t Len);

extern uint8_t variant_Controled_driveCMD[Tx_uart2_Size];
extern uint32_t variant_Controled_MoveCMD;
extern uint8_t variant_Controled_driveR[10];
extern uint8_t Flag_Controled_driveR;

void my_uart2_send_data(uint8_t *tdata,uint16_t tnum);
void usart2_niming_report(uint8_t len);
void MOVE_controled_Driver(uint32_t Recive);

#define RS485_T     HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_SET)
#define RS485_R     HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_RESET)
#endif
