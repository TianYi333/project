#ifndef __DEFINE_H__
#define	__DEFINE_H__

//
#define Commun_ID 0x0010
#define	Convertor_ID 0x0100+Serve_Device_Num.Setting_Para  
#define PCID   0xee            //添加人任晓宇 添加日期2023/12/24

//支架状态宏定义

//蜂鸣器相关宏定义
#define BEEP_PORT		GPIOC
#define BEEP_PIN		GPIO_PIN_4
#define BEEP_ON 		HAL_GPIO_WritePin(BEEP_PORT,BEEP_PIN,GPIO_PIN_SET)
#define BEEP_OFF		HAL_GPIO_WritePin(BEEP_PORT,BEEP_PIN,GPIO_PIN_RESET)

//键盘板LED相关宏定义
#define LED_UNKOWNUP					0
#define LED_UP_DOWN						1



#endif


