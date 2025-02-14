#include "bsp_hd7279.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "main.h"
extern QueueHandle_t Key_Value_ISR_Queue;
static 	uint8_t Backvlaue1,Backvlaue2=0;
static void SET_GPIO_MODE_INPUT(void);
static void SET_GPIO_MODE_OUTPUT(void);
unsigned char CH450_buf[6];                 //定义6个数码管的数据映象缓存区

	uint8_t  LED_4vlaue;
	uint8_t  LED_5vlaue;
	uint8_t  LED_6vlaue;
	uint8_t  LED_7vlaue;
	
	
const unsigned char BCD_decode_tab[0x10] = { 0X3F, 0X06, 0X5B, 0X4F, 0X66, 0X6D, 0X7D, 0X07, 0X7F, 0X6F, 0X77, 0X7C, 0X58, 0X5E, 0X79, 0X71 };
void LED_Management_Fanction(uint8_t LED_Light);
void	mDelaymS( unsigned char ms )     // 延时毫秒
{
	unsigned short i;
	while ( ms -- ) {
		for(i=0;i!=1000;i++);
	}
}


static void HD7279_Delay(void)
{
	uint16_t i;
	for (i = 0; i < 1000; i++);
}

void delay_us(void)
{
	uint16_t i;
	for (i = 0; i < 100; i++);
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	uint8_t Key_Num;
	BaseType_t xHigherPriorityTaskWoken;
	xHigherPriorityTaskWoken = pdFALSE;
	if(GPIO_Pin==GPIO_PIN_3)
	{
		Key_Num = CH455_Key_Read();
		xQueueSendFromISR(Key_Value_ISR_Queue,&Key_Num,&xHigherPriorityTaskWoken);		
		
	}
}

static void SET_GPIO_MODE_INPUT(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
}

static void SET_GPIO_MODE_OUTPUT(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
}



