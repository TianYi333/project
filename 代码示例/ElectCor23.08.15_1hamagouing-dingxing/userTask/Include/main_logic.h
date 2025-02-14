#ifndef __MAIN_LOGIC_H__
#define __MAIN_LOGIC_H__
#include "stm32f4xx_hal.h"
#include "parameter_operation.h"

typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;

/*******************_variant_Mainlogic_statement.PRESENTstate�ã�Ϊ��ǰ״̬******************/
#define   URGENCY_STOP    0x00
#define   NORMOL_IDLE     0x01
#define   AUTO_IDLE       0x02
#define   SETTING       0x03
#define   CONTROLED       0x04
#define   GROUP_CONTROLED       0x07
#define   FREEZE       0x05
#define   MOVE       0x06
#define   URGENCY_LOCK       0x0a

/*******************_variant_Mainlogic_singleCMD.movetype�ã�Ϊ������������******************/
#define   SINGAL_CANCEI    0x00
#define   SINGAL_PITCH_ON     0x01
#define   SINGAL_MOVE_START       0x02
/*******************_variant_Mainlogic_singleCMD.direction�ã�Ϊ������������******************/

#define   DIRECTION_L_LEFT    0x11
#define   DIRECTION_R_LEFT    0x21
#define   DIRECTION_L_RIGHT   0x12
#define   DIRECTION_R_RIGHT   0x22
#define   DIRECTION_LEFT    0x01
#define   DIRECTION_RIGHT   0x02
#define   DIRECTION_ROMATE   0x03
#define   DIRECTION_COMPUTER   0x04





/*******************_variant_Mainlogic_CMD.singlemoveCMD******************/
#define   MOVE_NONE                                0          //��
#define   MOVE_COLUMN_UP                                 0x01            //������
#define   MOVE_COLUMN_DOWN                               0x02         //������
#define   MOVE_HYDRAULICFRAME_ADVANCING                   0x03          //�Ƽ�
#define   MOVE_CHUTE_PUSH                                 0x04         //����
#define   MOVE_BOTTOM_UP      	                         0x05        //̧��
#define   MOVE_FUEL_SPRAY                                 0x06            //����
#define   MOVE_UPLIFT_BOTTOM_COLUMN_DOWN                 0x07          //̧�׽���
#define   MOVE_UPLIFT_BOTTOM_HYDRAULIC_ADVANCING        0x08            //̧���Ƽ�
#define   MOVE_BALANCE_BEAM_STRETCH                         0x09            //��ƽ����
#define   MOVE_BALANCE_BEAM_WITHDRAW                         0x0a           	//��ƽ����
#define   MOVE_SIDE_PLATE_STRETCH                          0x0b           //�����
#define   MOVE_SIDE_PLATE_WITHDRAW                         0x0c          //�ղ���
#define   MOVE_TELESCOPIC_GIRDER_STRETCH                  0x0d         //��������
#define   MOVE_TELESCOPIC_GIRDER_WITHDRAW                  0x0e            //��������
#define   MOVE_PLATE_STRETCH                            0x0f            //�커���
#define   MOVE_PLATE_WITHDRAW                           0x10             //�ջ����
#define   MOVE_HYDRAULIC_BOTTOM_STRETCH                0x11         //��׵�
#define   MOVE_HYDRAULIC_BOTTOM_WITHDRAW               0x12           //�յ׵�
#define   MOVE_DROP_AD_UP                 0x13             //������
#define   MOVE_STANDBY_APPLICATION1                0x14             //���ö���1
#define   MOVE_STANDBY_APPLICATION2                 0x15             //���ö���2
#define   MOVE_BACK_WASH2                0x16             //����ϴ1
#define   MOVE_BACK_WASH1                0x17             //����ϴ2


#define   MOVE_GROUP_ADVANCING                           0x20          //�Ƽ�
#define   MOVE_GROUP_CHUTE_PUSH                          0x21          //����
#define   MOVE_GROUPTELESCOPICPLATE_GIRDER_WITHDRAW      0x24          //�������������
#define   MOVE_GROUP_CHUTE_PULL                          0x22          //����
#define   MOVE_GROUP_TELESCOPIC_GIRDER_STRETCH           0x23          //��������
#define   MOVE_GROUP_PLATE_STRETCH                       0x25          //�커���
#define   MOVE_GROUP_PLATE_WITHDRAW                       0x26          //�ջ����



