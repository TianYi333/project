#ifndef __CANRECEIVE_H__
#define __CANRECEIVE_H__
#include "main.h"



typedef uint8_t  u8;

//typedef struct _variant_BUScheak1{
//	u8 head;		
//	u8 divicenum;		//单动方向
//	u8 Sensor;		//单动动作类型

//}_variant_BUScheak1;




/**********************************************数组定义*******************************************************/
extern uint8_t array_Canreceive_CanLRcmdR[8];//传递命令变量  创建人蒋博 创建日期2023/02/3
extern uint8_t array_Canreceive_CanBUScmdR[8];//传递命令变量  创建人蒋博 创建日期2023/02/3
extern uint8_t array_Canreceive_CanLRinnerT[8];//传递命令变量  创建人蒋博 创建日期2023/02/3
extern uint8_t array_Canreceive_CanBUSinnerT[8];//传递命令变量  创建人蒋博 创建日期2023/02/3
extern uint8_t variant_BUS_continue;
extern uint8_t variant_Canreceive_STATE ;//控制器状态   传递命令变量  添加人任晓宇 添加日期2023/12/24

void LRcan_Reveice_Fuction(void);

/*******************can邻架通信前置分类******************/
#define   TPYE_SINGAL               0x01      //单动
#define   TPYE_GROUP                0x02      //成组
#define   TPYE_SET_GLOBALPAR        0x03      //设置全局参数
#define   TPYE_SERIALNUMBER_L       0x04      //架号传递左
#define   TPYE_PARAMETER            0x05      //参数
#define   TPYE_PARAMETER_L          0x06      //参数左
#define   TPYE_PARAMETER_R          0x07      //参数右
#define   TPYE_GROUP_ALARM          0x08      //成组报警
#define   TPYE_FOLLOW_STRAT         0x09      //跟机开始
#define   TPYE_SERIALNUMBER_R       0x0a      //架号传递右
#define   TPYE_GROUP_CANAEL         0x10      //成组取消
#define   TPYE_RESET                0x0b       //邻架重启

#define   PITCH_ON   0xc0  //选中
#define   SINGAL_MOVE   0xc1  //单动
#define   GROUP_MOVE_CMD   0xc2  //成组动作
#define   GROUP_CMD   0xc5  //成组选中
#define   CANCEL   0xc3  //取消
#define   DEVICE_CONNECT  0xc4  //遥控连接

#define   PLICHRETURN_1     0xcc
#define   PLICHRETURN_2     0xcd
#define   CANCELRETURN_1     0xca
#define   CANCELRETURN_2     0xcb


/*******************can总线接收通信前置分类******************/
#define   BUS_R_PROGRAM            0xAA   //程序传递


/*******************can总线通信前置分类******************/
#define   BUS_STOP_LOCK            0x12   //急停
#define   BUS_CHEAK_R            0xE1   //总线检测
#define   BUS_CHEAK_POSITION            0xE4   //总线检测


#define   BUS_FOLLOW_START           0x5A   //总线检测
#define   BUS_FOLLOW_FAIL           0x5E   //总线失败
#define   BUS_FOLLOW_STOP           0x5F   //总线停止
#define   BUS_FOLLOW_MOVE           0x51   //总线跟机动作

#define   BUS_COMPUTER_MOVE           0x61   //总线跟机动作
#define   BUS_COMPUTER_CHOSE          0x60   //总线跟机动作
#define   BUS_COMPUTER_CANCEL          0x62   //总线跟机动作

/*******************can总线 发送 通信前置分类******************/
#define   BUS_CHEAK_SEND            0x01   //总线检测
#define   BUS_FOLLOWSTART_SEND            0x4A   //跟机开始检测
#define   BUS_FOLLOWSTART_R_SEND            0x40   //跟机开启回复
#define   BUS_FOLLOWMOVE_R_SEND            0x41   //跟机动作回复
#define   BUS_FOLLOWMOVE_STOP            0x4D   //跟机动作回复
#define   BUS_GROUP_STOP            0x02   //跟机动作回复
/**********************************************数组定义*******************************************************/





//跟机自动化控制标志位  接收
#define INLET_SAFEGUARD_BEGIN 0X01       //伸出前护帮板（开始）
//#define INLET_SAFEGUARD_PAUSE 0X02       //伸出前护帮板（暂停）
//#define INLET_SAFEGUARD_STOP 0X03        //伸出前护帮板（中止）
//#define INLET_SAFEGUARD_END 0X04         //伸出前护帮板（结束）

