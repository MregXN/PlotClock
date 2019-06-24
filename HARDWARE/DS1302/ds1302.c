#include "ds1302.h"
#include "sys.h"

#define RST PAout(5)
#define IO  PAout(6)
#define SCK PAout(7)

#define ds1302_sec_add 0x80 
#define ds1302_min_add 0x82 
#define ds1302_hr_add 0x84 
#define ds1302_date_add 0x86 
#define ds1302_month_add 0x88 
#define ds1302_day_add 0x8a 
#define ds1302_year_add 0x8c 
#define ds1302_control_add 0x8e 
#define ds1302_charger_add 0x90  
#define ds1302_clkburst_add 0xbe

uchar time_buf[8] = {0x20,0x19,0x06,0x24,0x9,0x34,0};
uchar readtime[14];

void DS1302_GPIOInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA, GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);
}

void DS1302_OUT(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}


void DS1302_IN(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}


void Delay_xms(uint x)
{
	uint i,j;
	for(i=0;i<56;i++)
		for(j=0;j<112;j++);
}


void ds1302_init(void)
{
	RST=0; 
	SCK=0; 
}


void ds1302_write_byte(uchar addr, uchar d)
{
	uchar i;
	DS1302_OUT();
	RST=1; 

	addr = addr & 0xFE;   
	for (i = 0; i < 8; i ++)
	{
		if (addr & 0x01) 
		{
			IO=1;
		}
		else 
		{
			IO=0;
		}
		SCK=1;      
		SCK=0;
		addr = addr >> 1;
	}

	for (i = 0; i < 8; i ++) 
	{
		if (d & 0x01) 
		{
			IO=1;
		}
		else 
		{
			IO=0;
		}
		SCK=1;    
		SCK=0;
		d = d >> 1;
	}
		RST=0; 
}


uchar ds1302_read_byte(uchar addr) 
{
	uchar i,temp;
	DS1302_OUT();
	RST=1; 
	
	addr = addr | 0x01;    
	for (i = 0; i < 8; i ++) 
	{
		if (addr & 0x01) 
		{
			IO=1;
		}
		else 
		{
			IO=0;
		}
		SCK=1;
		SCK=0;
		addr = addr >> 1;
	}

	DS1302_IN();
	for (i = 0; i < 8; i ++) 
	{
		temp = temp >> 1;
		if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6)) 
		{
			temp |= 0x80;
		}
		else 
		{
			temp &= 0x7F;
		}
		SCK=1;
		SCK=0;
	}
	RST=0; 
	return temp;
}


void ds1302_write_time(void)
{
	ds1302_write_byte(ds1302_control_add,0x00); 
	ds1302_write_byte(ds1302_sec_add,0x80); 
	//ds1302_write_byte(ds1302_charger_add,0xa9);    
	ds1302_write_byte(ds1302_year_add,time_buf[1]); 
	ds1302_write_byte(ds1302_month_add,time_buf[2]); 
	ds1302_write_byte(ds1302_date_add,time_buf[3]); 
	ds1302_write_byte(ds1302_hr_add,time_buf[4]); 
	ds1302_write_byte(ds1302_min_add,time_buf[5]); 
	ds1302_write_byte(ds1302_sec_add,time_buf[6]); 
	ds1302_write_byte(ds1302_day_add,time_buf[7]); 
	ds1302_write_byte(ds1302_control_add,0x80);     
}


void ds1302_read_time(void)  
{
	time_buf[1]=ds1302_read_byte(ds1302_year_add); 
	time_buf[2]=ds1302_read_byte(ds1302_month_add); 
	time_buf[3]=ds1302_read_byte(ds1302_date_add); 
	time_buf[4]=ds1302_read_byte(ds1302_hr_add); 
	time_buf[5]=ds1302_read_byte(ds1302_min_add); 
	time_buf[6]=(ds1302_read_byte(ds1302_sec_add))&0x7f;
	time_buf[7]=ds1302_read_byte(ds1302_day_add); 
	
	readtime[0]=(time_buf[0]>>4);   
	readtime[1]=(time_buf[0]&0x0F); 
	readtime[2]=(time_buf[1]>>4);   
	readtime[3]=(time_buf[1]&0x0F); 

	readtime[4]=(time_buf[2]>>4);   
	readtime[5]=(time_buf[2]&0x0F); 

	readtime[6]=(time_buf[3]>>4);   
	readtime[7]=(time_buf[3]&0x0F); 

	readtime[8]=(time_buf[4]>>4);   
	readtime[9]=(time_buf[4]&0x0F); 
	readtime[10]=(time_buf[5]>>4);   
	readtime[11]=(time_buf[5]&0x0F); 
	readtime[12]=(time_buf[6]>>4);   
	readtime[13]=(time_buf[6]&0x0F);   
}
