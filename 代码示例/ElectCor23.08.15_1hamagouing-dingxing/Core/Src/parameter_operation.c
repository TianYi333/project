#include "stm32f4xx_hal.h"
#include "bsp_w25q128.h"
#include "parameter_operation.h"
#include "set.h"
#include "string.h"
#include "main.h"
#include "time.h"
#include "main_logic.h"
#include "interface.h"
#include "bsp_oled19264.h"
#include "canreceive.h"
#include "bsp_ncv7356.h"
#include "cansend.h"
//#define  FLASH_SAVE_ADDR  0X080B0000 	 				//设置FLASH 保存地址F413ZG地址	最大到080FFFF0
#define  FLASH_SAVE_ADDR  0x080C0000 	 				//设置FLASH 保存地址F413ZG地址	最大到080FFFF0


volatile  uint8_t FlashWriteBuffer[120];
#define BufLenth sizeof(FlashWriteBuffer)	 		  	//数组长度	
#define SIZE BufLenth/4+((BufLenth%4)?1:0)
volatile uint8_t FlashReadBuffer[BufLenth];

uint16_t variant_Parameter_Globalset =0;
uint8_t count_Parameter_deliveryR = 0;
void Serial_Number_delivery_S(void)
{
	uint8_t  send[8];
	
	send[0] = TPYE_SERIALNUMBER_R; 
	xQueueSend(CAN_Send_Queue,&send,0);
	vTaskDelay(1);
	send[0] = TPYE_SERIALNUMBER_L; 
	xQueueSend(CAN_Send_Queue,&send,0);
	
	
		
	send[0] = TPYE_SERIALNUMBER_L;
	send[1] = Structset_GlobalSet_Increase4_4.Setting_Para;
	send[2] = Structset_Service_DeviceNum1_1.Setting_Para;
	
	
	CANx_SendNormalData(&LEFT_CAN,COMMUN_ID,send,8);				
	vTaskDelay(10);	
	send[0] = TPYE_SERIALNUMBER_R;
	CANx_SendNormalData(&RIGHT_CAN,COMMUN_ID,send,8);
	
}
//接收程序直接写在canLR接收里
//void Serial_Number_delivery_R(uint8_t *Recive)
//{
//	uint8_t send[8];
//	send[0] = TPYE_SERIALNUMBER; 
//	
//	send[1] = Recive[1];
//	if(Recive[1] == Structset_GlobalSet_Increase4_4.Setting_Para)
//	{
//		Structset_Service_DeviceNum1_1.Setting_Para = Recive[2] + 1;
//	}
//	else if(Recive[1] != Structset_GlobalSet_Increase4_4.Setting_Para)
//	{
//		Structset_Service_DeviceNum1_1.Setting_Para = Recive[2] - 1;
//	}
//	xQueueSend(CAN_Send_Queue,&send,0);
//	variant_Service_DeviceNum = Structset_Service_DeviceNum1_1.Setting_Para;
//	DeviceNum[0] = (variant_Service_DeviceNum/100)+48;
//	DeviceNum[1] = ((variant_Service_DeviceNum%100)/10)+48;
//	DeviceNum[2] = (variant_Service_DeviceNum%10)+48;
//	DeviceNum[3] = '\0';
//	SaveParaToFlash_WritingFun();
//}
void Structset_GlobalSet_SynchronizationFun_S(void)
{
	
	uint8_t flag[8] = {0};
	
	flag[0] = TPYE_SET_GLOBALPAR;
	flag[1] = 0xff;
	flag[2] = variant_Parameter_Globalset;

	switch(variant_Parameter_Globalset)
	{
		case 0x01:
			flag[3] = Structset_GlobalSet_MaxDeviceNum1_1.Setting_Para;
		break;
		
		case 0x02:
			flag[3] = Structset_GlobalSet_minDeviceNum2_1.Setting_Para;
		break;
				
		case 0x03:
			flag[3] = Structset_GlobalSet_Settingtime3_1.Setting_Para;
		break;
				
		case 0x04:
			flag[3] = Structset_GlobalSet_Increase4_4.Setting_Para;
		break;
				
		case 0x05:
			flag[3] = Structset_GlobalSet_Conswitch5_2.Setting_Para;
		break;
				
		case 0x06:
			flag[3] = Structset_GlobalSet_Conposition6_1.Setting_Para;
		break;
				
		case 0x07:
			flag[3] = Structset_GlobalSet_PositionUpload7_2.Setting_Para;
		break;
				
		case 0x08:
			flag[3] = Structset_GlobalSet_Adjacentcheak8_2.Setting_Para;
		break;
				
		case 0x09:
			flag[3] = Structset_GlobalSet_AdjacentWarring9_2.Setting_Para;
		break;
				
		case 0x0a:
			flag[3] = Structset_GlobalSet_Adjacenttime10_1.Setting_Para;
		break;
				
		case 0x0b:
			flag[3] = Structset_GlobalSet_BUSworkcheak11_2.Setting_Para;
		break;
				
		case 0x0c:
			flag[3] = Structset_GlobalSet_BUSreport12_2.Setting_Para;
		break;
				
		case 0x0d:
			flag[3] = Structset_GlobalSet_BUScheakTime13_1.Setting_Para;
		break;
				
		default:
			break;
	
	}
	xQueueSend(CAN_Send_Queue,&flag,0);
}
void Structset_GlobalSet_SynchronizationFun_R(uint8_t *Recive)
{
	if(Recive[1] ==  DIRECTION_LEFT)
	{
		CANx_SendNormalData(&LEFT_CAN,COMMUN_ID,Recive,8);				
	}
	else if(Recive[1] ==  DIRECTION_RIGHT)
	{
		CANx_SendNormalData(&RIGHT_CAN,COMMUN_ID,Recive,8);				
	}
	switch(Recive[2])
	{
		case 0x01:
			Structset_GlobalSet_MaxDeviceNum1_1.Setting_Para = Recive[3];
		Set_Para_to_char(Structset_GlobalSet_MaxDeviceNum1_1.Setting_Para,Structset_GlobalSet_MaxDeviceNum1_1.veiw_type,Structset_GlobalSet_MaxDeviceNum1_1.Setting_Char);
		break;
		
		case 0x02:
			Structset_GlobalSet_minDeviceNum2_1.Setting_Para = Recive[3];
		Set_Para_to_char(Structset_GlobalSet_minDeviceNum2_1.Setting_Para,Structset_GlobalSet_minDeviceNum2_1.veiw_type,Structset_GlobalSet_minDeviceNum2_1.Setting_Char);
		break;
				
		case 0x03:
			Structset_GlobalSet_Settingtime3_1.Setting_Para = Recive[3];
		Set_Para_to_char(Structset_GlobalSet_Settingtime3_1.Setting_Para,Structset_GlobalSet_Settingtime3_1.veiw_type,Structset_GlobalSet_Settingtime3_1.Setting_Char);
		break;
				
		case 0x04:
			Structset_GlobalSet_Increase4_4.Setting_Para = Recive[3];
		Set_Para_to_char(Structset_GlobalSet_Increase4_4.Setting_Para,Structset_GlobalSet_Increase4_4.veiw_type,Structset_GlobalSet_Increase4_4.Setting_Char);
		break;
				
		case 0x05:
			Structset_GlobalSet_Conswitch5_2.Setting_Para = Recive[3];
		Set_Para_to_char(Structset_GlobalSet_Conswitch5_2.Setting_Para,Structset_GlobalSet_Conswitch5_2.veiw_type,Structset_GlobalSet_Conswitch5_2.Setting_Char);
		break;
				
		case 0x06:
			Structset_GlobalSet_Conposition6_1.Setting_Para = Recive[3];
		Set_Para_to_char(Structset_GlobalSet_Conposition6_1.Setting_Para,Structset_GlobalSet_Conposition6_1.veiw_type,Structset_GlobalSet_Conposition6_1.Setting_Char);
		break;
				
		case 0x07:
			Structset_GlobalSet_PositionUpload7_2.Setting_Para = Recive[3];
		Set_Para_to_char(Structset_GlobalSet_PositionUpload7_2.Setting_Para,Structset_GlobalSet_PositionUpload7_2.veiw_type,Structset_GlobalSet_PositionUpload7_2.Setting_Char);
		break;
				
		case 0x08:
			Structset_GlobalSet_Adjacentcheak8_2.Setting_Para = Recive[3];
		Set_Para_to_char(Structset_GlobalSet_Adjacentcheak8_2.Setting_Para,Structset_GlobalSet_Adjacentcheak8_2.veiw_type,Structset_GlobalSet_Adjacentcheak8_2.Setting_Char);
		break;
				
		case 0x09:
			Structset_GlobalSet_AdjacentWarring9_2.Setting_Para = Recive[3];
		Set_Para_to_char(Structset_GlobalSet_AdjacentWarring9_2.Setting_Para,Structset_GlobalSet_AdjacentWarring9_2.veiw_type,Structset_GlobalSet_AdjacentWarring9_2.Setting_Char);
		break;
				
		case 0x0a:
			Structset_GlobalSet_Adjacenttime10_1.Setting_Para = Recive[3];
		Set_Para_to_char(Structset_GlobalSet_Adjacenttime10_1.Setting_Para,Structset_GlobalSet_Adjacenttime10_1.veiw_type,Structset_GlobalSet_Adjacenttime10_1.Setting_Char);
		break;
				
		case 0x0b:
			Structset_GlobalSet_BUSworkcheak11_2.Setting_Para = Recive[3];
		Set_Para_to_char(Structset_GlobalSet_BUSworkcheak11_2.Setting_Para,Structset_GlobalSet_BUSworkcheak11_2.veiw_type,Structset_GlobalSet_BUSworkcheak11_2.Setting_Char);
		break;
				
		case 0x0c:
			Structset_GlobalSet_BUSreport12_2.Setting_Para = Recive[3];
		Set_Para_to_char(Structset_GlobalSet_BUSreport12_2.Setting_Para,Structset_GlobalSet_BUSreport12_2.veiw_type,Structset_GlobalSet_BUSreport12_2.Setting_Char);
		break;
				
		case 0x0d:
			Structset_GlobalSet_BUScheakTime13_1.Setting_Para = Recive[3];
		Set_Para_to_char(Structset_GlobalSet_BUScheakTime13_1.Setting_Para,Structset_GlobalSet_BUScheakTime13_1.veiw_type,Structset_GlobalSet_BUScheakTime13_1.Setting_Char);
		break;
				
		default:
			break;
	
	}
		SaveParaToFlash_WritingFun();
		xQueueSend(CAN_Send_Queue,&Recive,0);				


}

