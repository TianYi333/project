#include "bsp_ds1302.h"

static void SET_GPIO_MODE_INPUT(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = DS1302_I2C_DAT;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(DS1302_DAT_PORT, &GPIO_InitStruct);
}

static void SET_GPIO_MODE_OUTPUT(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = DS1302_I2C_DAT;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(DS1302_DAT_PORT, &GPIO_InitStruct);
}

uint8_t DS1302_Read(void)
{
	uint8_t status;
	
	SET_GPIO_MODE_INPUT();
	
	status = HAL_GPIO_ReadPin(DS1302_DAT_PORT,DS1302_I2C_DAT);
	
	SET_GPIO_MODE_OUTPUT();
	
	return status;
}

//DS1302初始化函数
void ds1302_init(void) 
{
	DS1302_RST_L;			//RST脚置低
	DS1302_I2C_CLK_H;			//SCK脚置低
}

//向DS1302写入一字节数据
void ds1302_write_byte(uint8_t addr, uint8_t d) 
{
	uint8_t i;
	DS1302_RST_H;					//启动DS1302总线	
	//写入目标地址：addr
	addr = addr & 0xFE;   //最低位置零，寄存器0位为0时写，为1时读
	for (i = 0; i < 8; i ++) {
		if (addr & 0x01) {
			DS1302_I2C_DAT_H;
			}
		else {
			DS1302_I2C_DAT_L;
			}
		DS1302_I2C_CLK_H;      //产生时钟
		DS1302_I2C_CLK_L;
		addr = addr >> 1;
		}	
	//写入数据：d
	for (i = 0; i < 8; i ++) {
		if (d & 0x01) {
			DS1302_I2C_DAT_H;
			}
		else {
			DS1302_I2C_DAT_L;
			}
		DS1302_I2C_CLK_H;    //产生时钟
		DS1302_I2C_CLK_L;
		d = d >> 1;
		}
	DS1302_RST_L;		//停止DS1302总线
}

//从DS1302读出一字节数据
uint8_t ds1302_read_byte(uint8_t addr) {

	uint8_t i,temp;	
	DS1302_RST_H;					//启动DS1302总线
	//写入目标地址：addr
	addr = addr | 0x01;    //最低位置高，寄存器0位为0时写，为1时读
	for (i = 0; i < 8; i ++) {
		if (addr & 0x01) {
			DS1302_I2C_DAT_H;
			}
		else {
			DS1302_I2C_DAT_L;
			}
		DS1302_I2C_CLK_H;
		DS1302_I2C_CLK_L;
		addr = addr >> 1;
		}	
	//输出数据：temp
	for (i = 0; i < 8; i ++) {
		temp = temp >> 1;
		if (DS1302_Read()) {
			temp |= 0x80;
			}
		else {
			temp &= 0x7F;
			}
		DS1302_I2C_CLK_H;
		DS1302_I2C_CLK_L;
		}	
	DS1302_RST_L;					//停止DS1302总线
	return temp;
}

//向DS302写入时钟数据
void DS1302_Write_Time(uint8_t *time_buf) 
{
	ds1302_write_byte(ds1302_control_add,0x00);			//关闭写保护 
	ds1302_write_byte(ds1302_sec_write,0x80);				//暂停时钟 
	//ds1302_write_byte(ds1302_charger_add,0xa9);	    //涓流充电 
	ds1302_write_byte(ds1302_year_write,time_buf[1]);		//年 
	ds1302_write_byte(ds1302_month_write,time_buf[2]);	//月 
	ds1302_write_byte(ds1302_date_write,time_buf[3]);		//日 
	ds1302_write_byte(ds1302_hr_write,time_buf[4]);		//时 
	ds1302_write_byte(ds1302_min_write,time_buf[5]);		//分
	ds1302_write_byte(ds1302_sec_write,time_buf[6]);		//秒
	ds1302_write_byte(ds1302_day_write,time_buf[7]);		//周 
	ds1302_write_byte(ds1302_control_add,0x80);			//打开写保护     
}

//从DS302读出时钟数据
void DS1302_Read_Time(uint8_t *time_buf)
{
	time_buf[1]=ds1302_read_byte(ds1302_year_read	);		//年 
	time_buf[2]=ds1302_read_byte(ds1302_month_read	);		//月 
	time_buf[3]=ds1302_read_byte(ds1302_date_read	);		//日 
	time_buf[4]=ds1302_read_byte(ds1302_hr_read	);		//时 
	time_buf[5]=ds1302_read_byte(ds1302_min_read	);		//分 
	time_buf[6]=(ds1302_read_byte(ds1302_sec_read	))&0x7f;//秒，屏蔽秒的第7位，避免超出59
	time_buf[7]=ds1302_read_byte(ds1302_day_read	);		//周 	
}
