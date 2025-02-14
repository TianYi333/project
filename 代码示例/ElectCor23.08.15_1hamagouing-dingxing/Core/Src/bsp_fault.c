#include "bsp_fault.h"

uint8_t near_test[8] = {TPYE_NEAR_TEST, 00, 33, 00, 00, 00, 00, 00};
uint16_t near_pre[2];
uint8_t near_flag[2];


/*�ټܼ��ʱ����Ҫ����5s����*/
void Fault_Near_Function(void)
{
  while(1)
  {  
    if(Structset_GlobalSet_Adjacentcheak8_2.Setting_Para)
    {
      //�����ټܷ���
      near_test[1] = 0x11;
      near_test[4] = _sensor_pressure.sensor_value >> 8;
      near_test[5] = _sensor_pressure.sensor_value & 0xFF;
      CANx_SendNormalData(&hcan1, 0x10, near_test, 8);
      
      vTaskDelay(1000);

      //�����ټܷ���

      near_test[1] = 0x22;
      near_test[4] = _sensor_pressure.sensor_value >> 8;
      near_test[5] = _sensor_pressure.sensor_value & 0xFF;
      CANx_SendNormalData(&hcan3, 0x10, near_test, 8);
      
      vTaskDelay(1000);
    
      //�����ټܷ���
      near_test[1] = 0x11;
      near_test[4] = _sensor_pressure.sensor_value >> 8;
      near_test[5] = _sensor_pressure.sensor_value & 0xFF;
      CANx_SendNormalData(&hcan1, 0x10, near_test, 8);
      
      vTaskDelay(1000);

      //�����ټܷ���

      near_test[1] = 0x22;
      near_test[4] = _sensor_pressure.sensor_value >> 8;
      near_test[5] = _sensor_pressure.sensor_value & 0xFF;
      CANx_SendNormalData(&hcan3, 0x10, near_test, 8);
      
      vTaskDelay(2000);

      //�ж�ͨѶ״̬

        
      if(near_flag[0] == 0)
      {
        //�ж��Լ��Ƿ�Ϊ����
        //����ʱΪ���ܺ�
        if(!(Structset_GlobalSet_Increase4_4.Setting_Para == DIRECTION_LEFT && Structset_Service_DeviceNum1_1.Setting_Para == Structset_GlobalSet_MaxDeviceNum1_1.Setting_Para)
        //����ʱΪ��С�ܺ�
          || (Structset_GlobalSet_Increase4_4.Setting_Para == DIRECTION_RIGHT && Structset_Service_DeviceNum1_1.Setting_Para == Structset_GlobalSet_minDeviceNum2_1.Setting_Para))
        {
          //���ټ�ͨѶ�쳣
          Structset_Fault_Lcommunicate1_5.Setting_Para = 1;
        }
      }
      
      if(near_flag[1] == 0)
      {
        //�ж��Լ��Ƿ�Ϊ����
        //����ʱΪ��С�ܺ�
        if(!(Structset_GlobalSet_Increase4_4.Setting_Para == DIRECTION_LEFT && Structset_Service_DeviceNum1_1.Setting_Para == Structset_GlobalSet_minDeviceNum2_1.Setting_Para)
        //����ʱΪ���ܺ�
          || (Structset_GlobalSet_Increase4_4.Setting_Para == DIRECTION_RIGHT && Structset_Service_DeviceNum1_1.Setting_Para == Structset_GlobalSet_MaxDeviceNum1_1.Setting_Para))
        {
          //���ټ�ͨѶ�쳣
          Structset_Fault_Rcommunicate2_5.Setting_Para = 1;
        }
      }
      
      
      near_flag[0] = 0;
      near_flag[1] = 0;
        
      vTaskDelay((Structset_GlobalSet_Adjacenttime10_1.Setting_Para - 5) * 1000);
    }
    else
    {
      //�����
      vTaskDelay(5000);
    }
  }
}

void Fault_Pre_Function(void)
{
  uint8_t up_time = 0, try_time = 0, break_flag = 0;
  while(1)
  {
    //�жϵ�ǰΪ����״̬,�Ҵ�ѹ�����
    if(_variant_Mainlogic_statement.PRESENTstate_value == State_Mainlogic_DWFAULTstate && Structset_Default_Pressurecheak1_2.Setting_Para)
    {
      //���ڲ�ѹ���ޣ��ж�Ϊ�ֶ���ѹ�������п���
      if(_sensor_pressure.sensor_value >= Structset_Default_PressureHighlimt6_1.Setting_Para)
      {
        //��������ʱ��Ҫ�Զ���ѹ
        if(_sensor_pressure.sensor_value <= Structset_Default_PressureHighlimt6_1.Setting_Para)
        {
          //��ѹ����ѭ��
          for(try_time = 1; try_time <= Structset_Default_Stepuppressuretimes10_1.Setting_Para; try_time++)
          {
            for(up_time = 0; up_time < Structset_Default_StepuppressureTime8_1.Setting_Para * 1000; up_time++)
            {
              //�жϵ�ǰΪ����״̬,�Ҵ�ѹ�����
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
                //״̬��ת�󲻽����Զ���ѹ
                continue;
              }
              MOVE_controled_Driver(MOVE_COLUMN_UP_OUT);
              if(_sensor_pressure.sensor_value > Structset_Default_aimPressure7_1.Setting_Para)
              {
                //����Ŀ��ѹ��
                break_flag = 1;
                break;
              }
            }
            if(break_flag)
            {
              //�Զ���ѹ�ɹ�������ѭ��
              break;
            }
            //���в�ѹ��δ�ﵽĿ��ѹ��
            if(_sensor_pressure.sensor_value <= Structset_Default_aimPressure7_1.Setting_Para)
            {
              if(try_time < Structset_Default_Stepuppressuretimes10_1.Setting_Para)
              {
                //���ʱ��
                vTaskDelay(Structset_Default_StepuppressureGap9_1.Setting_Para);
              }
            }
          }
          if(!break_flag && _sensor_pressure.sensor_value <= Structset_Default_aimPressure7_1.Setting_Para)
          {
            //������һ�β�ѹ���ڣ�����δ�ﵽĿ��ѹ��
            //�Զ���ѹ�쳣
            Structset_Fault_PressureStepup4_5.Setting_Para = 1;
          }
        }
      }
    }
    //���������Ϊ2s
    vTaskDelay(2000);
  }
}
