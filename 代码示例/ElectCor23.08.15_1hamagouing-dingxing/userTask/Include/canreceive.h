#ifndef __CANRECEIVE_H__
#define __CANRECEIVE_H__
#include "main.h"



typedef uint8_t  u8;

//typedef struct _variant_BUScheak1{
//	u8 head;		
//	u8 divicenum;		//��������
//	u8 Sensor;		//������������

//}_variant_BUScheak1;




/**********************************************���鶨��*******************************************************/
extern uint8_t array_Canreceive_CanLRcmdR[8];//�����������  �����˽��� ��������2023/02/3
extern uint8_t array_Canreceive_CanBUScmdR[8];//�����������  �����˽��� ��������2023/02/3
extern uint8_t array_Canreceive_CanLRinnerT[8];//�����������  �����˽��� ��������2023/02/3
extern uint8_t array_Canreceive_CanBUSinnerT[8];//�����������  �����˽��� ��������2023/02/3
extern uint8_t variant_BUS_continue;
extern uint8_t variant_Canreceive_STATE ;//������״̬   �����������  ����������� �������2023/12/24

void LRcan_Reveice_Fuction(void);

/*******************can�ڼ�ͨ��ǰ�÷���******************/
#define   TPYE_SINGAL               0x01      //����
#define   TPYE_GROUP                0x02      //����
#define   TPYE_SET_GLOBALPAR        0x03      //����ȫ�ֲ���
#define   TPYE_SERIALNUMBER_L       0x04      //�ܺŴ�����
#define   TPYE_PARAMETER            0x05      //����
#define   TPYE_PARAMETER_L          0x06      //������
#define   TPYE_PARAMETER_R          0x07      //������
#define   TPYE_GROUP_ALARM          0x08      //���鱨��
#define   TPYE_FOLLOW_STRAT         0x09      //������ʼ
#define   TPYE_SERIALNUMBER_R       0x0a      //�ܺŴ�����
#define   TPYE_GROUP_CANAEL         0x10      //����ȡ��
#define   TPYE_RESET                0x0b       //�ڼ�����

#define   PITCH_ON   0xc0  //ѡ��
#define   SINGAL_MOVE   0xc1  //����
#define   GROUP_MOVE_CMD   0xc2  //���鶯��
#define   GROUP_CMD   0xc5  //����ѡ��
#define   CANCEL   0xc3  //ȡ��
#define   DEVICE_CONNECT  0xc4  //ң������

#define   PLICHRETURN_1     0xcc
#define   PLICHRETURN_2     0xcd
#define   CANCELRETURN_1     0xca
#define   CANCELRETURN_2     0xcb


/*******************can���߽���ͨ��ǰ�÷���******************/
#define   BUS_R_PROGRAM            0xAA   //���򴫵�


/*******************can����ͨ��ǰ�÷���******************/
#define   BUS_STOP_LOCK            0x12   //��ͣ
#define   BUS_CHEAK_R            0xE1   //���߼��
#define   BUS_CHEAK_POSITION            0xE4   //���߼��


#define   BUS_FOLLOW_START           0x5A   //���߼��
#define   BUS_FOLLOW_FAIL           0x5E   //����ʧ��
#define   BUS_FOLLOW_STOP           0x5F   //����ֹͣ
#define   BUS_FOLLOW_MOVE           0x51   //���߸�������

#define   BUS_COMPUTER_MOVE           0x61   //���߸�������
#define   BUS_COMPUTER_CHOSE          0x60   //���߸�������
#define   BUS_COMPUTER_CANCEL          0x62   //���߸�������

/*******************can���� ���� ͨ��ǰ�÷���******************/
#define   BUS_CHEAK_SEND            0x01   //���߼��
#define   BUS_FOLLOWSTART_SEND            0x4A   //������ʼ���
#define   BUS_FOLLOWSTART_R_SEND            0x40   //���������ظ�
#define   BUS_FOLLOWMOVE_R_SEND            0x41   //���������ظ�
#define   BUS_FOLLOWMOVE_STOP            0x4D   //���������ظ�
#define   BUS_GROUP_STOP            0x02   //���������ظ�
/**********************************************���鶨��*******************************************************/





//�����Զ������Ʊ�־λ  ����
#define INLET_SAFEGUARD_BEGIN 0X01       //���ǰ����壨��ʼ��
//#define INLET_SAFEGUARD_PAUSE 0X02       //���ǰ����壨��ͣ��
//#define INLET_SAFEGUARD_STOP 0X03        //���ǰ����壨��ֹ��
//#define INLET_SAFEGUARD_END 0X04         //���ǰ����壨������

