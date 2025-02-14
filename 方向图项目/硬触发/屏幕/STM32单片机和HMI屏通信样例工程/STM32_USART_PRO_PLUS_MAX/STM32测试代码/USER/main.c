/*********************************************************************************/	 
//STM32串口屏示例程序
//更多资料：http://wiki.tjc1688.com/
//单片机STM32F103C8T6  波特率：9600									  
/*********************************************************************************/	

#include "delay.h"
#include "sys.h"
#include "usart.h"	
#include <string.h>
u8 idfind(u8 *buf,u8 *val,u8 len);
void HMISends(char *buf1);
void HMISendb(u8 buf);
void beepms(u16 va);
u8 key,beep;
int a = 0;
void beepms(u16 va)
{
	beep=1;
	delay_ms(va);
	beep=0;
}

void HMISendstart(void)
	{
	 	delay_ms(200);
		HMISendb(0xff);
		delay_ms(200);
	}
						
int main(void)
{	   	 
	delay_init();	    	 //延时函数初始化	  
	NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(9600);	  	 //串口初始化为9600
	HMISendstart();          //为确保串口HMI正常通信
	
	HMISends("cls RED");  //发送串口指令(刷屏为红色)
	HMISendb(0xff);		  //发送结束符 0XFF 0XFF 0XFF
	delay_ms(1000);			//延时1s
	HMISends("cls GREEN");	
	HMISendb(0xff);
	delay_ms(1000);
	HMISends("cls BLUE");
	HMISendb(0xff);
	delay_ms(1000);
	HMISends("cls BLACK");
	HMISendb(0xff);
	delay_ms(1000);
	HMISends("cls WHITE");
	HMISendb(0xff);
	delay_ms(1000);
	HMISends("t0.txt=\"春风不度玉门关\"");	//让t0文本显示"春风不度玉门关"
	HMISendb(0xff);
	HMISends("xstr 0,0,100,50,0,BLUE,WHITE,1,1,3,\"中国\"");	//不建议使用这种指令去显示汉字，建议使用文本控件显示的文字
	HMISendb(0xff);	
			
	while(1)
	{
		//-----------------------------发送变化的字符开始--------------------------------
		
		//定义一个字符串数组
		char tjcstr[100];
		
		/********示例1:给文本控件赋值**********
		用sprintf来格式化字符串,给t0的txt属性赋值，
		给控件的文本属性赋值时,内容需要用成对的 \" \" 包裹住,
		当格式化的参数为字符串时，请保证字符串以\0结尾
		*****************************************/
		sprintf(tjcstr, "t1.txt=\"现在是%d\"", a);

		//把字符串发送出去
		HMISends(tjcstr);

		//发送结束符
		HMISendb(0xff);
		
		
		/********示例2:给数字控件赋值**********
		//用sprintf来格式化字符串，给n0.val属性赋值，同时加上结束符,
		请注意给val属性赋值时,是没有 \" \" 的,
		当格式化的参数为字符串时，请保证字符串以\0结尾
		*****************************************/
		sprintf(tjcstr, "n0.val=%d\xff\xff\xff", a);
		
		//把字符串发送出去
		HMISends(tjcstr);
		
		//不用发送结束符,因为结束符已经被整合在字符串中了
		
		
		
		/********示例3:跨页面赋值**********
		//用sprintf来格式化字符串，给setting页面的count.val属性赋值，同时加上结束符,
		请注意跨页面赋值时,需要把控件设置为全局,
		*****************************************/
		sprintf(tjcstr, "setting.count.val=%d\xff\xff\xff", a);
		
		//把字符串发送出去
		HMISends(tjcstr);
		
		//不用发送结束符,因为结束符已经被整合在字符串中了		
		
		
		//延时1000ms
		delay_ms(1000);
		a++;

		//-------------------------------------发送变化的字符结束-------------------------------------			
	}
}
 
//字符串发送函数
void HMISends(char *buf1)		  
{
	u8 i=0;
	while(1)
	{
		if(buf1[i] != 0)
	 	{
			USART_SendData(USART1,buf1[i]);  //发送一个字节
			while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET){};//等待发送结束
		 	i++;
		}
		else
		{
			return ;
		}
	}
}

//字节发送函数
void HMISendb(u8 k)		         
{		 
	u8 i;
	 for(i=0; i<3; i++)
	 {
			if(k != 0)
			{
				USART_SendData(USART1,k);  //发送一个字节
				while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET){};//等待发送结束
			}
			else
			{
				return ;
			}
	 } 
} 



