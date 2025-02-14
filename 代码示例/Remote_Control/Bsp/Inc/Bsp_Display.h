#ifndef __BSP_DISPLAY_H__
#define __BSP_DISPLAY_H__

#include "main.h"

/*----------------------------------------------------------------------------------*/
#define START_UP_DELAY 2000       //������ʱ

#define START_PIC_DELAY 5000      //����ͼƬ��ʾʱ��

/*��ɫ���--Ĭ��*/        
#define DEFAULT_COLOR_BACK               34//����
#define DEFAULT_COLOR_TEXT               15    //Ĭ����ɫ�����֡����֣�
#define DEFAULT_COLOR_CHOOSE_BACK        46    //ѡ�����ֵı���ɫ
#define DEFAULT_COLOR_NUM_BACK           0     //ѡ�мܺ����ֵı���ɫ
#define DEFAULT_COLOR_CONNECTION         1     //����������ɫ
/*������ѡ�н�����ʾ�ܺŵ�����*/
#define MAIN_ELENUM_STR_X   18    //����
#define MAIN_ELENUM_X       120
#define MAIN_ELENUM_Y       42
/*��ʾ�������ֵ�����*/
#define MAIN_CONNECTION_X   10    //����
#define MAIN_CONNECTION_Y   0
/*����������*/
#define FIRST_LINE_X     180
#define FIRST_LINE_Y     42
/*ѡ�в˵�֧�ܲ������꣨������ú��λ�ã�*/
#define CHOOSE_PARA_STR_X             18    //ȫ�����ֵ�X��
#define CHOOSE_PARA_MINUS_X           90    //���ŵ�X��
#define CHOOSE_PARA_NUM_X             120   //ȫ�����ֵ�X��
#define CHOOSE_PARA_LOCATION_Y        68    //��ú��λ��
#define CHOOSE_PARA_DISTANCE_Y        94    //�г̴�����
#define CHOOSE_PARA_PRESSURE_Y        120   //ѹ��������
#define CHOOSE_PARA_INCLINATION_Y1    146   //��Ǵ�����1
#define CHOOSE_PARA_INCLINATION_Y2    172   //��Ǵ�����2
#define CHOOSE_PARA_INCLINATION_Y3    198   //��Ǵ�����3

/*ѡ�в˵��б��и�ѡ�������*/
#define CHOOSE_MENU_ITEM_X    180  
#define CHOOSE_MENU_ITEM_Y1   68
#define CHOOSE_MENU_ITEM_Y2   94
#define CHOOSE_MENU_ITEM_Y3   120
#define CHOOSE_MENU_ITEM_Y4   146
#define CHOOSE_MENU_ITEM_Y5   172
#define CHOOSE_MENU_ITEM_Y6   198
/*����˵��б��и�ѡ�������*/
#define GROUP_MENU_ITEM_X    180  
#define GROUP_MENU_ITEM_Y1   68
#define GROUP_MENU_ITEM_Y2   94
#define GROUP_MENU_ITEM_Y3   120
#define GROUP_MENU_ITEM_Y4   146
#define GROUP_MENU_ITEM_Y5   172
#define GROUP_MENU_ITEM_Y6   198

/*����˵�������ʾ����*/
#define GROUP_ORIENTATION_SX        21    //����X
#define GROUP_ORIENTATION_SY        68    //����Y
#define GROUP_ORIENTATION_Y         94    //���ּ���־Y
#define GROUP_ORIENTATION_NUM1      20    //����1
#define GROUP_ORIENTATION_NUM2      120   //����2
#define GROUP_ORIENTATION_STR       55    //�����־

/*�͵�����ʾ������*/                
#define POWER_DIS_X             180
#define POWER_DIS_Y             0


