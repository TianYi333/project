#ifndef __BSP_KEYBOARD_H__
#define __BSP_KEYBOARD_H__

#include "main.h"

/* �궨��KEYxΪ��ȡ��Ӧkey�ĵ�ƽ */
/* ע��key�ı�Ų�����Ӧ�������ϵı��,����鿴keysö�� */
#define KEY1  HAL_GPIO_ReadPin(KEY1_GPIO_Port,  KEY1_Pin)
#define KEY2  HAL_GPIO_ReadPin(KEY2_GPIO_Port,  KEY2_Pin)
#define KEY3  HAL_GPIO_ReadPin(KEY3_GPIO_Port,  KEY3_Pin)
#define KEY4  HAL_GPIO_ReadPin(KEY4_GPIO_Port,  KEY4_Pin)
#define KEY5  HAL_GPIO_ReadPin(KEY5_GPIO_Port,  KEY5_Pin)
#define KEY6  HAL_GPIO_ReadPin(KEY6_GPIO_Port,  KEY6_Pin)
#define KEY7  HAL_GPIO_ReadPin(KEY7_GPIO_Port,  KEY7_Pin)
#define KEY8  HAL_GPIO_ReadPin(KEY8_GPIO_Port,  KEY8_Pin)
#define KEY9  HAL_GPIO_ReadPin(KEY9_GPIO_Port,  KEY9_Pin)
#define KEY10 HAL_GPIO_ReadPin(KEY10_GPIO_Port, KEY10_Pin)
#define KEY11 HAL_GPIO_ReadPin(KEY11_GPIO_Port, KEY11_Pin)
#define KEY12 HAL_GPIO_ReadPin(KEY12_GPIO_Port, KEY12_Pin)
#define KEY13 HAL_GPIO_ReadPin(KEY13_GPIO_Port, KEY13_Pin)
#define KEY14 HAL_GPIO_ReadPin(KEY14_GPIO_Port, KEY14_Pin)
#define KEY15 HAL_GPIO_ReadPin(KEY15_GPIO_Port, KEY15_Pin)
#define KEY16 HAL_GPIO_ReadPin(KEY16_GPIO_Port, KEY16_Pin)
#define KEY17 HAL_GPIO_ReadPin(KEY17_GPIO_Port, KEY17_Pin)
#define KEY18 HAL_GPIO_ReadPin(KEY18_GPIO_Port, KEY18_Pin)
#define KEY19 HAL_GPIO_ReadPin(KEY19_GPIO_Port, KEY19_Pin)
#define KEY20 HAL_GPIO_ReadPin(KEY20_GPIO_Port, KEY20_Pin)
#define KEY21 HAL_GPIO_ReadPin(KEY21_GPIO_Port, KEY21_Pin)
#define KEY22 HAL_GPIO_ReadPin(KEY22_GPIO_Port, KEY22_Pin)
#define KEY23 HAL_GPIO_ReadPin(KEY23_GPIO_Port, KEY23_Pin)


/* ��Ϣ���� */
extern QueueHandle_t Queue_KeyBoard_To_Analyze_Handle;    //�ð������������������Ϣ

enum keys
{
  ACTION7_7 = 1,  //KEY1    ����7
  ACTION8_8,      //KEY2    ����8
  ACTION2_2,      //KEY3    ����2
  ACTION1_1,      //KEY4    ����1
  LOCK,           //KEY5    ����
  ACTION3_3,      //KEY6    ����3
  ACTION4_4,      //KEY7    ����4
  ACTION5_5,      //KEY8    ����5
  ACTION9_9,      //KEY9    ����9
  ACTION10_0,     //KEY10   ����10
  CRASH_STOP,     //KEY11   ��ͣ
  UP,             //KEY12   ��
  ENTER,          //KEY13   ȷ��
  LEFT,           //KEY14   ��
  RIGHT,          //KEY15   ��
  DOWN,           //KEY16   ��
  CANCLE,         //KEY17   ȡ��
  ACTION6_6,      //KEY18   ����6
  GROUP,          //KEY19   ����
  GROUP_LEFT,     //KEY20   �����
  GROUP_RIGHT,    //KEY21   �ҳ���
  ACTION11,       //KEY22   
  SWITCH,         //KEY23   �л�
  KEY_NULL = 0xFF
};

/* ������ */
void KeyBoard_Function(void);		// �����庯��

//�Ƿ�ȴ�̧��
void Wait_lift(uint8_t key);
//��������
void Enter_Key(uint8_t key);

void Stop_Function(void);

extern uint8_t enter_key[24];



#endif
