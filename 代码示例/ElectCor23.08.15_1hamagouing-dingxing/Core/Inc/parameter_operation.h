//QQQQ�������涯���������ú�������QQQQ
//QQQQ����QJY.2021.5.25����QQQQ

#ifndef __PARAMETER_OPERATION_H__
#define __PARAMETER_OPERATION_H__

#include "stm32f4xx_hal.h"












//�ڲ�FLASH��ʼ��ַ
#define STM32_FLASH_BASE 0x08000000 	//STM32 FLASH����ʼ��ַ
#define FLASH_WAITETIME  50000          //FLASH�ȴ���ʱʱ��

//Stm32F413ZG�ڲ�Flash ��������ʼ��ַ
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) 	//����0��ʼ��ַ, 16 Kbytes  
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) 	//����1��ʼ��ַ, 16 Kbytes  
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) 	//����2��ʼ��ַ, 16 Kbytes  
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) 	//����3��ʼ��ַ, 16 Kbytes  
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) 	//����4��ʼ��ַ, 64 Kbytes  
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) 	//����5��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) 	//����6��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) 	//����7��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000) 	//����8��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x080A0000) 	//����9��ʼ��ַ, 128 Kbytes  
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x080C0000) 	//����10��ʼ��ַ,128 Kbytes  
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x080E0000) 	//����11��ʼ��ַ,128 Kbytes
#define ADDR_FLASH_SECTOR_12		((uint32_t)0x08100000) 	//����12��ʼ��ַ, 16 Kbytes  
#define ADDR_FLASH_SECTOR_13		((uint32_t)0x08104000) 	//����13��ʼ��ַ, 16 Kbytes  
#define ADDR_FLASH_SECTOR_14    ((uint32_t)0x08108000) 	//����14��ʼ��ַ, 16 Kbytes  
#define ADDR_FLASH_SECTOR_15		((uint32_t)0x0810C000) 	//����15��ʼ��ַ, 16 Kbytes 


/***************��������*****************/
#define	GPara_GlobalSet_1  Structset_GlobalSet_MaxDeviceNum1_1
#define	GPara_GlobalSet_2  Structset_GlobalSet_minDeviceNum2_1
#define	GPara_GlobalSet_3  Structset_GlobalSet_Settingtime3_1
#define	GPara_GlobalSet_4  Structset_GlobalSet_Increase4_4
#define	GPara_GlobalSet_5  Structset_GlobalSet_Conswitch5_2
#define	GPara_GlobalSet_6  Structset_GlobalSet_Conposition6_1
#define	GPara_GlobalSet_7  Structset_GlobalSet_PositionUpload7_2
#define	GPara_GlobalSet_8  Structset_GlobalSet_Adjacentcheak8_2
#define	GPara_GlobalSet_9  Structset_GlobalSet_AdjacentWarring9_2
#define	GPara_GlobalSet_10  Structset_GlobalSet_Adjacenttime10_1
#define	GPara_GlobalSet_11  Structset_GlobalSet_BUSworkcheak11_2
#define	GPara_GlobalSet_12  Structset_GlobalSet_BUSreport12_2
#define	GPara_GlobalSet_13  Structset_GlobalSet_BUScheakTime13_1



/***************����*****************/
#define	GPara_Service_1  Structset_Service_DeviceNum1_1
#define	GPara_Service_2  Structset_Service_Adjacentcorrelation2_2
#define	GPara_Service_3  Structset_Service_RemoteCortrol3_2

/***************Ĭ�ϲ���*****************/

#define	GPara_Default_1  Structset_Default_Pressurecheak1_2
#define	GPara_Default_2  Structset_Default_Distancecheak2_2
#define	GPara_Default_3  Structset_Default_Pushtime3_1
#define	GPara_Default_4  Structset16_Default_Pushdistance4_1
#define	GPara_Default_5  Structset_Default_PressureLowlimt5_1
#define	GPara_Default_6  Structset_Default_PressureHighlimt6_1
#define	GPara_Default_7  Structset_Default_aimPressure7_1
#define	GPara_Default_8  Structset_Default_StepuppressureTime8_1
#define	GPara_Default_9  Structset_Default_StepuppressureGap9_1
#define	GPara_Default_10  Structset_Default_Stepuppressuretimes10_1