/*dis_msg*/
#define DIS_CLEAN               0x00    //����
#define DIS_JUMP_MAIN           0x01    //��ת��������
#define DIS_DIR_LR              0x03    //���������(��/��)
#define DIS_DIR_UP              0x04    //���������(��)
#define DIS_DIR_DOWN            0x05    //���������(��)
#define DIS_JUMP_GROUP          0x06    //��ת���������
#define DIS_CLEAN_RED_MESSAGE   0xEA    //ȥ����ɫ��ʾ��Ϣ
#define DIS_CONNECTING          0xE0    //������
#define DIS_CONNECTION_FAILED   0xE1    //����ʧ��
#define DIS_IN_ACTION           0xE2    //������
#define DIS_MAIN_NUM            0x10    //��������ʾ��ѡ�ܺ�
#define DIS_GROUP_TO_LEFT       0x21    //������������仯
#define DIS_GROUP_TO_RIGHT      0x22    //��������ҳ���仯
#define DIS_NO_DIR              0xEE    //����״̬����ȷ��ʱδѡ����

#define DIS_JUMP_CHOOSE         0xCC    //��ת��ѡ�н���
#define DIS_PARA_REFRESH_2      0xCD    //����ˢ��
//#define DIS_PARA_REFRESH_3      0xCA    //����ˢ��
//#define DIS_PARA_REFRESH_4      0xCB    //����ˢ��
//#define DIS_ONGOING             0xA0    //��ʾ���ڽ��еĶ���

#define LOW_POWER               0xBB    //��������
#define HIGH_POWER              0xBC    //��������
/*-------------------------------------------------------------------------------------*/

#define   MOVE_NONE                                0          //��
#define   MOVE_COLUMN_UP                           0x01       //������
#define   MOVE_COLUMN_DOWN                         0x02       //������
#define   MOVE_HYDRAULICFRAME_ADVANCING            0x03       //�Ƽ�
#define   MOVE_CHUTE_PUSH                          0x04       //����
#define   MOVE_BOTTOM_UP                           0x05       //̧��
#define   MOVE_FUEL_SPRAY                          0x06       //����
#define   MOVE_UPLIFT_BOTTOM_COLUMN_DOWN           0x07       //̧�׽���
#define   MOVE_UPLIFT_BOTTOM_HYDRAULIC_ADVANCING   0x08       //̧���Ƽ�
#define   MOVE_BALANCE_BEAM_STRETCH                0x09       //��ƽ����
#define   MOVE_BALANCE_BEAM_WITHDRAW               0x0a       //��ƽ����
#define   MOVE_SIDE_PLATE_STRETCH                  0x0b       //�����
#define   MOVE_SIDE_PLATE_WITHDRAW                 0x0c       //�ղ���
#define   MOVE_TELESCOPIC_GIRDER_STRETCH           0x0d       //��������
#define   MOVE_TELESCOPIC_GIRDER_WITHDRAW          0x0e       //��������
#define   MOVE_PLATE_STRETCH                       0x0f       //�커���
#define   MOVE_PLATE_WITHDRAW                      0x10       //�ջ����
#define   MOVE_HYDRAULIC_BOTTOM_STRETCH            0x11       //��׵�
#define   MOVE_HYDRAULIC_BOTTOM_WITHDRAW           0x12       //�յ׵�
#define   MOVE_DROP_AD_UP                          0x13       //������

