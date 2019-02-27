#include<reg52.h> //包含头文件，一般情况不需要改动，头文件包含特殊功能寄存器的定义
#include<intrins.h>
#define uint unsigned int
#define uchar unsigned char
sbit RS = P1^0;   //定义端口
sbit RW = P1^1;
sbit EN = P2^5;
sbit DU = P2^6;
sbit WE = P2^7;
sbit  DS1302_CLK = P2^1;              //实时时钟时钟线引脚
sbit  DS1302_IO  = P2^0;              //实时时钟数据线引脚
sbit  DS1302_RST = P2^4;              //实时时钟复位线引脚
sbit  ACC0 = ACC^0;
sbit  ACC7 = ACC^7;

typedef struct __SYSTEMTIME__
{
	unsigned char Second;
	unsigned char Minute;
	unsigned char Hour;
	unsigned char Week;
	unsigned char Day;
	unsigned char Month;
	unsigned char Year;
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

uint x,y;
uchar num,num_m;

void cmg88()//关数码管
{
		DU = 1;
		P0 = 0X00;
		DU = 0;
		WE = 0;
}
/*------------------------------------------------
 uS延时函数，含有输入参数 unsigned char t，无返回值
 unsigned char 是定义无符号字符变量，其值的范围是
 0~255 这里使用晶振12M，精确延时请使用汇编,大致延时
 长度如下 T=tx2+5 uS
------------------------------------------------*/
void DelayUs2x(unsigned char t)
{
 	while(--t);
}
/*------------------------------------------------
 mS延时函数，含有输入参数 unsigned char t，无返回值
 unsigned char 是定义无符号字符变量，其值的范围是
 0~255 这里使用晶振12M，精确延时请使用汇编
------------------------------------------------*/
void DelayMs(unsigned char t)
{

 while(t--)
 {
     //大致延时1mS
     DelayUs2x(245);
	 DelayUs2x(245);
 }
}

 bit LCD_Check_Busy(void)
 {
	 P0 = 0xff;
	 RS = 0;
	 RW = 1;
	 EN = 0;
	 _nop_();
	 EN = 1;
	 return (bit)(P0 & 0x80);	  //强制取最高位
 }

 void LCD_Write_Com(unsigned char com)
 {
	 while(LCD_Check_Busy()); //忙则等待
	 RS=0;
	 RW=0;
	 EN=1;
	 P0 = com;
	 _nop_();
	 EN=0;
 }

 void LCD_Write_Data(unsigned char Data)
 {
	 while(LCD_Check_Busy()); //忙则等待
	 RS=1;
	 RW=0;
	 EN=1;
	 P0 = Data;
	 _nop_();
	 EN=0;
 }

 void LCD_Clear(void)
 {
	 LCD_Write_Com(0x01);
	 DelayMs(5);
 }

 void LCD_Write_String(unsigned char x,unsigned char y,unsigned char *s)
 {
	 if (y == 0)
	 {
		 	LCD_Write_Com(0x80 + x);     //表示第一行
	 }
	 else
	 {
	 		LCD_Write_Com(0xC0 + x);      //表示第二行
	 }
	 while (*s)
	 {
			LCD_Write_Data( *s);
			s ++;
	 }
 }

 void LCD_Write_Char(unsigned char x,unsigned char y,unsigned char Data)
 {
	 if (y == 0)
	 {
	 	LCD_Write_Com(0x80 + x);
	 }
	 else
	 {
	 	LCD_Write_Com(0xC0 + x);
	 }
	 LCD_Write_Data( Data);
 }

 void LCD_Init(void)		//静态初始化
 {
   LCD_Write_Com(0x38);    /*显示模式设置*/
   DelayMs(5);
   LCD_Write_Com(0x38);
   DelayMs(5);
   LCD_Write_Com(0x38);
   DelayMs(5);
   LCD_Write_Com(0x38);
   LCD_Write_Com(0x08);    /*显示关闭*/
   LCD_Write_Com(0x01);    /*显示清屏*/
   LCD_Write_Com(0x06);    /*显示光标移动设置*/
   DelayMs(5);
   LCD_Write_Com(0x0C);    /*显示开及光标设置*/
   //LCD_Write_Com(0x0f);
 }

void delay(uint z)
{
	uint x,y;
	for(x=z;z>0;z--)
		for(y=112;y>0;y--);
}

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
		Write1302(0x8E,0x80);   //应该是0x80才对
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
	Time->TimeString[8] = '\0';
}

