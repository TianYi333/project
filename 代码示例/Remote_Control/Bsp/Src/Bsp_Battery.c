#include "Bsp_Battery.h"

uint32_t m_adc;

/*
电量检测函数
暂时未测满电和空电量时的adc值
1600----快没电时的值
2040----几乎满电
*/
void Batery_Function(void)
{
  uint32_t ava_batery,batery_sub;
  uint8_t i, dis_msg[8] = {0};
  uint8_t high_power_time = 0;;
  while(1)
  {
    batery_sub = 0;
    for(i = 0; i < 10; i++)
    {
      HAL_ADC_Start(&hadc1);    //启动ADC转换
      HAL_ADC_PollForConversion(&hadc1,0x10);  //等待转换完成
      m_adc = HAL_ADC_GetValue(&hadc1);	//获取转换值
      batery_sub += m_adc;
      vTaskDelay(100);
    }
    
    ava_batery = batery_sub / 10;
    
    if(ava_batery < 1800)
    {
      //电量不足显示
      dis_msg[0] = LOW_POWER;
      xQueueSend(Queue_Analyze_To_Display_Handle, dis_msg, 10);
      
      high_power_time = 0;
      
    }
    else
    {
      high_power_time++;
      if(high_power_time == 30)
      {
        high_power_time = 0;
        dis_msg[0] = HIGH_POWER;
        xQueueSend(Queue_Analyze_To_Display_Handle, dis_msg, 10);
      }
    }
  }
}