/*�������Ƽ�ָ�24����*/
#define ACTION_0    "������  "
#define ACTION_0_ACT   MOVE_TELESCOPIC_GIRDER_WITHDRAW
#define ACTION_1    "����    "
#define ACTION_1_ACT   MOVE_COLUMN_UP
#define ACTION_2    "����    "
#define ACTION_2_ACT   MOVE_CHUTE_PUSH
#define ACTION_3    "��ƽ��  "
#define ACTION_3_ACT   MOVE_BALANCE_BEAM_STRETCH
#define ACTION_4    "�커��  "
#define ACTION_4_ACT   MOVE_PLATE_STRETCH
#define ACTION_5    "������  "
#define ACTION_5_ACT   MOVE_TELESCOPIC_GIRDER_STRETCH
#define ACTION_6    "����    "
#define ACTION_6_ACT   MOVE_COLUMN_DOWN
#define ACTION_7    "����    "
#define ACTION_7_ACT   MOVE_HYDRAULICFRAME_ADVANCING
#define ACTION_8    "��ƽ��  "
#define ACTION_8_ACT   MOVE_BALANCE_BEAM_WITHDRAW
#define ACTION_9    "�ջ���  "
#define ACTION_9_ACT   MOVE_PLATE_WITHDRAW
#define ACTION_10   "̧��    "
#define ACTION_10_ACT   MOVE_BOTTOM_UP
#define ACTION_11   "����    "
#define ACTION_11_ACT   MOVE_FUEL_SPRAY
#define ACTION_12   "�����  "
#define ACTION_12_ACT   MOVE_SIDE_PLATE_STRETCH
#define ACTION_13   "�ղ���  "
#define ACTION_13_ACT   MOVE_SIDE_PLATE_WITHDRAW
#define ACTION_14   "��׵�  "
#define ACTION_14_ACT   MOVE_HYDRAULIC_BOTTOM_STRETCH
#define ACTION_15   "�յ׵�  "
#define ACTION_15_ACT   MOVE_HYDRAULIC_BOTTOM_WITHDRAW
#define ACTION_16   "̧�׽���"
#define ACTION_16_ACT   MOVE_UPLIFT_BOTTOM_COLUMN_DOWN
#define ACTION_17   "̧���Ƽ�"
#define ACTION_17_ACT   MOVE_UPLIFT_BOTTOM_HYDRAULIC_ADVANCING
/*δʹ��*/
#define ACTION_18   "����18"
#define ACTION_19   "����19"
#define ACTION_20   "����20"
#define ACTION_21   "����21"
#define ACTION_22   "����22"
#define ACTION_23   "����23"

/*�������Ƽ�ָ�6����*/

#define   MOVE_GROUP_ADVANCING                           0x20          //�Ƽ�
#define   MOVE_GROUP_CHUTE_PUSH                          0x21          //����
#define   MOVE_GROUPTELESCOPICPLATE_GIRDER_WITHDRAW      0x24         //�������������
#define   MOVE_GROUP_CHUTE_PULL                          0x22         //����
#define   MOVE_GROUP_TELESCOPIC_GIRDER_STRETCH           0x23          //��������
#define   MOVE_GROUP_PLATE_STRETCH                       0x25          //�커���

#define G_ACTION_0    "�Զ��Ƽ�"
#define G_ACTION_0_ACT   MOVE_GROUP_ADVANCING
#define G_ACTION_1    "����    "
#define G_ACTION_1_ACT   MOVE_GROUP_CHUTE_PUSH
#define G_ACTION_2    "��������"
#define G_ACTION_2_ACT   MOVE_GROUPTELESCOPICPLATE_GIRDER_WITHDRAW
#define G_ACTION_3    "����    "
#define G_ACTION_3_ACT   MOVE_GROUP_CHUTE_PULL
#define G_ACTION_4    "��������"
#define G_ACTION_4_ACT   MOVE_GROUP_TELESCOPIC_GIRDER_STRETCH
#define G_ACTION_5    "�커���"
#define G_ACTION_5_ACT   MOVE_GROUP_PLATE_STRETCH

/*δʹ��*/
#define G_ACTION_6    "����06  "
#define G_ACTION_6_ACT   0x57
#define G_ACTION_7    "����07  "
#define G_ACTION_7_ACT   0x58

////#define G_ACTION_0    "�����Զ��Ƽ�"
////#define G_ACTION_0_ACT   0x51
////#define G_ACTION_1    "��������    "
////#define G_ACTION_1_ACT   0x52
////#define G_ACTION_2    "������������"
////#define G_ACTION_2_ACT   0x53
////#define G_ACTION_3    "������������"
////#define G_ACTION_3_ACT   0x54
////#define G_ACTION_4    "�����ղ໤��"
////#define G_ACTION_4_ACT   0x55
////#define G_ACTION_5    "������໤��"
////#define G_ACTION_5_ACT   0x56
/////*δʹ��*/
////#define G_ACTION_6    "���鶯��06  "
////#define G_ACTION_6_ACT   0x57
////#define G_ACTION_7    "���鶯��07  "
////#define G_ACTION_7_ACT   0x58

/*----------------------------------------------------------------------------------*/

