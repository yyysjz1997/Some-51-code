/************************************************************
 DS1802s是用RAM存储的,掉电后数据肯定会丢失，除非外加纽扣电池
***********************************************************/ 

#include <reg52.h>
#define uchar unsigned char
#define uint unsigned int
unsigned char code table[]={
0x3f  , 0x06 , 0x5b , 0x4f ,
0x66 , 0x6d ,0x7d , 0x07 , 
0x7f  , 0x6f , 0x77 , 0x7c ,
0x39 , 0x5e , 0x79 , 0x71 ,0xbf};

sbit  DS1302_CLK = P2^1;              //实时时钟时钟线引脚
sbit  DS1302_IO  = P2^0;              //实时时钟数据线引脚
sbit  DS1302_RST = P2^4;              //实时时钟复位线引脚
sbit  ACC0 = ACC^0;
sbit  ACC7 = ACC^7;
sbit we=P2^7;
sbit du=P2^6;

typedef struct __SYSTEMTIME__
{
	unsigned char Second;
	unsigned char Minute;
	unsigned char Hour;
	unsigned char Week;
	unsigned char Day;
	unsigned char Month;
	unsigned char  Year;
	unsigned char DateString[9];
	unsigned char TimeString[9];
}SYSTEMTIME;	//定义的时间类型

#define AM(X)	X
#define PM(X)	(X+12)            	  // 转成24小时制
#define DS1302_SECOND	0x80     //写寄存器
#define DS1302_MINUTE	0x82
#define DS1302_HOUR		0x84
#define DS1302_WEEK		0x8A
#define DS1302_DAY		0x86
#define DS1302_MONTH	0x88
#define DS1302_YEAR		0x8C
#define DS1302_RAM(X)	(0xC0+(X)*2)   	//用于计算 DS1302_RAM 地址的宏

void DS1302InputByte(unsigned char d) 	//实时时钟写入一字节(内部函数)
{
    unsigned char i;
    ACC = d;
    for(i=8; i>0; i--)
    {
        DS1302_IO = ACC0;           	//相当于汇编中的 RRC
        DS1302_CLK = 1;
        DS1302_CLK = 0;
        ACC = ACC >> 1;
    }
}

unsigned char DS1302OutputByte(void) 	//实时时钟读取一字节(内部函数)
{
    unsigned char i;
    for(i=8; i>0; i--)
    {
        ACC = ACC >>1;         			//相当于汇编中的 RRC
        ACC7 = DS1302_IO;
        DS1302_CLK = 1;
        DS1302_CLK = 0;
    }
    return(ACC);
}

void Write1302(unsigned char ucAddr, unsigned char ucDa)	//ucAddr: DS1302地址, ucData: 要写的数据
{
    DS1302_RST = 0;
    DS1302_CLK = 0;
    DS1302_RST = 1;
    DS1302InputByte(ucAddr);       	// 地址，命令
    DS1302InputByte(ucDa);       	// 写1Byte数据
//    DS1302_CLK = 1;
    DS1302_RST = 0;
}

unsigned char Read1302(unsigned char ucAddr)	//读取DS1302某地址的数据
{
    unsigned char ucData;
    DS1302_RST = 0;
    DS1302_CLK = 0;
    DS1302_RST = 1;
    DS1302InputByte(ucAddr|0x01);        // 地址，命令
    ucData = DS1302OutputByte();         // 读1Byte数据
//    DS1302_CLK = 1;
    DS1302_RST = 0;
    return(ucData);
}

void DS1302_SetProtect(bit flag)        //是否写保护
{
	if(flag)
		//Write1302(0x8E,0x10);   //应该是0x80才对
		Write1302(0x8E,0x80);  
	else
		Write1302(0x8E,0x00);
}

void DS1302_SetTime(unsigned char Address, unsigned char Value)        // 设置时间函数
{
	DS1302_SetProtect(0);
	Write1302(Address, ((Value/10)<<4 | (Value%10)));  //生成BCD码
}

