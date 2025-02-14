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

//DS1302��ʼ������
void ds1302_init(void) 
{
	DS1302_RST_L;			//RST���õ�
	DS1302_I2C_CLK_H;			//SCK���õ�
}

//��DS1302д��һ�ֽ�����
void ds1302_write_byte(uint8_t addr, uint8_t d) 
{
	uint8_t i;
	DS1302_RST_H;					//����DS1302����	
	//д��Ŀ���ַ��addr
	addr = addr & 0xFE;   //���λ���㣬�Ĵ���0λΪ0ʱд��Ϊ1ʱ��
	for (i = 0; i < 8; i ++) {
		if (addr & 0x01) {
			DS1302_I2C_DAT_H;
			}
		else {
			DS1302_I2C_DAT_L;
			}
		DS1302_I2C_CLK_H;      //����ʱ��
		DS1302_I2C_CLK_L;
		addr = addr >> 1;
		}	
	//д�����ݣ�d
	for (i = 0; i < 8; i ++) {
		if (d & 0x01) {
			DS1302_I2C_DAT_H;
			}
		else {
			DS1302_I2C_DAT_L;
			}
		DS1302_I2C_CLK_H;    //����ʱ��
		DS1302_I2C_CLK_L;
		d = d >> 1;
		}
	DS1302_RST_L;		//ֹͣDS1302����
}

//��DS1302����һ�ֽ�����
uint8_t ds1302_read_byte(uint8_t addr) {

	uint8_t i,temp;	
	DS1302_RST_H;					//����DS1302����
	//д��Ŀ���ַ��addr
	addr = addr | 0x01;    //���λ�øߣ��Ĵ���0λΪ0ʱд��Ϊ1ʱ��
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
	//������ݣ�temp
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
	DS1302_RST_L;					//ֹͣDS1302����
	return temp;
}

//��DS302д��ʱ������
void DS1302_Write_Time(uint8_t *time_buf) 
{
	ds1302_write_byte(ds1302_control_add,0x00);			//�ر�д���� 
	ds1302_write_byte(ds1302_sec_write,0x80);				//��ͣʱ�� 
	//ds1302_write_byte(ds1302_charger_add,0xa9);	    //������ 
	ds1302_write_byte(ds1302_year_write,time_buf[1]);		//�� 
	ds1302_write_byte(ds1302_month_write,time_buf[2]);	//�� 
	ds1302_write_byte(ds1302_date_write,time_buf[3]);		//�� 
	ds1302_write_byte(ds1302_hr_write,time_buf[4]);		//ʱ 
	ds1302_write_byte(ds1302_min_write,time_buf[5]);		//��
	ds1302_write_byte(ds1302_sec_write,time_buf[6]);		//��
	ds1302_write_byte(ds1302_day_write,time_buf[7]);		//�� 
	ds1302_write_byte(ds1302_control_add,0x80);			//��д����     
}

//��DS302����ʱ������
void DS1302_Read_Time(uint8_t *time_buf)
{
	time_buf[1]=ds1302_read_byte(ds1302_year_read	);		//�� 
	time_buf[2]=ds1302_read_byte(ds1302_month_read	);		//�� 
	time_buf[3]=ds1302_read_byte(ds1302_date_read	);		//�� 
	time_buf[4]=ds1302_read_byte(ds1302_hr_read	);		//ʱ 
	time_buf[5]=ds1302_read_byte(ds1302_min_read	);		//�� 
	time_buf[6]=(ds1302_read_byte(ds1302_sec_read	))&0x7f;//�룬������ĵ�7λ�����ⳬ��59
	time_buf[7]=ds1302_read_byte(ds1302_day_read	);		//�� 	
}