#define GROUP_TO_STR    "����֧��"
#define GROUP_TO_NONE   "----"
#define GROUP_TO_LEFT   "<-<-"
#define GROUP_TO_RIGHT  "->->"

/*----------------------------------------------------------------------------------*/

extern QueueHandle_t Queue_Analyze_To_Display_Handle;    //������������ʾ������Ϣ�����ڿ�����Ļ��ʾ��

/* ������ */
void Display_Function(void);		// ��ʾ����
/*ѡ�н����ʼ��*/
void Selection_Menu_Init(void); 
/*��������ʼ��*/
void Group_Menu_Init(void); 
/*�ȴ���Ļ��������*/
void CheckBusy(void);           

/*��������ɫ��*/
void Display_Clean(uint8_t color, uint8_t ifmain);
/*�������ֵ�ɫ����ɫ��*/
void Display_Set_Str_Back(uint8_t color);

/*��Ļ��ʾ���֣�x��y�����֣���ɫ����ɫ��----color��15Ϊ��ɫ��0Ϊ��ɫ*/
void Display_Num(uint16_t x, uint16_t y, uint8_t num, uint8_t color, uint8_t back);

/*
��Ļ��ʾ�ַ�����x��y���ַ�������ɫ����С����ɫ��
color��15Ϊ��ɫ��0Ϊ��ɫ
large��16��24��32��48
back����ɫ��Ϊ0x88ʱ��ʾΪ�޵�ɫ
*/
void Display_Str(uint16_t x, uint16_t y, char *str, uint16_t color, uint16_t large, uint16_t back);
/*��ʾ��ʼ����----����˵����ú��λ��*/
void Display_Main_Menu(void);
/*�����ɫ��ʾ��Ϣ*/
void Display_Clean_Red_Message(void);
/*��Ļ��ʾ������*/
void Display_Connecting(void);
/*��Ļ��ʾ����ʧ��*/
void Display_Connection_Failed(void);
/*��Ļ��ʾ������*/
void Display_In_Action(void);
/*��Ļ��ʾδѡ����*/
void Display_No_Dir(void);
/*��Ļ��ʾѡ�н��棨ѡ�еļܺţ�Ĭ�϶�����ţ�*/
void Display_Choose_Menu(uint8_t num, uint8_t action);
/*ѡ�н���ܺŸı䣨�ܺţ�*/
void Display_Choose_Menu_Num_Change(uint8_t num);
/*��ʾѡ�н���֧�ܲ���*/
void Display_Choose_Para_Item(uint8_t para[8]);
/*��ʾѡ�н����б��еĵ���߶ȣ���ţ�*/
void Display_Choose_Menu_Item(uint8_t height, uint8_t num);
/*��ʾ�б�仯*/
void Display_Choose_Menu_Change_List(uint8_t direction, uint8_t action);
/*��Ļ��ʾ������棨ѡ�еļܺţ�Ĭ�϶�����ţ�*/
void Display_Group_Menu(uint8_t num, uint8_t action);
/*��ʾ��������б��еĵ���߶ȣ���ţ�*/
void Display_Group_Menu_Item(uint8_t height, uint8_t num);
/*��ʾ������涯����Χ������*/
void Display_Group_Serial_Num(uint8_t key);
/*��ʾ�͵�����ʾ��Ϣ*/
void Display_Low_Power(uint8_t power);



/*----------------------------------------------------------------------------------*/
struct selection    //ѡ���б�ṹ��
{
  uint8_t action;       //�������
  uint8_t picnum;       //��ͼ���
  char *action_string;  //��ʾ����
};

/*----------------------------------------------------------------------------------*/

extern char *pstr[256];

extern struct selection selection_menu[25];
extern struct selection group_menu[8];   

extern uint8_t last_dir;   //�ϴεķ���----0x00���ޣ�0x11��С��С��0x12��С����0x21����С��0x22���󣬴�

extern uint8_t last_group_dir; //��¼���һ�ΰ��µ�����/�ҳ���
extern uint8_t group_other_num;       //���鷽�����һ̨�ļܺ�
extern uint8_t group_last_num;        //����������ļܺ�

#endif
