#include "bsp_fault.h"

uint8_t near_test[8] = {TPYE_NEAR_TEST, 00, 33, 00, 00, 00, 00, 00};
uint16_t near_pre[2];
uint8_t near_flag[2];


/*临架检测时间需要设置5s以上*/
void Fault_Near_Function(void)
{
  while(1)
  {  
    if(Structset_GlobalSet_Adjacentcheak8_2.Setting_Para)
    {
      //向左临架发送
      near_test[1] = 0x11;
      near_test[4] = _sensor_pressure.sensor_value >> 8;
      near_test[5] = _sensor_pressure.sensor_value & 0xFF;
      CANx_SendNormalData(&hcan1, 0x10, near_test, 8);
      
      vTaskDelay(1000);

      //向右临架发送

      near_test[1] = 0x22;
      near_test[4] = _sensor_pressure.sensor_value >> 8;
      near_test[5] = _sensor_pressure.sensor_value & 0xFF;
      CANx_SendNormalData(&hcan3, 0x10, near_test, 8);
      
      vTaskDelay(1000);
    
      //向左临架发送
      near_test[1] = 0x11;
      near_test[4] = _sensor_pressure.sensor_value >> 8;
      near_test[5] = _sensor_pressure.sensor_value & 0xFF;
      CANx_SendNormalData(&hcan1, 0x10, near_test, 8);
      
      vTaskDelay(1000);

      //向右临架发送

      near_test[1] = 0x22;
      near_test[4] = _sensor_pressure.sensor_value >> 8;
      near_test[5] = _sensor_pressure.sensor_value & 0xFF;
      CANx_SendNormalData(&hcan3, 0x10, near_test, 8);
      
      vTaskDelay(2000);

      //判断通讯状态

        
      if(near_flag[0] == 0)
      {
        //判断自己是否为最左
        //左增时为最大架号
        if(!(Structset_GlobalSet_Increase4_4.Setting_Para == DIRECTION_LEFT && Structset_Service_DeviceNum1_1.Setting_Para == Structset_GlobalSet_MaxDeviceNum1_1.Setting_Para)
        //右增时为最小架号
          || (Structset_GlobalSet_Increase4_4.Setting_Para == DIRECTION_RIGHT && Structset_Service_DeviceNum1_1.Setting_Para == Structset_GlobalSet_minDeviceNum2_1.Setting_Para))
        {
          //左临架通讯异常
          Structset_Fault_Lcommunicate1_5.Setting_Para = 1;
        }
      }
      
      if(near_flag[1] == 0)
      {
        //判断自己是否为最右
        //左增时为最小架号
        if(!(Structset_GlobalSet_Increase4_4.Setting_Para == DIRECTION_LEFT && Structset_Service_DeviceNum1_1.Setting_Para == Structset_GlobalSet_minDeviceNum2_1.Setting_Para)
        //右增时为最大架号
          || (Structset_GlobalSet_Increase4_4.Setting_Para == DIRECTION_RIGHT && Structset_Service_DeviceNum1_1.Setting_Para == Structset_GlobalSet_MaxDeviceNum1_1.Setting_Para))
        {
          //右临架通讯异常
          Structset_Fault_Rcommunicate2_5.Setting_Para = 1;
        }
      }
      
      
      near_flag[0] = 0;
      near_flag[1] = 0;
        
      vTaskDelay((Structset_GlobalSet_Adjacenttime10_1.Setting_Para - 5) * 1000);
    }
    else
    {
      //不检测
      vTaskDelay(5000);
    }
  }
}

void Fault_Pre_Function(void)
{
  uint8_t up_time = 0, try_time = 0, break_flag = 0;
  while(1)
  {
    //判断当前为空闲状态,且打开压力检测
    if(_variant_Mainlogic_statement.PRESENTstate_value == State_Mainlogic_DWFAULTstate && Structset_Default_Pressurecheak1_2.Setting_Para)
    {
      //低于补压下限，判断为手动降压，不进行控制
      if(_sensor_pressure.sensor_value >= Structset_Default_PressureHighlimt6_1.Setting_Para)
      {
        //低于上限时需要自动补压
        if(_sensor_pressure.sensor_value <= Structset_Default_PressureHighlimt6_1.Setting_Para)
        {
          //补压次数循环
          for(try_time = 1; try_time <= Structset_Default_Stepuppressuretimes10_1.Setting_Para; try_time++)
          {
            for(up_time = 0; up_time < Structset_Default_StepuppressureTime8_1.Setting_Para * 1000; up_time++)
            {
              //判断当前为空闲状态,且打开压力检测
              if(_variant_Mainlogic_statement.PRESENTstate_value == State_Mainlogic_DWFAULTstate && Structset_Default_Pressurecheak1_2.Setting_Para)
              {
                MOVE_controled_Driver(MOVE_COLUMN_UP_OUT);
                if(_sensor_pressure.sensor_value > Structset_Default_aimPressure7_1.Setting_Para)
                {
                  break;
                }
              }
              else
              {
                //状态跳转后不进行自动补压
                continue;
              }
              MOVE_controled_Driver(MOVE_COLUMN_UP_OUT);
              if(_sensor_pressure.sensor_value > Structset_Default_aimPressure7_1.Setting_Para)
              {
                //到达目标压力
                break_flag = 1;
                break;
              }
            }
            if(break_flag)
            {
              //自动补压成功，跳出循环
              break;
            }
            //进行补压后未达到目标压力
            if(_sensor_pressure.sensor_value <= Structset_Default_aimPressure7_1.Setting_Para)
            {
              if(try_time < Structset_Default_Stepuppressuretimes10_1.Setting_Para)
              {
                //间隔时间
                vTaskDelay(Structset_Default_StepuppressureGap9_1.Setting_Para);
              }
            }
          }
          if(!break_flag && _sensor_pressure.sensor_value <= Structset_Default_aimPressure7_1.Setting_Para)
          {
            //进行了一次补压周期，但还未达到目标压力
            //自动补压异常
            Structset_Fault_PressureStepup4_5.Setting_Para = 1;
          }
        }
      }
    }
    //检测周期设为2s
    vTaskDelay(2000);
  }
}