#define OUTLET_SAFEGUARD_BEGIN 0X06      //收回前护帮板（开始）
//#define OUTLET_SAFEGUARD_PAUSE 0X07      //收回前护帮板（暂停）
//#define OUTLET_SAFEGUARD_STOP 0X08       //收回前护帮板（中止）
//#define OUTLET_SAFEGUARD_END 0X09        //收回前护帮板（结束）

#define INLET_FRONTBEAM_BEGIN 0X11       //伸出伸缩梁（开始）
//#define INLET_FRONTBEAM_PAUSE 0X12       //伸出伸缩梁（暂停）
//#define INLET_FRONTBEAM_STOP 0X13        //伸出伸缩梁（中止）
//#define INLET_FRONTBEAM_END 0X14         //伸出伸缩梁（结束）

#define OUTLET_FRONTBEAM_BEGIN 0X16      //收回伸缩梁（开始）
//#define OUTLET_FRONTBEAM_PAUSE 0X17      //收回伸缩梁（暂停）
//#define OUTLET_FRONTBEAM_STOP 0X18       //收回伸缩梁（中止）
//#define OUTLET_FRONTBEAM_END 0X19        //收回伸缩梁（结束）

#define PUSH_SCRAPER_BEGIN 0X21          //推溜（开始）
//#define PUSH_SCRAPER_PAUSE 0X22          //推溜（暂停）
//#define PUSH_SCRAPER_STOP 0X23           //推溜（中止）
//#define PUSH_SCRAPER_END 0X24            //推溜（结束）

#define AUTO_MOVE_MACHINE_BEGIN 0X36            //自动移架（开始）：收前护（收到位）、收伸缩梁（收到位）
//#define AUTO_MOVE_MACHINE_DECLINE_LEG 0X37      //自动移架（降柱、收侧护）
//#define AUTO_MOVE_MACHINE_PULL_SCRAPER 0X38     //自动移架（拉架）
//#define AUTO_MOVE_MACHINE_RISE_LEG 0X39         //自动移架（伸侧护、升柱）
//#define AUTO_MOVE_MACHINE_INLET_SAFEGUARD 0X3A  //自动移架（伸前护）
//#define AUTO_MOVE_MACHINE_END 0X3C              //自动移架（结束）



/*******************can总线发送通信前置分类******************/
/******************添加人任晓宇 添加日期2023/12/24******************/
#define   BUS_SEND_MOVE              0x21  //回复上位机动作开始，失败，中止等状态
#define   BUS_SEND_SINFORMATION      0x22 
#define   BUS_SEND_BUSCHEAK1         0x23 
#define   BUS_SEND_BUSCHEAK2         0x24 
#define   BUS_SEND_FAULT             0x25 
#define   BUS_SEND_STATE             0x28  //上报控制器状态//一字节：校验位；二字节：架号；三字节：状态号。

#define   BUS_STOP            0x12   //急停  广播
#define   BUS_LOCK            0x13   //闭锁   上报上位机


#define   BUS_R_CHEAK_R            0x26    //收到后回复本机参数
#define   BUS_R_POSITION           0x27    //收到煤机位置

/*******************can跟机接收通信前置分类******************/

#define   BUS_FOLLOW_STARTCHEAK_R          0x3B   //开通跟机指示，上位机检测设备是否在线
#define   BUS_FOLLOW_FAIL_R           0x3E   //总线失败
#define   BUS_FOLLOW_STOP_R           0x3F   //总线停止
#define   BUS_FOLLOW_MOVE_R           0x3C   //总线跟机动作
#define   BUS_FOLLOW_START_R          0x36   //上位机开启跟机指示，通报上位机跟机工艺及数据
/*******************can跟机发送通信前置分类******************/
#define   BUS_FOLLOW_STARTCHEAK_T          0x3B   //开通跟机回复，控制器回复上位机检测设备是否在线
#define   BUS_FOLLOW_MOVE_T            0x21   //跟机动作回复----开始
#define   BUS_FOLLOW_START_T          0x36   //下位机开启跟机指示，通报上位机跟机工艺及数据




#define   BUS_COMPUTER_MOVE           0x61   //总线跟机动作  远程单动
#define   BUS_COMPUTER_CHOSE          0x60   //总线跟机动作  远程单动
#define   BUS_COMPUTER_CANCEL          0x62   //总线跟机动作  远程单动

/*******************can总线 发送 通信前置分类******************/

#define   BUS_GROUP_STOP            0x02   //成组控制结束标志位


//void Fuction_LRCANmessage_send(void);
void BUScan_Reveice_Fuction(void);
#endif



