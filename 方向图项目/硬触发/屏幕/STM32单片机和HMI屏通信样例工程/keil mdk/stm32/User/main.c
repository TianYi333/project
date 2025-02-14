#include "stm32f10x.h"   
#include "tjc_usart_hmi.h"
#include "stm32f10x_rcc.h"

#define FRAMELENGTH 6

void NVIC_Configuration(void);
void RCC_Configuration(void);

int main(void)
{
	RCC_Configuration();
	NVIC_Configuration();
	USART1_Init(115200);	  	 //���ڳ�ʼ��Ϊ115200
	TJCPrintf("\x00");          //Ϊȷ������HMI����ͨ��
	
	
	while(1)
	{
	
	//stm32f103��GND�Ӵ������򴮿ڹ��ߵ�GND,����
	//stm32f103��TX1(PA9)�Ӵ������򴮿ڹ��ߵ�RX
	//stm32f103��RX1(PA10)�Ӵ������򴮿ڹ��ߵ�TX
	//stm32f103��5V�Ӵ�������5V,����Ǵ��ڹ���,���ý�5VҲ����
		
	//�������ݸ�ʽ��
	//��������֡���ȣ�6�ֽ�
	//֡ͷ      led���  LED״̬    ֡β
	//0x55      1�ֽ�    1�ֽ�     0xffffff
	//����1����λ������  printh 55 01 00 ff ff ff  ���壺1��led�ر�
	//����2����λ������  printh 55 04 01 ff ff ff  ���壺4��led��
	//����3����λ������  printh 55 00 01 ff ff ff  ���壺0��led��
	//����4����λ������  printh 55 04 00 ff ff ff  ���壺4��led�ر�
	  while(usize >= FRAMELENGTH)
	  {
		  //У��֡ͷ֡β�Ƿ�ƥ��
		  if(u(0) != 0x55 || u(3) != 0xff || u(4) != 0xff || u(5) != 0xff)
		  {
			  //��ƥ��ɾ��1�ֽ�
			  udelete(1);
		  }else
		  {
			  //ƥ�䣬����ѭ��
			  break;
		  }

	  }

	  //���н���
	  if(usize >= FRAMELENGTH && u(0) == 0x55 && u(3) == 0xff && u(4) == 0xff && u(5) == 0xff)
	  {
		  TJCPrintf("msg.txt=\"led %d is %s\"", u(1), u(2) ? "on" : "off");
		  udelete(FRAMELENGTH);
	  }		

		//delay_ms(1000);
		
		
	}
	
}


void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* Ƕ�������жϿ�������ѡ�� */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  
  /* ����USARTΪ�ж�Դ */
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  /* �������ȼ�*/
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  /* �����ȼ� */
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  /* ʹ���ж� */
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  /* ��ʼ������NVIC */
  NVIC_Init(&NVIC_InitStructure);
}


void RCC_Configuration(void){ //RCCʱ�ӵ�����  
	ErrorStatus HSEStartUpStatus;   
	RCC_DeInit();              /* RCC system reset(for debug purpose) RCC�Ĵ����ָ���ʼ��ֵ*/   
	RCC_HSEConfig(RCC_HSE_ON); /* Enable HSE ʹ���ⲿ���پ���*/   
	HSEStartUpStatus = RCC_WaitForHSEStartUp(); /* Wait till HSE is ready �ȴ��ⲿ���پ���ʹ�����*/   
	if(HSEStartUpStatus == SUCCESS){   
		/*����PLLʱ��Դ����Ƶϵ��*/   
		RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9); //RCC_PLLMul_x��ö��2~16���Ǳ�Ƶֵ����HSE=8MHZ,RCC_PLLMul_9ʱPLLCLK=72MHZ   
		/*����AHBʱ�ӣ�HCLK��*/   
		RCC_HCLKConfig(RCC_SYSCLK_Div1); //RCC_SYSCLK_Div1����AHBʱ�� = ϵͳʱ��(SYSCLK) = 72MHZ���ⲿ����8HMZ��   
		/*ע��˴������ã����ʹ��SYSTICK����ʱ���򣬴�ʱSYSTICK(Cortex System timer)=HCLK/8=9MHZ*/   
		RCC_PCLK1Config(RCC_HCLK_Div2); //���õ���AHBʱ�ӣ�PCLK1��,RCC_HCLK_Div2����APB1ʱ�� = HCLK/2 = 36MHZ���ⲿ����8HMZ��   
		RCC_PCLK2Config(RCC_HCLK_Div1); //���ø���AHBʱ�ӣ�PCLK2��,RCC_HCLK_Div1����APB2ʱ�� = HCLK = 72MHZ���ⲿ����8HMZ��   
		/*ע��AHB��Ҫ�����ⲿ�洢��ʱ�ӡ�APB2����AD��I/O���߼�TIM������1��APB1����DA��USB��SPI��I2C��CAN������2��3��4��5����ͨTIM */  
		FLASH_SetLatency(FLASH_Latency_2); //����FLASH�洢����ʱʱ��������   
		/*FLASHʱ���ӳټ������ڣ��ȴ�����ͬ��������   
		�Ƽ����յ�Ƭ��ϵͳ����Ƶ�ʣ�
		0��24MHzʱ��ȡLatency_0��   
		24��48MHzʱ��ȡLatency_1��   
		48~72MHzʱ��ȡLatency_2*/   
		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable); //ѡ��FLASHԤȡָ�����ģʽ��Ԥȡָ����ʹ��   
		RCC_PLLCmd(ENABLE);	//ʹ��PLL
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET); //�ȴ�PLL����ȶ�   
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK); //ѡ��SYSCLKʱ��ԴΪPLL
		while(RCC_GetSYSCLKSource() != 0x08); //�ȴ�PLL��ΪSYSCLKʱ��Դ   
	}  
	/*��ʼʹ�ܳ�������Ҫʹ�õ�����ʱ��*/   
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOD, ENABLE); //APB2����ʱ��ʹ��      
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); 
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
}  