void Initial_DS1302(void)
{
	unsigned char Second=Read1302(DS1302_SECOND);
	if(Second&0x80)
		DS1302_SetTime(DS1302_SECOND,0);
}

void Week(SYSTEMTIME *Time)	
{
	switch(Time->Week)
	{								
		case 1:LCD_Write_Char(12,0,'M');//delay(5);
		       delay(5);
			   LCD_Write_Char(13,0,'0');//delay(5);
			   delay(5);
			   LCD_Write_Char(14,0,'N');
			   delay(5);
			   break;

		case 2:LCD_Write_Char(12,0,'T');//delay(5);
		       delay(5);
			   LCD_Write_Char(13,0,'U');//delay(5);
			   delay(5);
			   LCD_Write_Char(14,0,'E');
			   delay(5);
			   break;

		case 3:LCD_Write_Char(12,0,'W');//delay(5);
		       delay(5);
			   LCD_Write_Char(13,0,'E');//delay(5);
			   delay(5);
			   LCD_Write_Char(14,0,'D');
			   delay(5);
			   break;

		case 4:LCD_Write_Char(12,0,'T');//delay(5);
		       delay(5);
			   LCD_Write_Char(13,0,'H');//delay(5);
			   delay(5);
			   LCD_Write_Char(14,0,'U');
			   delay(5);
			   break;

		case 5:LCD_Write_Char(12,0,'F');//delay(5);
		       delay(5);
			   LCD_Write_Char(13,0,'R');//delay(5);
			   delay(5);
			   LCD_Write_Char(14,0,'I');
			   delay(5);
			   break;

		case 6:LCD_Write_Char(12,0,'S');//delay(5);
		       delay(5);
			   LCD_Write_Char(13,0,'T');//delay(5);
			   delay(5);
			   LCD_Write_Char(14,0,'A');
			   delay(5);
			   break;

		case 7:LCD_Write_Char(12,0,'S');//delay(5);
		       delay(5);
			   LCD_Write_Char(13,0,'U');//delay(5);
			   delay(5);
			   LCD_Write_Char(14,0,'N');
			   delay(5);
			   break;
	}
}

void display(SYSTEMTIME *Time)
{
   LCD_Write_Char(1,0,'2');
   LCD_Write_Char(2,0,'0');
   LCD_Write_Char(3,0,(Time->DateString[0])+'0');   
   LCD_Write_Char(4,0,(Time->DateString[1])+'0');
   LCD_Write_Char(5,0,'-');
   LCD_Write_Char(6,0,(Time->DateString[3])+'0');
   LCD_Write_Char(7,0,(Time->DateString[4])+'0');   
   LCD_Write_Char(8,0,'-');
   LCD_Write_Char(9,0,(Time->DateString[6])+'0');
   LCD_Write_Char(10,0,(Time->DateString[7])+'0'); 
   Week(Time);
   LCD_Write_Char(4,1,(Time->TimeString[0])+'0');   
   LCD_Write_Char(5,1,(Time->TimeString[1])+'0');
   LCD_Write_Char(6,1,':');
   LCD_Write_Char(7,1,(Time->TimeString[3])+'0');
   LCD_Write_Char(8,1,(Time->TimeString[4])+'0');   
   LCD_Write_Char(9,1,':');
   LCD_Write_Char(10,1,(Time->TimeString[6])+'0');
   LCD_Write_Char(11,1,(Time->TimeString[7])+'0');
}

void main()
{
    SYSTEMTIME CurrentTime;
	Initial_DS1302();
	cmg88();//关数码管，点阵函数
	LCD_Init();   //静态初始化
	LCD_Clear();//清屏
	DS1302_SetTime(DS1302_HOUR,23);	//设置的时间是10：54：30
	DS1302_SetTime(DS1302_MINUTE,59);
	DS1302_SetTime(DS1302_SECOND,56);
	DS1302_SetTime(DS1302_WEEK,7);
	DS1302_SetTime(DS1302_DAY,13);
	DS1302_SetTime(DS1302_MONTH,8);
	DS1302_SetTime(DS1302_YEAR,17); 
	while(1)
	{
	    DS1302_GetTime(&CurrentTime);
		DateToStr(&CurrentTime);
		TimeToStr(&CurrentTime);
		display(&CurrentTime);
	}
}

