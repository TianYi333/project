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
	USART1_Init(115200);	  	 //串口初始化为115200
	TJCPrintf("\x00");          //为确保串口HMI正常通信
	
	
	while(1)
	{
	
	//stm32f103的GND接串口屏或串口工具的GND,共地
	//stm32f103的TX1(PA9)接串口屏或串口工具的RX
	//stm32f103的RX1(PA10)接串口屏或串口工具的TX
	//stm32f103的5V接串口屏的5V,如果是串口工具,不用接5V也可以
		
	//串口数据格式：
	//串口数据帧长度：6字节
	//帧头      led编号  LED状态    帧尾
	//0x55      1字节    1字节     0xffffff
	//例子1：上位机代码  printh 55 01 00 ff ff ff  含义：1号led关闭
	//例子2：上位机代码  printh 55 04 01 ff ff ff  含义：4号led打开
	//例子3：上位机代码  printh 55 00 01 ff ff ff  含义：0号led打开
	//例子4：上位机代码  printh 55 04 00 ff ff ff  含义：4号led关闭
	  while(usize >= FRAMELENGTH)
	  {
		  //校验帧头帧尾是否匹配
		  if(u(0) != 0x55 || u(3) != 0xff || u(4) != 0xff || u(5) != 0xff)
		  {
			  //不匹配删除1字节
			  udelete(1);
		  }else
		  {
			  //匹配，跳出循环
			  break;
		  }

	  }

	  //进行解析
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
  
  /* 嵌套向量中断控制器组选择 */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  
  /* 配置USART为中断源 */
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  /* 抢断优先级*/
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  /* 子优先级 */
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  /* 使能中断 */
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  /* 初始化配置NVIC */
  NVIC_Init(&NVIC_InitStructure);
}


void RCC_Configuration(void){ //RCC时钟的设置  
	ErrorStatus HSEStartUpStatus;   
	RCC_DeInit();              /* RCC system reset(for debug purpose) RCC寄存器恢复初始化值*/   
	RCC_HSEConfig(RCC_HSE_ON); /* Enable HSE 使能外部高速晶振*/   
	HSEStartUpStatus = RCC_WaitForHSEStartUp(); /* Wait till HSE is ready 等待外部高速晶振使能完成*/   
	if(HSEStartUpStatus == SUCCESS){   
		/*设置PLL时钟源及倍频系数*/   
		RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9); //RCC_PLLMul_x（枚举2~16）是倍频值。当HSE=8MHZ,RCC_PLLMul_9时PLLCLK=72MHZ   
		/*设置AHB时钟（HCLK）*/   
		RCC_HCLKConfig(RCC_SYSCLK_Div1); //RCC_SYSCLK_Div1——AHB时钟 = 系统时钟(SYSCLK) = 72MHZ（外部晶振8HMZ）   
		/*注意此处的设置，如果使用SYSTICK做延时程序，此时SYSTICK(Cortex System timer)=HCLK/8=9MHZ*/   
		RCC_PCLK1Config(RCC_HCLK_Div2); //设置低速AHB时钟（PCLK1）,RCC_HCLK_Div2——APB1时钟 = HCLK/2 = 36MHZ（外部晶振8HMZ）   
		RCC_PCLK2Config(RCC_HCLK_Div1); //设置高速AHB时钟（PCLK2）,RCC_HCLK_Div1——APB2时钟 = HCLK = 72MHZ（外部晶振8HMZ）   
		/*注：AHB主要负责外部存储器时钟。APB2负责AD，I/O，高级TIM，串口1。APB1负责DA，USB，SPI，I2C，CAN，串口2，3，4，5，普通TIM */  
		FLASH_SetLatency(FLASH_Latency_2); //设置FLASH存储器延时时钟周期数   
		/*FLASH时序延迟几个周期，等待总线同步操作。   
		推荐按照单片机系统运行频率：
		0—24MHz时，取Latency_0；   
		24—48MHz时，取Latency_1；   
		48~72MHz时，取Latency_2*/   
		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable); //选择FLASH预取指缓存的模式，预取指缓存使能   
		RCC_PLLCmd(ENABLE);	//使能PLL
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET); //等待PLL输出稳定   
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK); //选择SYSCLK时钟源为PLL
		while(RCC_GetSYSCLKSource() != 0x08); //等待PLL成为SYSCLK时钟源   
	}  
	/*开始使能程序中需要使用的外设时钟*/   
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOD, ENABLE); //APB2外设时钟使能      
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); 
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
}  














