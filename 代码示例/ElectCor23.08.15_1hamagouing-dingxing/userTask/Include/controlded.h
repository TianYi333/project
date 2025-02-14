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
	u8 direction;		//单动方向
	u8 movetype;		//单动动作类型
	u8 num;		//  控制到第几架单动
	u8 back_Flag;  //
	u8 moveCMD;		//单动动作命令码
	u8 nc1;
	u8 nc2;
}_variant_controldedCMD1;

typedef struct _variant_controldedCMD2{
		u8 type0;		
	u8 direction1;		//成组方向
	u8 alarmnum2;		//成组报警数
	u8 movenum3;		        //  成组动作范围
	u8 moveCMD4;		    //单动动作命令码
	u8 limit5;		    //单动动作命令码
	u8 gap6;		    //隔离架
	u8 controlnum7;		    //成组动作发起者id
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