//读取指定地址的字(32位数据)
//faddr:读地址
//返回值:对应数据.
uint32_t STMFLASH_ReadWord(uint32_t faddr)
{
    return *(uint32_t*)faddr;
}

//获取某个地址所在的flash扇区
//addr:flash地址
//返回值:0~11,即addr所在的扇区
uint8_t STMFLASH_GetFlashSector(uint32_t addr)
{
	if(addr<ADDR_FLASH_SECTOR_1)return FLASH_SECTOR_0;
	else if(addr<ADDR_FLASH_SECTOR_2)return FLASH_SECTOR_1;
	else if(addr<ADDR_FLASH_SECTOR_3)return FLASH_SECTOR_2;
	else if(addr<ADDR_FLASH_SECTOR_4)return FLASH_SECTOR_3;
	else if(addr<ADDR_FLASH_SECTOR_5)return FLASH_SECTOR_4;
	else if(addr<ADDR_FLASH_SECTOR_6)return FLASH_SECTOR_5;
	else if(addr<ADDR_FLASH_SECTOR_7)return FLASH_SECTOR_6;
	else if(addr<ADDR_FLASH_SECTOR_8)return FLASH_SECTOR_7;
	else if(addr<ADDR_FLASH_SECTOR_9)return FLASH_SECTOR_8;
	else if(addr<ADDR_FLASH_SECTOR_10)return FLASH_SECTOR_9;
	else if(addr<ADDR_FLASH_SECTOR_11)return FLASH_SECTOR_10; 
	else if(addr<ADDR_FLASH_SECTOR_12)return FLASH_SECTOR_11; 
	else if(addr<ADDR_FLASH_SECTOR_13)return FLASH_SECTOR_12; 
	else if(addr<ADDR_FLASH_SECTOR_14)return FLASH_SECTOR_13; 
	else if(addr<ADDR_FLASH_SECTOR_15)return FLASH_SECTOR_14; 
	return FLASH_SECTOR_15;	
}

