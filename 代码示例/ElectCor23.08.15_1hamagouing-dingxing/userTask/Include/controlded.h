#ifndef __CONTROLDED_H__
#define __CONTROLDED_H__
#include "main.h"
#include "stm32f4xx_hal.h"
typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;

#define   CONTROLED_MOVE_ON    0x01
#define   CONTROLED_MOVE_OFF     0x00

#define   CONTROLED_MOVE_CONTINUE    0x01
#define   CONTROLED_MOVE_STOP     0x00

extern u8  state_Controled_MoveFlag;
extern  u8 state_Controled_ContiueFlag;


typedef struct _variant_controldedCMD1{
	u8 type;		
	u8 direction;		//��������
	u8 movetype;		//������������
	u8 num;		//  ���Ƶ��ڼ��ܵ���
	u8 back_Flag;  //
	u8 moveCMD;		//��������������
	u8 nc1;
	u8 nc2;
}_variant_controldedCMD1;

typedef struct _variant_controldedCMD2{
		u8 type0;		
	u8 direction1;		//���鷽��
	u8 alarmnum2;		//���鱨����
	u8 movenum3;		        //  ���鶯����Χ
	u8 moveCMD4;		    //��������������
	u8 limit5;		    //��������������
	u8 gap6;		    //�����
	u8 controlnum7;		    //���鶯��������id
}_variant_controldedCMD2;



union _unit_Controled_CMDR{

		uint8_t arrayCMD[8];
		struct _variant_controldedCMD1 Controled_singalCMD;
		struct _variant_controldedCMD2 Controled_GroupCMD;

};








void CMD_Control_Faction(void);
void Security_controled_Single(void);
extern u8 array_Controled_Reply[8];

uint16_t CRC16(uint8_t * pMsg, uint16_t Len);
void MOVE_controled_Driver(uint32_t Recive);










extern uint8_t state_controlded_movetype;





#endif