void IIC_Start(void)
{
	SET_GPIO_MODE_OUTPUT();     //sda线输出
	IIC_SDA_1;	  	  
	IIC_SCL_1;
	delay_us();
 	IIC_SDA_0;//START:when CLK is high,DATA change form high to low 
	delay_us();
	IIC_SCL_0;//钳住I2C总线，准备发送或接收数据 
}	  
//产生IIC停止信号
void IIC_Stop(void)
{
	SET_GPIO_MODE_OUTPUT();//sda线输出
	IIC_SCL_0;
	IIC_SDA_0;//STOP:when CLK is high DATA change form low to high
 	delay_us();
	IIC_SCL_1; 
	delay_us();			
	IIC_SDA_1;//发送I2C总线结束信号				   	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
uint8_t IIC_Wait_Ack(void)
{
	uint8_t ucErrTime=0;
	SET_GPIO_MODE_INPUT();      //SDA设置为输入  
	IIC_SDA_1;delay_us();	   
	IIC_SCL_1;delay_us();	 
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL_0;//时钟输出0 	   
	return 0;  
} 
//产生ACK应答
void IIC_Ack(void)
{
	IIC_SCL_0;
	SET_GPIO_MODE_OUTPUT();
	IIC_SDA_0;
	delay_us();
	IIC_SCL_1;
	delay_us();
	IIC_SCL_0;
}
//不产生ACK应答		    
void IIC_NAck(void)
{
	IIC_SCL_0;
	SET_GPIO_MODE_OUTPUT();
	IIC_SDA_1;
	delay_us();
	IIC_SCL_1;
	delay_us();
	IIC_SCL_0;
}					 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void IIC_Send_Byte(uint8_t txd)
{                        
    uint8_t t;   
	SET_GPIO_MODE_OUTPUT(); 	    
    IIC_SCL_0;//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {         
				if((txd&0x80)>>7 == 1)
				{
					IIC_SDA_1;
				}
				else
				{
					IIC_SDA_0;
				}
        txd<<=1; 	  
				IIC_SCL_1;
				delay_us(); 
				IIC_SCL_0;	
				delay_us();
    }	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
uint8_t IIC_Read_Byte(uint8_t ack)
{
	uint8_t i,receive=0;
	SET_GPIO_MODE_INPUT();//SDA设置为输入
  for(i=0;i<8;i++ )
	{
        IIC_SCL_0; 
        delay_us();
		IIC_SCL_1;
        receive<<=1;
        if(READ_SDA)receive++;   
		delay_us(); 
    }					 
    if (!ack)
        IIC_NAck();//发送nACK
    else
        IIC_Ack(); //发送ACK   
    return receive;
}

uint8_t CH455_Key_Read(void)		// 向CH450发出按键读操作命令
{
		uint8_t keycode;
		uint8_t ch455_key_num=0;
   	IIC_Start();               
	IIC_Send_Byte(((uint8_t)(CH450_GET_KEY>>7)&CH450_I2C_MASK)|0x01|CH450_I2C_ADDR1);
		IIC_Wait_Ack();
    	keycode=IIC_Read_Byte(0);  
		IIC_Stop();        

		//将码值转换为key1-key16
//		switch(keycode)
//		{
//			case 0x44:
//				ch455_key_num = 0;   //对应按键的编号key1
//				break;			
//			case 0x45:
//				ch455_key_num = 1;
//				break;
//	       //省略部分代码
//		}
		return(keycode);
}

void CH450_Write(unsigned short cmd)	//写命令
{

	IIC_Start();               /*启动总线*/
   	IIC_Send_Byte(((unsigned char)(cmd>>7)&CH450_I2C_MASK)|CH450_I2C_ADDR1);
	Backvlaue1 = IIC_Wait_Ack();
   	IIC_Send_Byte((unsigned char)cmd);               /*发送数据*/
	Backvlaue2 = IIC_Wait_Ack();
  	IIC_Stop();                 /*结束总线*/ 
}

//数码管显示
void CH455_Display(uint8_t digital,uint8_t num)		
{
	if(digital == 1){
		CH450_Write(CH450_DIG4 | BCD_decode_tab[num]);  //第1位数码管显示
	}
	else if(digital == 2){
		CH450_Write(CH450_DIG5 | BCD_decode_tab[num]);  //第2位数码管显示
	}
	else if(digital == 3){
		CH450_Write(CH450_DIG6 | BCD_decode_tab[num]);  //第3位数码管显示
	}
	else if(digital == 4){
		CH450_Write(CH450_DIG7 | BCD_decode_tab[num]);  //第4位数码管显示
	}
}
void CH455_init(void)
{
//unsigned char i;for ( i = 0; i < 6; i ++ ) 
//	CH450_buf_index( i, 0 );  // 因为CH450复位时不清空显示内容，所以刚开电后必须人为清空，再开显示

	// 因为CH450复位时不清空显示内容，所以刚开电后必须人为清空，再开显示
	CH450_Write(0x0403);	// 开启显示及键盘
	LED_Management_Fanction(LED_NORMOL);
//	CH450_Write(CH450_DIG6 |0xff);  


}
void	CH450_buf_write( unsigned short cmd )  // 向CH450输出数据或者操作命令,自动建立数据映象
{
	if ( cmd & 0x1000 ) {   // 加载数据的命令,需要备份数据到映象缓冲区
		CH450_buf[ (unsigned char)( ( cmd >> 8 ) - 2 ) & 0x07 ] = (unsigned char)( cmd & 0xFF );	// 备份数据到相应的映象单元
	}
	CH450_Write( cmd );	// 发出
}

void	CH450_buf_index( unsigned char index, unsigned char dat )  // 向CH450指定的数码管输出数据,自动建立数据映象
// index 为数码管序号,有效值为0到5,分别对应DIG2到DIG7
{
	unsigned short cmd;
	CH450_buf[ index ] = dat;	// 备份数据到相应的映象单元
	cmd = ( CH450_DIG2 + ( (unsigned short)index << 8 ) ) | dat ;	// 生成操作命令
	CH450_Write( cmd );	// 发出
}

void CH450_set_bit(unsigned char bit_addr)     //段位点亮
{
	unsigned char byte_addr;
	byte_addr=(bit_addr&0x38)>>3;
	if ( byte_addr < 6 ) CH450_buf_index( byte_addr, CH450_buf[byte_addr] | (1<<(bit_addr&0x07)) );
}

void CH450_clr_bit(unsigned char bit_addr)     //段位熄灭
{
	unsigned char byte_addr;
	byte_addr=(bit_addr&0x38)>>3;
	if ( byte_addr < 6 ) CH450_buf_index( byte_addr, CH450_buf[byte_addr] & ~(1<<(bit_addr&0x07)) );
}

void LED_Management_Fanction(uint8_t LED_Light)
{

	switch(LED_Light)
	{
		case LED_SINGAL:

			LED_7vlaue =  LED22_VALUE7|LED27_VALUE7|LED28_VALUE7|LED20_VALUE7|LED24_VALUE7|LED26_VALUE7;
			LED_6vlaue =  LED17_VALUE6|LED18_VALUE6|LED21_VALUE6|LED30_VALUE6|LED31_VALUE6|LED32_VALUE6|NORMAL_VALUE6;
			LED_5vlaue =  LED1_VALUE5|LED2_VALUE5|LED5_VALUE5|LED6_VALUE5|LED9_VALUE5|LED10_VALUE5|LED13_VALUE5|LED14_VALUE5;
			LED_4vlaue =  LED11_VALUE4;
		break;


		case LED_GROUPRIGHT:

			LED_7vlaue =  0x00;
			LED_6vlaue = NORMAL_VALUE6;
			LED_5vlaue =  0x00;
			LED_4vlaue =  LED3_VALUE4|LED4_VALUE4|LED7_VALUE4;		
		break;
	


		case LED_GROUPLEFT:
			LED_7vlaue =  0x00;
			LED_6vlaue = NORMAL_VALUE6;
			LED_5vlaue =  0x00;
			LED_4vlaue =  LED3_VALUE4|LED4_VALUE4|LED8_VALUE4;	
			
		break;
	
		case LED_SETTING_NUM:
			LED_7vlaue =  0x00;
			LED_6vlaue = NORMAL_VALUE6;
			LED_5vlaue =  LED1_VALUE5|LED2_VALUE5|LED5_VALUE5|LED6_VALUE5|LED9_VALUE5|LED10_VALUE5|LED13_VALUE5|LED14_VALUE5;
			LED_4vlaue =  LED15_VALUE4|LED11_VALUE4;	
		break;
		
		case LED_NORMOL:
			LED_7vlaue =  0x00;
			LED_6vlaue =  NORMAL_VALUE6;
			LED_5vlaue =  0x00;
			LED_4vlaue =  0x00;
		break;
		
		case LED_BLINK:

			LED_6vlaue =  LED_6vlaue|BLINK_VALUE6;

		break;
		
		case LED_SETING:
			LED_7vlaue =  LED19_VALUE7|LED20_VALUE7|LED23_VALUE7|LED24_VALUE7;
			LED_6vlaue =  NORMAL_VALUE6;
			LED_5vlaue =  0x00;
			LED_4vlaue =  0x00;
		break;
	}	
	
	if(LED_Light == LED_BLINK)
	{

		CH450_buf_write(CH450_DIG6 | LED_6vlaue);
		vTaskDelay(20);
		LED_6vlaue = LED_6vlaue&(0xfB);
		CH450_buf_write(CH450_DIG6 | LED_6vlaue);

	}	
	else if(LED_Light != LED_BLINK)
	{
		CH450_buf_write(CH450_DIG7 | LED_7vlaue);
		CH450_buf_write(CH450_DIG6 | LED_6vlaue);
		CH450_buf_write(CH450_DIG5 | LED_5vlaue);
		CH450_buf_write(CH450_DIG4 | LED_4vlaue);
	}

}

/*
LED_Management_Fanction(LED_BLINK);
LED_Management_Fanction(LED_NORMOL);
LED_Management_Fanction(LED_SETTING_NUM);
LED_Management_Fanction(LED_GROUPLEFT);
LED_Management_Fanction(LED_GROUPRIGHT);
LED_Management_Fanction(LED_SINGAL);
LED_Management_Fanction(LED_SETING);

*/