void DS1302_GetTime(SYSTEMTIME *Time)
{
	unsigned char ReadValue;
	ReadValue = Read1302(DS1302_SECOND);
	Time->Second = ((ReadValue&0x70)>>4)*10 + (ReadValue&0x0F);	//八进制转换成十进制
	ReadValue = Read1302(DS1302_MINUTE);
	Time->Minute = ((ReadValue&0x70)>>4)*10 + (ReadValue&0x0F);
	ReadValue = Read1302(DS1302_HOUR);
	Time->Hour = ((ReadValue&0x70)>>4)*10 + (ReadValue&0x0F);
	ReadValue = Read1302(DS1302_DAY);
	Time->Day = ((ReadValue&0x70)>>4)*10 + (ReadValue&0x0F);
	ReadValue = Read1302(DS1302_WEEK);
	Time->Week = ((ReadValue&0x70)>>4)*10 + (ReadValue&0x0F);
	ReadValue = Read1302(DS1302_MONTH);
	Time->Month = ((ReadValue&0x70)>>4)*10 + (ReadValue&0x0F);
	ReadValue = Read1302(DS1302_YEAR);
	Time->Year = ((ReadValue&0x70)>>4)*10 + (ReadValue&0x0F);
}

void DateToStr(SYSTEMTIME *Time)	 //转化为字符串
{
	Time->DateString[0] = Time->Year/10 ;	 //分离出个位和十位
	Time->DateString[1] = Time->Year%10 ;
	Time->DateString[2] = '-';
	Time->DateString[3] = Time->Month/10;
	Time->DateString[4] = Time->Month%10 ;
	Time->DateString[5] = '-';
	Time->DateString[6] = Time->Day/10 ;
	Time->DateString[7] = Time->Day%10 ;
	Time->DateString[8] = '\0';
}

void TimeToStr(SYSTEMTIME *Time)
{
	Time->TimeString[0] = Time->Hour/10 ;
	Time->TimeString[1] = Time->Hour%10 ;
	Time->TimeString[2] = ':';
	Time->TimeString[3] = Time->Minute/10 ;
	Time->TimeString[4] = Time->Minute%10 ;
	Time->TimeString[5] = ':';
	Time->TimeString[6] = Time->Second/10;
	Time->TimeString[7] = Time->Second%10 ;
	Time->DateString[8] = '\0';
}

void Initial_DS1302(void)
{
	unsigned char Second=Read1302(DS1302_SECOND);
	if(Second&0x80)
		DS1302_SetTime(DS1302_SECOND,0);
}

void delay()   //这样显示效果最好
{
	int x,y;
	for(x=2;x>0;x--)
		for(y=160;y>0;y--);
}

void display(uchar sw,uchar wan,uchar qian,uchar bai,uchar shi,uchar ge)
{
	we=1;
	P0=0xfe;
	we=0;
	P0=0;
	du=1;
	P0=sw;
	du=0;
	delay();

	we=1;
	P0=0xfd;
	we=0;
	P0=0;
	du=1;
	P0=wan;
	du=0;
	delay();

	we=1;
	P0=0xf7;
	we=0;
	P0=0;
	du=1;
	P0=qian;
	du=0;
	delay();

	we=1;
	P0=0xef;
	we=0;
	P0=0;
	du=1;
	P0=bai;
	du=0;
	delay();

	we=1;
	P0=0xbf;
	we=0;
	P0=0;
	du=1;
	P0=shi;
	du=0;
	delay();

	we=1;
	P0=0x7f;
	we=0;
	P0=0;
	du=1;
	P0=ge;
	du=0;
	delay();
}

void main()
{
	uchar t0,t1,t2,t3,t4,t5;
	SYSTEMTIME CurrentTime;
	Initial_DS1302();
	DS1302_SetTime(DS1302_HOUR,10);	//设置的时间是10：54：30
	DS1302_SetTime(DS1302_MINUTE,54);
	DS1302_SetTime(DS1302_SECOND,30);   
	while(1)
	{
		DS1302_GetTime(&CurrentTime);
		TimeToStr(&CurrentTime);
		t0=CurrentTime.TimeString[0];
		t1=CurrentTime.TimeString[1];
		t2=CurrentTime.TimeString[3];
		t3=CurrentTime.TimeString[4];
		t4=CurrentTime.TimeString[6];
		t5=CurrentTime.TimeString[7];
		display(table[t0],table[t1],table[t2],table[t3],table[t4],table[t5]);
	}
}