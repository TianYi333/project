#ifndef __FOLLOW_H__
#define __FOLLOW_H__
#include "stm32f4xx_hal.h"

extern uint8_t variant_Follow_Cutterposition;//��ú��λ��
extern char array_Follow_Cutterposition[4];
extern uint8_t variant_Follow_Cutterderction;//��ú��λ��
extern char array_Follow_Cutterderction[5];
void Follow_move(void);
void Faction_Follow_UP(uint8_t upper_num1,uint8_t upper_num2,uint8_t upper_num3,uint8_t State);
extern uint8_t state_Follow_movememory;//���������Ƿ�ʼ

#endif



