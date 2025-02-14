#include "bsp_leds.h"

/**
 *@brief ����ָ��led
 *@param led��ţ����BSP_LEDS.h�ж���
 *@retval void
 */
void LEDS_LIGHT(uint8_t led_num)
{
	if((led_num>>0)&0x01){
		HAL_GPIO_WritePin(LS_PORT, LS_A_PIN, GPIO_PIN_SET);
	}
	else{
		HAL_GPIO_WritePin(LS_PORT, LS_A_PIN, GPIO_PIN_RESET);
	}
	
	if((led_num>>1)&0x01){
		HAL_GPIO_WritePin(LS_PORT, LS_B_PIN, GPIO_PIN_SET);
	}
	else{
		HAL_GPIO_WritePin(LS_PORT, LS_B_PIN, GPIO_PIN_RESET);
	}
	
	if((led_num>>2)&0x01){
		HAL_GPIO_WritePin(LS_PORT, LS_C_PIN, GPIO_PIN_SET);
	}
	else{
		HAL_GPIO_WritePin(LS_PORT, LS_C_PIN, GPIO_PIN_RESET);
	}
	
	if((led_num>>3)&0x01){
		HAL_GPIO_WritePin(LS_PORT, LS_D_PIN, GPIO_PIN_SET);
	}
	else{
		HAL_GPIO_WritePin(LS_PORT, LS_D_PIN, GPIO_PIN_RESET);
	}
	
	if((led_num>>4)&0x01){
		HAL_GPIO_WritePin(LS_PORT, LS_E_PIN, GPIO_PIN_SET);
	}
	else{
		HAL_GPIO_WritePin(LS_PORT, LS_E_PIN, GPIO_PIN_RESET);
	}
}