//从指定地址开始写入指定长度的数据
//特别注意:因为STM32F4的扇区实在太大,没办法本地保存扇区数据,所以本函数
//         写地址如果非0XFF,那么会先擦除整个扇区且不保存扇区数据.所以
//         写非0XFF的地址,将导致整个扇区数据丢失.建议写之前确保扇区里
//         没有重要数据,最好是整个扇区先擦除了,然后慢慢往后写. 
//该函数对OTP区域也有效!可以用来写OTP区!
//OTP区域地址范围:0X1FFF7800~0X1FFF7A0F(注意：最后16字节，用于OTP数据块锁定，别乱写！！)
//WriteAddr:起始地址(此地址必须为4的倍数!!)
//pBuffer:数据指针
//NumToWrite:字(32位)数(就是要写入的32位数据的个数.) 
void STMFLASH_Write(uint32_t WriteAddr,uint32_t *pBuffer,uint32_t NumToWrite)	
{ 
	FLASH_EraseInitTypeDef FlashEraseInit;
	HAL_StatusTypeDef FlashStatus=HAL_OK;
	uint32_t SectorError=0;
	uint32_t addrx=0;
	uint32_t endaddr=0;	
	if(WriteAddr<STM32_FLASH_BASE||WriteAddr%4)return;	//非法地址
    
	HAL_FLASH_Unlock();             //解锁	
	addrx=WriteAddr;				//写入的起始地址
	endaddr=WriteAddr+NumToWrite*4;	//写入的结束地址
    
	if(addrx<0X1FFF0000)
	{
		while(addrx<endaddr)		//扫清一切障碍.(对非FFFFFFFF的地方,先擦除)
		{
			 if(STMFLASH_ReadWord(addrx)!=0XFFFFFFFF)//有非0XFFFFFFFF的地方,要擦除这个扇区
			{   
				FlashEraseInit.TypeErase=FLASH_TYPEERASE_SECTORS;       //擦除类型，扇区擦除 
				FlashEraseInit.Sector=STMFLASH_GetFlashSector(addrx);   //要擦除的扇区
				FlashEraseInit.NbSectors=1;                             //一次只擦除一个扇区
				FlashEraseInit.VoltageRange=FLASH_VOLTAGE_RANGE_3;      //电压范围，VCC=2.7~3.6V之间!!
				if(HAL_FLASHEx_Erase(&FlashEraseInit,&SectorError)!=HAL_OK) 
				{
					break;//发生错误了	
				}
				}else addrx+=4;
				FLASH_WaitForLastOperation(FLASH_WAITETIME);                //等待上次操作完成
		}
	}
	FlashStatus=FLASH_WaitForLastOperation(FLASH_WAITETIME);            //等待上次操作完成
	if(FlashStatus==HAL_OK)
	{
		 while(WriteAddr<endaddr)//写数据
		 {
			if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,WriteAddr,*pBuffer)!=HAL_OK)//写入数据
			{ 
				break;	//写入异常
			}
			WriteAddr+=4;
			pBuffer++;
		}  
	}
	HAL_FLASH_Lock();           //上锁
} 

//从指定地址开始读出指定长度的数据
//ReadAddr:起始地址
//pBuffer:数据指针
//NumToRead:字(32位)数
void STMFLASH_Read(uint32_t ReadAddr, uint32_t *pBuffer, uint32_t NumToRead)
{
    uint32_t i;

    for(i = 0; i < NumToRead; i++)
    {
        pBuffer[i] = STMFLASH_ReadWord(ReadAddr); //读取4个字节.
        ReadAddr += 4; //偏移4个字节.
    }
}


