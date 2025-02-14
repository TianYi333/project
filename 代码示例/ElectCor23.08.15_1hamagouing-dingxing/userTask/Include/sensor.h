#ifndef __SENSOR_H__
#define __SENSOR_H__
#include "stm32f4xx_hal.h"



#define SENSOR_DISTANCE_RABGE   1000    //单位毫米mm
#define SENSOR_DISTANCE_VOLTAGE_V 284    //单位0.00v
#define SENSOR_DISTANCE_VOLTAGE_L 71    //单位0.00v

#define SENSOR_PRESSURE_RABGE 60    //单位0.1Mpa
#define SENSOR_PRESSURE_VOLTAGE_V 40    //单位0.00v
#define SENSOR_PRESSURE_VOLTAGE_L 50    //单位0.00v

#define SENSOR_SCRANCONTROL_VALUE 500    //急停最低值


typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;
//无符号
 
typedef int32_t  s32;
typedef int16_t s16;
typedef int8_t  s8;

/**********************************************************结构体定义*************************************************************************************/

typedef struct _variant_sensor{
			uint16_t sensor_value;		//传感器值						
    char sensor_view[8];

}_variant_sensor;




/**************复制区*************/
/******************
_sensor_distance.sensor_view[7]
_sensor_pressure.sensor_view[7]
_sensor_Connectingrod_angle.sensor_view[7]
_sensor_canopy_angle.sensor_view[7]
_sensor_Plate_angle.sensor_view[7]

_sensor_distance.sensor_value
_sensor_pressure.sensor_value
_sensor_Connectingrod_angle.sensor_value
_sensor_canopy_angle.sensor_value
_sensor_Plate_angle.sensor_value
************/
/**************复制区*************/




/*******************************************对外声明************************************************************************************************************/

extern uint8_t   Count_Infrared_Sensor;     //红外计数  创建人蒋博 创建日期2023/1/05
extern uint8_t   Sensor_Receive_Num;//采煤机脉冲数
extern uint32_t  array_adc_receive[3];
extern uint8_t   array_Angel_receive[20];
extern uint8_t   Flag_Angel_receive;
 extern struct  _variant_sensor  _sensor_distance  ;     //位移
 extern struct  _variant_sensor  _sensor_pressure  ;     //压力
 extern struct  _variant_sensor  _sensor_Connectingrod_angle;      //连杆倾角
 extern struct  _variant_sensor  _sensor_canopy_angle;      //顶梁倾角
 extern struct  _variant_sensor  _sensor_Plate_angle;      //互帮倾角

extern  uint8_t  array_Angel_send[3];
extern uint16_t	 variant_Altitude_Vlaue;
extern char      array_Altitude_view[5];
extern int P_random;
extern uint8_t min_random;
void Sensor_Receive_rfuc(void);
void usart7_send_char(uint8_t c);
void usart7_niming_report(uint8_t len);
void Sensor_angeluart7_send_data(uint8_t *tdata,uint16_t tnum);
 void Function_sonser_pcReported(uint8_t type);//传感器上报函数  添加人任晓宇 添加日期2023/12/24

#endif



