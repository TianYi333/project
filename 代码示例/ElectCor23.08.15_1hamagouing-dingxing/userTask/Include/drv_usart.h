#ifndef __DRV_USART_H
#define __DRV_USART_H

#define UART_USING_DMA

#include "main.h"


typedef void(*func_cb)(void*);

typedef struct _hw_com_port{

	uint8_t rx_over_flag;
	uint8_t rx_count;
	uint8_t *rx_ptr;
	uint8_t *tx_ptr;
	
	UART_HandleTypeDef *uartx;
	DMA_HandleTypeDef		*hdma_rx;
	DMA_HandleTypeDef		*hdma_tx;
	uint8_t singlechar;				//中断接收临时字节
	uint8_t rx_tim_count;			//超时认为一帧
	uint8_t rx_sta;						//开始计时标志
	HAL_StatusTypeDef (*send)(struct _hw_com_port* port, uint8_t *buffer, uint32_t length);
	func_cb	cb;
}hw_com_port;

extern DMA_HandleTypeDef hdma_usart6_rx;
extern DMA_HandleTypeDef hdma_usart6_tx;
extern DMA_HandleTypeDef hdma_usart2_rx;
extern DMA_HandleTypeDef hdma_usart2_tx;

extern HAL_StatusTypeDef uart_send(struct _hw_com_port* port, uint8_t *buffer, uint32_t length);
extern void uart_rx_callback(struct _hw_com_port* port);
extern void uart_tx_callback(struct _hw_com_port* port);
extern void uart_tm_callback(struct _hw_com_port* port);

extern void hw_usart_register(UART_HandleTypeDef *uart);
extern void uart_callback_function(UART_HandleTypeDef *huart);


extern struct _hw_com_port com[2];
extern uint8_t array_drvusart_bufR[20];
extern uint8_t array_drvusart_bufT[20];
void Romate_control_reception(void);
void Romate_connect(void);
HAL_StatusTypeDef uart_send(struct _hw_com_port* port, uint8_t *buffer, uint32_t length);


extern uint8_t FLAG_drvusart_romatecontrol;//电液控被遥控选中标志位



#endif