void SaveParaToFlash_WritingFun(void)
{
	volatile  uint8_t i = 0;
uint8_t intermediate_variable_H,intermediate_variable_L;
	


/***************整体设置*****************/
 FlashWriteBuffer[i++] = GPara_GlobalSet_1 .Setting_Para;
 FlashWriteBuffer[i++] = GPara_GlobalSet_2 .Setting_Para;
 FlashWriteBuffer[i++] = GPara_GlobalSet_3 .Setting_Para;
 FlashWriteBuffer[i++] = GPara_GlobalSet_4 .Setting_Para;
 FlashWriteBuffer[i++] = GPara_GlobalSet_5 .Setting_Para;
 FlashWriteBuffer[i++] = GPara_GlobalSet_6 .Setting_Para;
 FlashWriteBuffer[i++] = GPara_GlobalSet_7 .Setting_Para;
 FlashWriteBuffer[i++] = GPara_GlobalSet_8 .Setting_Para;
 FlashWriteBuffer[i++] = GPara_GlobalSet_9 .Setting_Para;
 FlashWriteBuffer[i++] = GPara_GlobalSet_10.Setting_Para;
 FlashWriteBuffer[i++] = GPara_GlobalSet_11 .Setting_Para;
 FlashWriteBuffer[i++] = GPara_GlobalSet_12 .Setting_Para;
 FlashWriteBuffer[i++] = GPara_GlobalSet_13 .Setting_Para;
/***************服务*****************/
 FlashWriteBuffer[i++] = GPara_Service_1 .Setting_Para;
 FlashWriteBuffer[i++] = GPara_Service_2 .Setting_Para;
 FlashWriteBuffer[i++] = GPara_Service_3 .Setting_Para;
 
 /***************默认参数*****************/
 FlashWriteBuffer[i++] = GPara_Default_1.Setting_Para;
 FlashWriteBuffer[i++] = GPara_Default_2.Setting_Para;
 FlashWriteBuffer[i++] = GPara_Default_3.Setting_Para; 
 intermediate_variable_H = ((GPara_Default_4.Setting_Para>>8)&0xff);
 intermediate_variable_L= (GPara_Default_4.Setting_Para&0xff);
 FlashWriteBuffer[i++] = intermediate_variable_H;
 FlashWriteBuffer[i++] = intermediate_variable_L;
 
 FlashWriteBuffer[i++] = GPara_Default_5.Setting_Para;
 FlashWriteBuffer[i++] = GPara_Default_6.Setting_Para;
 FlashWriteBuffer[i++] = GPara_Default_7.Setting_Para;
 FlashWriteBuffer[i++] = GPara_Default_8.Setting_Para;
 FlashWriteBuffer[i++] = GPara_Default_9.Setting_Para;
 FlashWriteBuffer[i++] = GPara_Default_10.Setting_Para;


/***************移架*****************/

 FlashWriteBuffer[i++] = GPara_Framemove_1 .Setting_Para;
 FlashWriteBuffer[i++] = GPara_Framemove_2 .Setting_Para;
 FlashWriteBuffer[i++] = GPara_Framemove_3 .Setting_Para;
 FlashWriteBuffer[i++] = GPara_Framemove_4 .Setting_Para;
 FlashWriteBuffer[i++] = GPara_Framemove_5 .Setting_Para;
 FlashWriteBuffer[i++] = GPara_Framemove_6 .Setting_Para;
 FlashWriteBuffer[i++] = GPara_Framemove_7 .Setting_Para;
 FlashWriteBuffer[i++] = GPara_Framemove_8 .Setting_Para;
 FlashWriteBuffer[i++] = GPara_Framemove_9 .Setting_Para;
 FlashWriteBuffer[i++] = GPara_Framemove_10 .Setting_Para;
 FlashWriteBuffer[i++] = GPara_Framemove_11 .Setting_Para;
 FlashWriteBuffer[i++] = GPara_Framemove_12 .Setting_Para;
 FlashWriteBuffer[i++] = GPara_Framemove_13 .Setting_Para;
 FlashWriteBuffer[i++] = GPara_Framemove_14 .Setting_Para;
 
 intermediate_variable_H = ((GPara_Framemove_15.Setting_Para>>8)&0xff);
 intermediate_variable_L= (GPara_Framemove_15.Setting_Para&0xff);
 FlashWriteBuffer[i++] = intermediate_variable_H;
 FlashWriteBuffer[i++] = intermediate_variable_L;
 
 FlashWriteBuffer[i++] = GPara_Framemove_16 .Setting_Para;
 FlashWriteBuffer[i++] = GPara_Framemove_17 .Setting_Para;
 FlashWriteBuffer[i++] = GPara_Framemove_18 .Setting_Para;
 FlashWriteBuffer[i++] = GPara_Framemove_19 .Setting_Para;
 FlashWriteBuffer[i++] = GPara_Framemove_20.Setting_Para;
 FlashWriteBuffer[i++] = GPara_Framemove_21 .Setting_Para;
 FlashWriteBuffer[i++] = GPara_Framemove_22 .Setting_Para;
 FlashWriteBuffer[i++] = GPara_Framemove_23 .Setting_Para;
 FlashWriteBuffer[i++] = GPara_Framemove_24 .Setting_Para;
 FlashWriteBuffer[i++] = GPara_Framemove_25.Setting_Para;
 

/****************推溜********************/

 FlashWriteBuffer[i++] = GPara_ChutePush_1.Setting_Para;
 FlashWriteBuffer[i++] = GPara_ChutePush_2.Setting_Para;
 
 intermediate_variable_H = ((GPara_ChutePush_3.Setting_Para>>8)&0xff);
 intermediate_variable_L= (GPara_ChutePush_3.Setting_Para&0xff);
 FlashWriteBuffer[i++] = intermediate_variable_H;
 FlashWriteBuffer[i++] = intermediate_variable_L;
 
 FlashWriteBuffer[i++] = GPara_ChutePush_4.Setting_Para;
 
  intermediate_variable_H = ((GPara_ChutePush_5.Setting_Para>>8)&0xff);
 intermediate_variable_L= (GPara_ChutePush_5.Setting_Para&0xff);
 FlashWriteBuffer[i++] = intermediate_variable_H;
 FlashWriteBuffer[i++] = intermediate_variable_L;

 FlashWriteBuffer[i++] = GPara_ChutePush_6.Setting_Para;
 FlashWriteBuffer[i++] = GPara_ChutePush_7.Setting_Para;
 
/****************伸缩梁护帮********************/
FlashWriteBuffer[i++] = GPara_Telescopict_1.Setting_Para;
FlashWriteBuffer[i++] = GPara_Telescopict_2.Setting_Para;
FlashWriteBuffer[i++] = GPara_Telescopict_3.Setting_Para;
FlashWriteBuffer[i++] = GPara_Telescopict_4.Setting_Para;
FlashWriteBuffer[i++] = GPara_Telescopict_5.Setting_Para;
FlashWriteBuffer[i++] = GPara_Telescopict_6.Setting_Para;
FlashWriteBuffer[i++] = GPara_Telescopict_7.Setting_Para;
FlashWriteBuffer[i++] = GPara_Telescopict_8.Setting_Para;
FlashWriteBuffer[i++] = GPara_Telescopict_9.Setting_Para;

/****************拉溜和喷雾********************/
  FlashWriteBuffer[i++] = GPara_ChutePull_1.Setting_Para;
  FlashWriteBuffer[i++] = GPara_ChutePull_2.Setting_Para;
  FlashWriteBuffer[i++] = GPara_ChutePull_3.Setting_Para;
  FlashWriteBuffer[i++] = GPara_ChutePull_4.Setting_Para;
  FlashWriteBuffer[i++] = GPara_ChutePull_5.Setting_Para;
  FlashWriteBuffer[i++] = GPara_ChutePull_6.Setting_Para;
  FlashWriteBuffer[i++] = GPara_ChutePull_7.Setting_Para;
  FlashWriteBuffer[i++] = GPara_ChutePull_8.Setting_Para;
  FlashWriteBuffer[i++] = GPara_ChutePull_9.Setting_Para;
  FlashWriteBuffer[i++] = GPara_ChutePull_10.Setting_Para;

  
  STMFLASH_Write(FLASH_SAVE_ADDR, (uint32_t*)FlashWriteBuffer, SIZE);//保证稳定连写两次
  STMFLASH_Write(FLASH_SAVE_ADDR, (uint32_t*)FlashWriteBuffer, SIZE);			

}

