#ifndef __BSP_ANALYZE_H__
#define __BSP_ANALYZE_H__

#include "main.h"

#define STATE_IDEL        0x00    //����״̬
#define STATE_SELECTION   0x01    //ѡ�мܺ�״̬
#define STATE_SINGLE      0x02    //����״̬
#define STATE_GROUP       0x03    //����״̬

#define MESSAGE_CONNECT       0xC0    //����ָ���ܺ�
#define GROUP_CONNECT         0xC2    //����ָ���ܺ�
#define SIGNAL_SEND_MESSAGE   0xC1    //������־
#define GROUP_SEND_MESSAGE    0xC5    //�����־

/*��С/���ܺ�*/
#define NUM_MIN 1
#define NUM_MAX 102

/*������С/�����*/
#define ACTION_MIN 1
#define ACTION_MAX 18       //�ݶ�Ϊ18������
/*������С/�����*/
#define G_ACTION_MIN 1
#define G_ACTION_MAX 6      //�ݶ�Ϊ6�����鶯��

extern QueueHandle_t Queue_Step_Stop_Handle;    //ֹͣ������Ϣ

//������ʾ������
extern int16_t print_num;
//��ǰ���涯��
extern uint8_t choose_action;
//���鷽��
extern uint8_t m_dir;
//����ʱ�������λ��
extern uint8_t group_choose;

/*��¼��ǰ״̬
  0������״̬��
  1��ѡ�мܺ�״̬��
*/
extern uint8_t current_state;

extern struct group_msg group_para;

struct group_msg
{
  uint8_t dir; //��ַ����    1����2����
  int8_t qua[6]; //����֧������
};


/* ������ */
void Analyze_Function(void);		//��������

/*���鰴��������*/
void Key_Group(void);


#endif
