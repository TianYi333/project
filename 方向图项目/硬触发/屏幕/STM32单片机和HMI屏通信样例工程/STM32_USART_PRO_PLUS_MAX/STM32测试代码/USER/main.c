/*********************************************************************************/	 
//STM32������ʾ������
//�������ϣ�http://wiki.tjc1688.com/
//��Ƭ��STM32F103C8T6  �����ʣ�9600									  
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
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_Configuration(); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(9600);	  	 //���ڳ�ʼ��Ϊ9600
	HMISendstart();          //Ϊȷ������HMI����ͨ��
	
	HMISends("cls RED");  //���ʹ���ָ��(ˢ��Ϊ��ɫ)
	HMISendb(0xff);		  //���ͽ����� 0XFF 0XFF 0XFF
	delay_ms(1000);			//��ʱ1s
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
	HMISends("t0.txt=\"���粻�����Ź�\"");	//��t0�ı���ʾ"���粻�����Ź�"
	HMISendb(0xff);
	HMISends("xstr 0,0,100,50,0,BLUE,WHITE,1,1,3,\"�й�\"");	//������ʹ������ָ��ȥ��ʾ���֣�����ʹ���ı��ؼ���ʾ������
	HMISendb(0xff);	
			
	while(1)
	{
		//-----------------------------���ͱ仯���ַ���ʼ--------------------------------
		
		//����һ���ַ�������
		char tjcstr[100];
		
		/********ʾ��1:���ı��ؼ���ֵ**********
		��sprintf����ʽ���ַ���,��t0��txt���Ը�ֵ��
		���ؼ����ı����Ը�ֵʱ,������Ҫ�óɶԵ� \" \" ����ס,
		����ʽ���Ĳ���Ϊ�ַ���ʱ���뱣֤�ַ�����\0��β
		*****************************************/
		sprintf(tjcstr, "t1.txt=\"������%d\"", a);

		//���ַ������ͳ�ȥ
		HMISends(tjcstr);

		//���ͽ�����
		HMISendb(0xff);
		
		
		/********ʾ��2:�����ֿؼ���ֵ**********
		//��sprintf����ʽ���ַ�������n0.val���Ը�ֵ��ͬʱ���Ͻ�����,
		��ע���val���Ը�ֵʱ,��û�� \" \" ��,
		����ʽ���Ĳ���Ϊ�ַ���ʱ���뱣֤�ַ�����\0��β
		*****************************************/
		sprintf(tjcstr, "n0.val=%d\xff\xff\xff", a);
		
		//���ַ������ͳ�ȥ
		HMISends(tjcstr);
		
		//���÷��ͽ�����,��Ϊ�������Ѿ����������ַ�������
		
		
		
		/********ʾ��3:��ҳ�渳ֵ**********
		//��sprintf����ʽ���ַ�������settingҳ���count.val���Ը�ֵ��ͬʱ���Ͻ�����,
		��ע���ҳ�渳ֵʱ,��Ҫ�ѿؼ�����Ϊȫ��,
		*****************************************/
		sprintf(tjcstr, "setting.count.val=%d\xff\xff\xff", a);
		
		//���ַ������ͳ�ȥ
		HMISends(tjcstr);
		
		//���÷��ͽ�����,��Ϊ�������Ѿ����������ַ�������		
		
		
		//��ʱ1000ms
		delay_ms(1000);
		a++;

		//-------------------------------------���ͱ仯���ַ�����-------------------------------------			
	}
}
 
//�ַ������ͺ���
void HMISends(char *buf1)		  
{
	u8 i=0;
	while(1)
	{
		if(buf1[i] != 0)
	 	{
			USART_SendData(USART1,buf1[i]);  //����һ���ֽ�
			while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET){};//�ȴ����ͽ���
		 	i++;
		}
		else
		{
			return ;
		}
	}
}

//�ֽڷ��ͺ���
void HMISendb(u8 k)		         
{		 
	u8 i;
	 for(i=0; i<3; i++)
	 {
			if(k != 0)
			{
				USART_SendData(USART1,k);  //����һ���ֽ�
				while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET){};//�ȴ����ͽ���
			}
			else
			{
				return ;
			}
	 } 
} 