void SaveParaToFlash_ReadingFun(void)
{
	volatile	uint8_t i = 0;

	STMFLASH_Read(FLASH_SAVE_ADDR, (uint32_t*)FlashReadBuffer, SIZE);
/***************整体设置*****************/
	GPara_GlobalSet_1 .Setting_Para=FlashReadBuffer[i++];
	GPara_GlobalSet_2 .Setting_Para=FlashReadBuffer[i++];
	GPara_GlobalSet_3 .Setting_Para=FlashReadBuffer[i++];
	GPara_GlobalSet_4 .Setting_Para=FlashReadBuffer[i++];
	GPara_GlobalSet_5 .Setting_Para=FlashReadBuffer[i++];
	GPara_GlobalSet_6 .Setting_Para=FlashReadBuffer[i++];
	GPara_GlobalSet_7 .Setting_Para=FlashReadBuffer[i++];
	GPara_GlobalSet_8 .Setting_Para=FlashReadBuffer[i++];
	GPara_GlobalSet_9 .Setting_Para=FlashReadBuffer[i++];
	GPara_GlobalSet_10 .Setting_Para=FlashReadBuffer[i++];
	GPara_GlobalSet_11 .Setting_Para=FlashReadBuffer[i++];
	GPara_GlobalSet_12 .Setting_Para=FlashReadBuffer[i++];
	GPara_GlobalSet_13 .Setting_Para=FlashReadBuffer[i++];

	GPara_Service_1.Setting_Para=FlashReadBuffer[i++];
	GPara_Service_2.Setting_Para=FlashReadBuffer[i++];
	GPara_Service_3.Setting_Para=FlashReadBuffer[i++];
	
	GPara_Default_1.Setting_Para = FlashReadBuffer[i++];
	GPara_Default_2.Setting_Para = FlashReadBuffer[i++];
	GPara_Default_3.Setting_Para = FlashReadBuffer[i++];
	
 	GPara_Default_4.Setting_Para = FlashReadBuffer[i++];
	GPara_Default_4.Setting_Para = (GPara_Default_4.Setting_Para<<8);
	GPara_Default_4.Setting_Para = GPara_Default_4.Setting_Para + FlashReadBuffer[i++];
		
	GPara_Default_5.Setting_Para = FlashReadBuffer[i++];
	GPara_Default_6.Setting_Para = FlashReadBuffer[i++];
	GPara_Default_7.Setting_Para = FlashReadBuffer[i++];
	GPara_Default_8.Setting_Para = FlashReadBuffer[i++];
	GPara_Default_9.Setting_Para = FlashReadBuffer[i++];
	GPara_Default_10.Setting_Para = FlashReadBuffer[i++];
	
	GPara_Framemove_1.Setting_Para= FlashReadBuffer[i++];
	GPara_Framemove_2.Setting_Para= FlashReadBuffer[i++];
	GPara_Framemove_3.Setting_Para= FlashReadBuffer[i++];
	GPara_Framemove_4.Setting_Para= FlashReadBuffer[i++];
	GPara_Framemove_5.Setting_Para= FlashReadBuffer[i++];
	GPara_Framemove_6.Setting_Para= FlashReadBuffer[i++];
	GPara_Framemove_7.Setting_Para= FlashReadBuffer[i++];
	GPara_Framemove_8.Setting_Para= FlashReadBuffer[i++];
	GPara_Framemove_9.Setting_Para= FlashReadBuffer[i++];
	GPara_Framemove_10.Setting_Para= FlashReadBuffer[i++];
	GPara_Framemove_11.Setting_Para= FlashReadBuffer[i++];
	GPara_Framemove_12.Setting_Para= FlashReadBuffer[i++];
	GPara_Framemove_13.Setting_Para= FlashReadBuffer[i++];
	GPara_Framemove_14.Setting_Para= FlashReadBuffer[i++];

 	GPara_Framemove_15.Setting_Para = FlashReadBuffer[i++];
	GPara_Framemove_15.Setting_Para = (GPara_Framemove_15.Setting_Para<<8);
	GPara_Framemove_15.Setting_Para = GPara_Framemove_15.Setting_Para + FlashReadBuffer[i++];
	
	GPara_Framemove_16.Setting_Para= FlashReadBuffer[i++];
	GPara_Framemove_17.Setting_Para= FlashReadBuffer[i++];
	GPara_Framemove_18.Setting_Para= FlashReadBuffer[i++];
	GPara_Framemove_19.Setting_Para= FlashReadBuffer[i++];
	GPara_Framemove_20.Setting_Para= FlashReadBuffer[i++];
	GPara_Framemove_21.Setting_Para= FlashReadBuffer[i++];
	GPara_Framemove_22.Setting_Para= FlashReadBuffer[i++];
	GPara_Framemove_23.Setting_Para= FlashReadBuffer[i++];
	GPara_Framemove_24.Setting_Para= FlashReadBuffer[i++];
	GPara_Framemove_25.Setting_Para = FlashReadBuffer[i++];

	GPara_ChutePush_1.Setting_Para = FlashReadBuffer[i++];
	GPara_ChutePush_2.Setting_Para = FlashReadBuffer[i++];
	
	GPara_ChutePush_3.Setting_Para = FlashReadBuffer[i++];
	GPara_ChutePush_3.Setting_Para = (GPara_ChutePush_3.Setting_Para<<8);
	GPara_ChutePush_3.Setting_Para = GPara_ChutePush_3.Setting_Para + FlashReadBuffer[i++];
	
	GPara_ChutePush_4.Setting_Para = FlashReadBuffer[i++];
	
	GPara_ChutePush_5.Setting_Para = FlashReadBuffer[i++];
	GPara_ChutePush_5.Setting_Para = (GPara_ChutePush_5.Setting_Para<<8);
	GPara_ChutePush_5.Setting_Para = GPara_ChutePush_5.Setting_Para + FlashReadBuffer[i++];
	
	GPara_ChutePush_6.Setting_Para = FlashReadBuffer[i++];
	GPara_ChutePush_7.Setting_Para = FlashReadBuffer[i++];
	
	GPara_Telescopict_1.Setting_Para = FlashReadBuffer[i++];
	GPara_Telescopict_2.Setting_Para = FlashReadBuffer[i++];
	GPara_Telescopict_3.Setting_Para = FlashReadBuffer[i++];
	GPara_Telescopict_4.Setting_Para = FlashReadBuffer[i++];
	GPara_Telescopict_5.Setting_Para = FlashReadBuffer[i++];
	GPara_Telescopict_6.Setting_Para = FlashReadBuffer[i++];
	GPara_Telescopict_7.Setting_Para = FlashReadBuffer[i++];
	GPara_Telescopict_8.Setting_Para = FlashReadBuffer[i++];
	GPara_Telescopict_9.Setting_Para = FlashReadBuffer[i++];
	
	/****************拉溜和喷雾********************/
      GPara_ChutePull_1.Setting_Para = FlashReadBuffer[i++];
      GPara_ChutePull_2.Setting_Para = FlashReadBuffer[i++];
	  GPara_ChutePull_3.Setting_Para = FlashReadBuffer[i++];
	  GPara_ChutePull_4.Setting_Para = FlashReadBuffer[i++];
	  GPara_ChutePull_5.Setting_Para = FlashReadBuffer[i++];
	  GPara_ChutePull_6.Setting_Para = FlashReadBuffer[i++];
	  GPara_ChutePull_7.Setting_Para = FlashReadBuffer[i++];
	  GPara_ChutePull_8.Setting_Para = FlashReadBuffer[i++];
	  GPara_ChutePull_9.Setting_Para = FlashReadBuffer[i++];
	  GPara_ChutePull_10.Setting_Para = FlashReadBuffer[i++];
}

