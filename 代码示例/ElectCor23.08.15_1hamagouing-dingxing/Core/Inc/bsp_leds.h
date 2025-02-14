#ifndef __BSP_LEDS_H__
#define __BSP_LEDS_H__

#include "stm32f4xx_hal.h"

#define LS_PORT		GPIOB

#define LS_A_PIN	GPIO_PIN_4
#define LS_B_PIN	GPIO_PIN_5
#define LS_C_PIN	GPIO_PIN_6
#define LS_D_PIN	GPIO_PIN_7
#define LS_E_PIN	GPIO_PIN_8


#define KEY13_LED	3
#define KEY9_LED	7
#define KEY1_LED	11
#define KEY2_LED	12
#define KEY14_LED	2
#define KEY10_LED	6
#define KEY6_LED	10
#define KEY3_LED	13

#define KEY15_LED	1
#define KEY11_LED	5
#define KEY7_LED	9
#define KEY4_LED	14
#define KEY16_LED	0
#define KEY12_LED	4
#define KEY8_LED	8
#define KEY5_LED	15

#define KEY17_LED	29
#define KEY18_LED	23
#define KEY19_LED	22
#define KEY24_LED	16
#define KEY21_LED	25
#define KEY22_LED	24
#define KEY23_LED	21
#define KEY20_LED	20


#define UNKOWN_DOWN	27
#define KEY26_LED	18
#define KEY27_LED	19
#define KEY28_LED	17
#define UNKOWN_UP	28
#define KEY30_LED	31
#define KEY31_LED	30
#define KEY32_LED	26
void LEDS_LIGHT(uint8_t led_num);

#endif