#define   MOVE_NONE_OUT                                0          //��
#define   MOVE_COLUMN_UP_OUT                                 (1<<23)            //������
#define   MOVE_COLUMN_DOWN_OUT                              (1<<22)         //������
#define   MOVE_HYDRAULICFRAME_ADVANCING_OUT                  (1<<21)          //�Ƽ�
#define   MOVE_CHUTE_PUSH_OUT                                (1<<20)         //����
#define   MOVE_BOTTOM_UP_OUT      	                        (1<<4)         //̧��
#define   MOVE_FUEL_SPRAY_OUT                                (1<<5)            //����
#define   MOVE_BALANCE_BEAM_STRETCH_OUT                        (1<<19)            //��ƽ����
#define   MOVE_BALANCE_BEAM_WITHDRAW_OUT                         (1<<18)            	//��ƽ����
#define   MOVE_SIDE_PLATE_STRETCH_OUT                         (1<<9)           //�����
#define   MOVE_SIDE_PLATE_WITHDRAW_OUT                        (1<<8)           //�ղ���
#define   MOVE_TELESCOPIC_GIRDER_STRETCH_OUT                 (1<<13)          //��������
#define   MOVE_TELESCOPIC_GIRDER_WITHDRAW_OUT                 (1<<12)            //��������
#define   MOVE_PLATE_STRETCH_OUT                           (1<<17)            //�커���
#define   MOVE_PLATE_WITHDRAW_OUT                          (1<<16)             //�ջ����
#define   MOVE_HYDRAULIC_BOTTOM_STRETCH_OUT               (1<<3)           //��׵�
#define   MOVE_HYDRAULIC_BOTTOM_WITHDRAW_OUT              (1<<2)           //�յ׵�
#define   MOVE_STANDBY_APPLICATION1_OUT                  (1<<6)             //���ö���1
#define   MOVE_STANDBY_APPLICATION2_OUT                   (1<<7)           //���ö���2

///***************************variant_Mainlogic_Setting_subdirectory;//���ý�������Ӳ˵�  �����˽��� ��������2023/02/9*********************************/
//#define   SETING_COMMUNICATION_RAGE                         0x00          //ͨ�ŷ�Χ
//#define   SETING_GLOBAL_SET                                 0x01          //��������
//#define   SETING_SERVICE                                    0x02          //����
//#define   SETING_DEFAULT_PARAMETERS                         0x03          //ȱʡ����
//#define   SETING_GROUP_ADVANCING                                0x04          //�����Ƽ�
//#define   SETING_GROUP_CHUTE_PUSH     	                        0x05          	//��������
//#define   SETING_GROUP_BALANCE                 0x06          //����ƽ��
//#define   SETING_GROUP_TELESCOPIC         0x07           //������������
//#define   SETING_GROUP_NC                         0x08           //����
//#define   SETING_AUTOMATION                        0x09          	//�����Զ���
//#define   SETING_FAULT                          0x0a          //������ʾ


/**********************************************************�ṹ�嶨��*************************************************************************************/




/**********************************************************�ṹ�嶨��*************************************************************************************/
typedef struct _variant_statement{
			
			u8 PRESENTstate_value;		//״ֵ̬
			char PRESENTstate_view[5];

}_variant_statement;



typedef struct _variant_mainlogicCMD1{
	u8 type;		
	u8 direction;		//��������
	u8 movetype;		//������������
	u8 num;		//  ���Ƶ��ڼ��ܵ���
	u8 back_Flag;  //
	u8 moveCMD;		//��������������
}_variant_mainlogicCMD1;



typedef struct _variant_mainlogicCMD2{
	u8 type0;		
	u8 direction1;		//���鷽��
	u8 alarmnum2;		//���鱨����
	u8 movenum3;		        //  ���鶯����Χ
	u8 moveCMD4;		    //��������������
	u8 limit5;		    //��������������
	u8 gap6;		    //�����
	u8 controlnum7;		    //���鶯��������id
}_variant_mainlogicCMD2;



union _unit_mainlogic_Singal{

		uint8_t arrayKEYSingal[6];
		struct _variant_mainlogicCMD1 Mainlogic_singalCMD;

};


union _unit_mainlogic_Group{

		uint8_t arrayKEYGroup[8];
		struct _variant_mainlogicCMD2 Mainlogic_groupCMD;

};


/*******************************************��������************************************************************************************************************/
void Main_logic(void);
void Main_logic_STATEchose(void);
	
void Main_Logic_Init(void);
void Set_Prar_Change(uint8_t Setting_Crosswise,uint8_t Setting_Lengthways);

 extern   _variant_statement _variant_Mainlogic_statement;  




extern uint8_t variant_Mainlogic_KeyValue;
extern u8 State_Mainlogic_PRESENT;
extern uint8_t State_Mainlogic_DWFAULTstate;
extern uint8_t const_Mainlogic_InterfaceFlag;//���Ը���ָ��  �����˽��� ��������2023/01/17
void Group_Pause_Count(void);

extern uint8_t Flag_Mainlogic_Groupcontorl ;//���豸��һ���踺��ĳ������
extern uint8_t variant_Mainlogic_GroupMovenum;//���豸��һ���踺��ĳ������
extern uint8_t Flag_Mainlogic_croupmovecancel ;//��������
void Main_Logic_SingalMove(uint8_t Key_Value);
#endif

   