/***************�Ƽ�*****************/
#define	GPara_Framemove_1   Structset_Framemove_Forbiden1_3
#define	GPara_Framemove_2   Structset_Framemove_Alarm2_2
#define	GPara_Framemove_3   Structset_Framemove_AlarmTime3_1
#define	GPara_Framemove_4   Structset_Framemove_movePressure4_1
#define	GPara_Framemove_5   Structset_Framemove_TransPressure5_1
#define	GPara_Framemove_6   Structset_Framemove_SupportPressure6_1
#define	GPara_Framemove_7   Structset_Framemove_TelescopictMove7_2
#define	GPara_Framemove_8   Structset_Framemove_PlateMove8_2
#define	GPara_Framemove_9   Structset_Framemove_ColumndownDelay9_1
#define	GPara_Framemove_10  Structset_Framemove_ColumndownTime10_1
#define	GPara_Framemove_11  Structset_Framemove_ColumndownBalanceaction11_2
#define	GPara_Framemove_12  Structset_Framemove_Columndownsideplate12_2
#define	GPara_Framemove_13  Structset_Framemove_DcolumnAgainFrequency13_1
#define	GPara_Framemove_14  Structset_Framemove_BmttomUP14_2
#define	GPara_Framemove_15  Structset16_Framemove_Advancaim15_1
#define	GPara_Framemove_16  Structset_Framemove_AdvancTime16_1
#define	GPara_Framemove_17  Structset_Framemove_Advancsideplate17_2
#define	GPara_Framemove_18  Structset_Framemove_AdvancagainTime18_1
#define	GPara_Framemove_19  Structset_Framemove_ColumnUPtime19_1
#define	GPara_Framemove_20  Structset_Framemove_ColumnUPBalanceaction20_2
#define	GPara_Framemove_21  Structset_Framemove_ColumnUPsideplatew21_2
#define	GPara_Framemove_22  Structset_Framemove_Spray22_2
#define	GPara_Framemove_23  Structset_Framemove_SprayTime23_1
#define	GPara_Framemove_24  Structset_Framemove_GroupSeparatedNum24_1
#define	GPara_Framemove_25  Structset_Framemove_GroupRange25_1


/****************����********************/
#define	GPara_ChutePush_1   Structset_ChutePush_Forbiden1_3
#define	GPara_ChutePush_2   Structset_ChutePush_time2_1
#define	GPara_ChutePush_3   Structset16_ChutePush_aimDistance3_1
#define	GPara_ChutePush_4   Structset_ChutePush_Pressure4_1
#define	GPara_ChutePush_5   Structset16_ChutePush_Maximumdifference5_1
#define	GPara_ChutePush_6   Structset_ChutePush_GroupSeparatedNum6_1
#define	GPara_ChutePush_7   Structset_ChutePush_GroupRange7_1

/****************����������********************/


#define	GPara_Telescopict_1   Structset_Telescopict_Forbiden1_3
#define	GPara_Telescopict_2   Structset_Telescopict_StretchTime2_1
#define	GPara_Telescopict_3   Structset_Telescopict_ShrinkTime3_1
#define	GPara_Telescopict_4   Structset_Telescopict_DcolumnTime4_1
#define	GPara_Telescopict_5   Structset_Telescopict_Separatednum5_1
#define	GPara_Telescopict_6   Structset_Telescopict_Range6_1
#define	GPara_Telescopict_7   Structset_PLATE_Forbiden7_3
#define	GPara_Telescopict_8   Structset_PLATE_StretchTime8_1
#define	GPara_Telescopict_9   Structset_PLATE_ShrinkTime9_1


/****************���������********************/
#define	GPara_ChutePull_1 Structset_ChutePull_Forbiden1_3
#define	GPara_ChutePull_2 Structset_ChutePull_time2_1
#define	GPara_ChutePull_3 Structset_ChutePull_aimDistance3_1
#define	GPara_ChutePull_4 Structset_ChutePull_Pressure4_1
#define	GPara_ChutePull_5 Structset_ChutePull_GroupSeparatedNum5_1
#define	GPara_ChutePull_6 Structset_ChutePull_GroupRange6_1
#define	GPara_ChutePull_7 Structset_Mistspray_Forbiden7_3
#define	GPara_ChutePull_8 Structset_Mistspray_SeparatedNum8_1
#define	GPara_ChutePull_9 Structset_Mistspray_Range9_1
#define	GPara_ChutePull_10 Structset_Mistspray_Time10_1





void SaveParaToFlash_WritingFun(void);//�������溯��
void SaveParaToFlash_ReadingFun(void);//������ȡ����
void Structset_GlobalSet_SynchronizationFun_S(void);
void Structset_GlobalSet_SynchronizationFun_R(uint8_t *Recive);
void Serial_Number_delivery_S(void);
//void Serial_Number_delivery_R(uint8_t *Recive);
void Parameter_delivery_R(uint8_t *Recive);
void Parameter_delivery_S(void);	
extern uint16_t variant_Parameter_Globalset;
#endif