#define OUTLET_SAFEGUARD_BEGIN 0X06      //�ջ�ǰ����壨��ʼ��
//#define OUTLET_SAFEGUARD_PAUSE 0X07      //�ջ�ǰ����壨��ͣ��
//#define OUTLET_SAFEGUARD_STOP 0X08       //�ջ�ǰ����壨��ֹ��
//#define OUTLET_SAFEGUARD_END 0X09        //�ջ�ǰ����壨������

#define INLET_FRONTBEAM_BEGIN 0X11       //�������������ʼ��
//#define INLET_FRONTBEAM_PAUSE 0X12       //�������������ͣ��
//#define INLET_FRONTBEAM_STOP 0X13        //�������������ֹ��
//#define INLET_FRONTBEAM_END 0X14         //�����������������

#define OUTLET_FRONTBEAM_BEGIN 0X16      //�ջ�����������ʼ��
//#define OUTLET_FRONTBEAM_PAUSE 0X17      //�ջ�����������ͣ��
//#define OUTLET_FRONTBEAM_STOP 0X18       //�ջ�����������ֹ��
//#define OUTLET_FRONTBEAM_END 0X19        //�ջ���������������

#define PUSH_SCRAPER_BEGIN 0X21          //�����ʼ��
//#define PUSH_SCRAPER_PAUSE 0X22          //�����ͣ��
//#define PUSH_SCRAPER_STOP 0X23           //�����ֹ��
//#define PUSH_SCRAPER_END 0X24            //���������

#define AUTO_MOVE_MACHINE_BEGIN 0X36            //�Զ��Ƽܣ���ʼ������ǰ�����յ�λ���������������յ�λ��
//#define AUTO_MOVE_MACHINE_DECLINE_LEG 0X37      //�Զ��Ƽܣ��������ղ໤��
//#define AUTO_MOVE_MACHINE_PULL_SCRAPER 0X38     //�Զ��Ƽܣ����ܣ�
//#define AUTO_MOVE_MACHINE_RISE_LEG 0X39         //�Զ��Ƽܣ���໤��������
//#define AUTO_MOVE_MACHINE_INLET_SAFEGUARD 0X3A  //�Զ��Ƽܣ���ǰ����
//#define AUTO_MOVE_MACHINE_END 0X3C              //�Զ��Ƽܣ�������



/*******************can���߷���ͨ��ǰ�÷���******************/
/******************����������� �������2023/12/24******************/
#define   BUS_SEND_MOVE              0x21  //�ظ���λ��������ʼ��ʧ�ܣ���ֹ��״̬
#define   BUS_SEND_SINFORMATION      0x22 
#define   BUS_SEND_BUSCHEAK1         0x23 
#define   BUS_SEND_BUSCHEAK2         0x24 
#define   BUS_SEND_FAULT             0x25 
#define   BUS_SEND_STATE             0x28  //�ϱ�������״̬//һ�ֽڣ�У��λ�����ֽڣ��ܺţ����ֽڣ�״̬�š�

#define   BUS_STOP            0x12   //��ͣ  �㲥
#define   BUS_LOCK            0x13   //����   �ϱ���λ��


#define   BUS_R_CHEAK_R            0x26    //�յ���ظ���������
#define   BUS_R_POSITION           0x27    //�յ�ú��λ��

/*******************can��������ͨ��ǰ�÷���******************/

#define   BUS_FOLLOW_STARTCHEAK_R          0x3B   //��ͨ����ָʾ����λ������豸�Ƿ�����
#define   BUS_FOLLOW_FAIL_R           0x3E   //����ʧ��
#define   BUS_FOLLOW_STOP_R           0x3F   //����ֹͣ
#define   BUS_FOLLOW_MOVE_R           0x3C   //���߸�������
#define   BUS_FOLLOW_START_R          0x36   //��λ����������ָʾ��ͨ����λ���������ռ�����
/*******************can��������ͨ��ǰ�÷���******************/
#define   BUS_FOLLOW_STARTCHEAK_T          0x3B   //��ͨ�����ظ����������ظ���λ������豸�Ƿ�����
#define   BUS_FOLLOW_MOVE_T            0x21   //���������ظ�----��ʼ
#define   BUS_FOLLOW_START_T          0x36   //��λ����������ָʾ��ͨ����λ���������ռ�����




#define   BUS_COMPUTER_MOVE           0x61   //���߸�������  Զ�̵���
#define   BUS_COMPUTER_CHOSE          0x60   //���߸�������  Զ�̵���
#define   BUS_COMPUTER_CANCEL          0x62   //���߸�������  Զ�̵���

/*******************can���� ���� ͨ��ǰ�÷���******************/

#define   BUS_GROUP_STOP            0x02   //������ƽ�����־λ


//void Fuction_LRCANmessage_send(void);
void BUScan_Reveice_Fuction(void);
#endif



