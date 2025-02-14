#include "Bsp_Battery.h"

uint32_t m_adc;

/*
������⺯��
��ʱδ������Ϳյ���ʱ��adcֵ
1600----��û��ʱ��ֵ
2040----��������
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
      HAL_ADC_Start(&hadc1);    //����ADCת��
      HAL_ADC_PollForConversion(&hadc1,0x10);  //�ȴ�ת�����
      m_adc = HAL_ADC_GetValue(&hadc1);	//��ȡת��ֵ
      batery_sub += m_adc;
      vTaskDelay(100);
    }
    
    ava_batery = batery_sub / 10;
    
    if(ava_batery < 1800)
    {
      //����������ʾ
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
