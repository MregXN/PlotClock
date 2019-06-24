#ifndef _DS1302_H
#define _DS1302_H


#define uchar unsigned char
#define uint  unsigned int

void DS1302_GPIOInit(void);
void ds1302_init(void);
void ds1302_write_time(void);
void ds1302_read_time(void);
void Delay_xms(uint x);

extern uchar time_buf[8];
extern uchar readtime[14];

#endif