void Parameter_delivery_S(void)//参数传递发送
{
	uint8_t send[8];
	 uint8_t i_count,j_count=0;
	volatile uint8_t i = 0;
	send[0] = TPYE_PARAMETER;
	uint8_t sendBuffer[80];
	uint8_t intermediate_variable_H,intermediate_variable_L;
	
 sendBuffer[i++] = GPara_Service_2 .Setting_Para;
 
 /***************默认参数*****************/
 sendBuffer[i++] = GPara_Default_1.Setting_Para;
 sendBuffer[i++] = GPara_Default_2.Setting_Para;
 sendBuffer[i++] = GPara_Default_3.Setting_Para; 
 intermediate_variable_H = ((GPara_Default_4.Setting_Para>>8)&0xff);
 intermediate_variable_L= (GPara_Default_4.Setting_Para&0xff);
 sendBuffer[i++] = intermediate_variable_H;
 sendBuffer[i++] = intermediate_variable_L;
 
 sendBuffer[i++] = GPara_Default_5.Setting_Para;
 sendBuffer[i++] = GPara_Default_6.Setting_Para;
 sendBuffer[i++] = GPara_Default_7.Setting_Para;
 sendBuffer[i++] = GPara_Default_8.Setting_Para;
 sendBuffer[i++] = GPara_Default_9.Setting_Para;
 sendBuffer[i++] = GPara_Default_10.Setting_Para;


/***************移架*****************/

// sendBuffer[i++] = GPara_Framemove_1 .Setting_Para;//不传
 sendBuffer[i++] = GPara_Framemove_2 .Setting_Para;
 sendBuffer[i++] = GPara_Framemove_3 .Setting_Para;
 sendBuffer[i++] = GPara_Framemove_4 .Setting_Para;
 sendBuffer[i++] = GPara_Framemove_5 .Setting_Para;
 sendBuffer[i++] = GPara_Framemove_6 .Setting_Para;
 sendBuffer[i++] = GPara_Framemove_7 .Setting_Para;
 sendBuffer[i++] = GPara_Framemove_8 .Setting_Para;
 sendBuffer[i++] = GPara_Framemove_9 .Setting_Para;
 sendBuffer[i++] = GPara_Framemove_10 .Setting_Para;
 sendBuffer[i++] = GPara_Framemove_11 .Setting_Para;
 sendBuffer[i++] = GPara_Framemove_12 .Setting_Para;
 sendBuffer[i++] = GPara_Framemove_13 .Setting_Para;
 sendBuffer[i++] = GPara_Framemove_14 .Setting_Para;
 
 intermediate_variable_H = ((GPara_Framemove_15.Setting_Para>>8)&0xff);
 intermediate_variable_L= (GPara_Framemove_15.Setting_Para&0xff);
 sendBuffer[i++] = intermediate_variable_H;
 sendBuffer[i++] = intermediate_variable_L;
 
 sendBuffer[i++] = GPara_Framemove_16 .Setting_Para;
 sendBuffer[i++] = GPara_Framemove_17 .Setting_Para;
 sendBuffer[i++] = GPara_Framemove_18 .Setting_Para;
 sendBuffer[i++] = GPara_Framemove_19 .Setting_Para;
 sendBuffer[i++] = GPara_Framemove_20.Setting_Para;
 sendBuffer[i++] = GPara_Framemove_21 .Setting_Para;
 sendBuffer[i++] = GPara_Framemove_22 .Setting_Para;
 sendBuffer[i++] = GPara_Framemove_23 .Setting_Para;
 sendBuffer[i++] = GPara_Framemove_24 .Setting_Para;
 sendBuffer[i++] = GPara_Framemove_25.Setting_Para;
 

/****************推溜********************/

// sendBuffer[i++] = GPara_ChutePush_1.Setting_Para;//不传
 sendBuffer[i++] = GPara_ChutePush_2.Setting_Para;
 
 intermediate_variable_H = ((GPara_ChutePush_3.Setting_Para>>8)&0xff);
 intermediate_variable_L= (GPara_ChutePush_3.Setting_Para&0xff);
 sendBuffer[i++] = intermediate_variable_H;
 sendBuffer[i++] = intermediate_variable_L;
 
 sendBuffer[i++] = GPara_ChutePush_4.Setting_Para;
 
  intermediate_variable_H = ((GPara_ChutePush_5.Setting_Para>>8)&0xff);
 intermediate_variable_L= (GPara_ChutePush_5.Setting_Para&0xff);
 sendBuffer[i++] = intermediate_variable_H;
 sendBuffer[i++] = intermediate_variable_L;

 sendBuffer[i++] = GPara_ChutePush_6.Setting_Para;
 sendBuffer[i++] = GPara_ChutePush_7.Setting_Para;
 
/****************伸缩梁护帮********************/
//sendBuffer[i++] = GPara_Telescopict_1.Setting_Para;//不传
sendBuffer[i++] = GPara_Telescopict_2.Setting_Para;
sendBuffer[i++] = GPara_Telescopict_3.Setting_Para;
sendBuffer[i++] = GPara_Telescopict_4.Setting_Para;
sendBuffer[i++] = GPara_Telescopict_5.Setting_Para;
sendBuffer[i++] = GPara_Telescopict_6.Setting_Para;
//sendBuffer[i++] = GPara_Telescopict_7.Setting_Para;//不传
sendBuffer[i++] = GPara_Telescopict_8.Setting_Para;
sendBuffer[i++] = GPara_Telescopict_9.Setting_Para;

/****************拉溜和喷雾********************/
//  sendBuffer[i++] = GPara_ChutePull_1.Setting_Para;//不传
  sendBuffer[i++] = GPara_ChutePull_2.Setting_Para;
  sendBuffer[i++] = GPara_ChutePull_3.Setting_Para;
  sendBuffer[i++] = GPara_ChutePull_4.Setting_Para;
  sendBuffer[i++] = GPara_ChutePull_5.Setting_Para;
  sendBuffer[i++] = GPara_ChutePull_6.Setting_Para;
//  sendBuffer[i++] = GPara_ChutePull_7.Setting_Para;//不传
  sendBuffer[i++] = GPara_ChutePull_8.Setting_Para;
  sendBuffer[i++] = GPara_ChutePull_9.Setting_Para;
  sendBuffer[i++] = GPara_ChutePull_10.Setting_Para;
	

	
	if(sendBuffer[0] == GPara_Service_2.Setting_Para)
	{
		Clear_Screen();
		Display_String(3,32,"参数传递中");
		for(j_count=0;j_count<10;j_count++)
		{
			send[1] = j_count;
			for(i_count=0;i_count<6;i_count++)
			{		
				send[i_count+2] = sendBuffer[i_count+(j_count*6)];	
			}
			xQueueSend(CAN_Send_Queue,&send,0);
			vTaskDelay(200);
			
		}

	}
}
void Parameter_delivery_R(uint8_t *Recive)//参数传递接收
{
	
	switch(Recive[1])
	{
		case 0:
			GPara_Service_2 .Setting_Para = Recive[2];
			GPara_Default_1.Setting_Para = Recive[3];
			GPara_Default_2.Setting_Para = Recive[4];
			GPara_Default_3.Setting_Para = Recive[5]; 
			GPara_Default_4.Setting_Para = Recive[6];
			GPara_Default_4.Setting_Para = (GPara_Default_4.Setting_Para<<8);
			GPara_Default_4.Setting_Para = GPara_Default_4.Setting_Para + Recive[7];
			count_Parameter_deliveryR ++;
		break;
		
		case 1:
			GPara_Default_5.Setting_Para = Recive[2];
			GPara_Default_6.Setting_Para = Recive[3];
			GPara_Default_7.Setting_Para = Recive[4];
			GPara_Default_8.Setting_Para = Recive[5];
			GPara_Default_9.Setting_Para = Recive[6];
			GPara_Default_10.Setting_Para = Recive[7];
			count_Parameter_deliveryR++;
		break;
		
		case 2:
			GPara_Framemove_2.Setting_Para = Recive[2];
		    GPara_Framemove_3.Setting_Para = Recive[3];
			GPara_Framemove_4.Setting_Para = Recive[4];
			GPara_Framemove_5.Setting_Para = Recive[5];
			GPara_Framemove_6.Setting_Para = Recive[6];
			GPara_Framemove_7.Setting_Para = Recive[7];
		    count_Parameter_deliveryR++;
		break;
		
		case 3:
			GPara_Framemove_8.Setting_Para = Recive[2];
		    GPara_Framemove_9.Setting_Para = Recive[3];
			GPara_Framemove_10.Setting_Para = Recive[4];
			GPara_Framemove_11.Setting_Para = Recive[5];
			GPara_Framemove_12.Setting_Para = Recive[6];
			GPara_Framemove_13.Setting_Para = Recive[7];
				    count_Parameter_deliveryR++;
		break;
		
		case 4:
			GPara_Framemove_14.Setting_Para = Recive[2];
		    GPara_Framemove_15.Setting_Para = Recive[3];
			GPara_Framemove_15.Setting_Para = (GPara_Framemove_15.Setting_Para<<8);
			GPara_Framemove_15.Setting_Para = GPara_Framemove_15.Setting_Para+Recive[4];
			GPara_Framemove_16.Setting_Para = Recive[5];
			GPara_Framemove_17.Setting_Para = Recive[6];
			GPara_Framemove_18.Setting_Para = Recive[7];
				    count_Parameter_deliveryR++;
		break;
		
		case 5:
			GPara_Framemove_19.Setting_Para = Recive[2];
		    GPara_Framemove_20.Setting_Para = Recive[3];
			GPara_Framemove_21.Setting_Para = Recive[4];
			GPara_Framemove_22.Setting_Para = Recive[5];
			GPara_Framemove_23.Setting_Para = Recive[6];
			GPara_Framemove_24.Setting_Para = Recive[7];
		count_Parameter_deliveryR++;
		break;
		
		case 6:
			GPara_Framemove_25.Setting_Para = Recive[2];
		    GPara_ChutePush_2.Setting_Para = Recive[3];
		
			GPara_ChutePush_3.Setting_Para = Recive[4];
			GPara_ChutePush_3.Setting_Para = (GPara_ChutePush_3.Setting_Para<<8);
			GPara_ChutePush_3.Setting_Para = GPara_ChutePush_3.Setting_Para+Recive[5];
		
			GPara_ChutePush_4.Setting_Para = Recive[6];
			GPara_ChutePush_5.Setting_Para = Recive[7];
	        count_Parameter_deliveryR++;	
		break;
		
		case 7:
			GPara_ChutePush_5.Setting_Para = (GPara_ChutePush_5.Setting_Para<<8);
			GPara_ChutePush_5.Setting_Para = GPara_ChutePush_5.Setting_Para+Recive[2];
			GPara_ChutePush_6.Setting_Para = Recive[3];
			GPara_ChutePush_7.Setting_Para = Recive[4];
		
			GPara_Telescopict_2.Setting_Para = Recive[5];
			GPara_Telescopict_3.Setting_Para = Recive[6];
			GPara_Telescopict_4.Setting_Para = Recive[7];
		    count_Parameter_deliveryR++;
			
		


		break;
		
		case 8:
			GPara_Telescopict_5.Setting_Para = Recive[2];
			GPara_Telescopict_6.Setting_Para = Recive[3];
			GPara_Telescopict_8.Setting_Para = Recive[4];
			GPara_Telescopict_9.Setting_Para = Recive[5];
			GPara_ChutePull_2.Setting_Para = Recive[6];
			GPara_ChutePull_3.Setting_Para = Recive[7];
		    count_Parameter_deliveryR++;

		break;
		
		case 9:
		GPara_ChutePull_4.Setting_Para = Recive[2];
		GPara_ChutePull_5.Setting_Para = Recive[3];
		GPara_ChutePull_6.Setting_Para = Recive[4];
		GPara_ChutePull_8.Setting_Para = Recive[5];
		GPara_ChutePull_9.Setting_Para = Recive[6];
		GPara_ChutePull_10.Setting_Para = Recive[7];
		count_Parameter_deliveryR++;
		if(count_Parameter_deliveryR == 10)
		{
			Clear_Screen();
			Display_String(3,32,"参数传递完成");
			count_Parameter_deliveryR = 0;
			SaveParaToFlash_WritingFun();
			Set_Init_char();
		}
		else if(count_Parameter_deliveryR != 10)
		{
			Clear_Screen();
			Display_String(3,32,"参数传递不全");
			count_Parameter_deliveryR = 0;
		}
		vTaskDelay(500);
		xQueueSend(Interface_Mian_Logic_Queue,&const_Mainlogic_InterfaceFlag,0);
		break;
		
		
		default:
		break;
	}
	
	
	
	
}
