#include<reg52.h> //包含头文件，一般情况不需要改动，头文件包含特殊功能寄存器的定义
#include<intrins.h>
#define uint unsigned int
#define uchar unsigned char
sbit RS = P1^0;   //定义端口
sbit RW = P1^1;
sbit EN = P2^5;
sbit DU = P2^6;
sbit WE = P2^7;
sbit S1 = P3^0;		 //秒钟停止+光标闪烁
sbit S2 = P3^1;
sbit S3 = P3^2;
sbit rd = P3^7;  //使用最右端的前三个键来控制
uint x,y;
uchar num,num_m;
uchar count,shi,fen,miao;
uchar S1num;

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
 	   cmg88();//关数码管，点阵函数 
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
   	   	LCD_Clear();//清屏
	   //	while (1)
	//	{

	 	 	LCD_Write_String(0,0,"   2017-08-11");
		 	LCD_Write_String(0,1,"    00:00:00");
	//	}
   TMOD = 0x01;
   TH0 = (65536 - 50000) / 256;
   TL0 = (65536 - 50000) % 256;
   EA = 1;
   ET0 = 1;
   TR0 = 1;
 }
 
void delay(uint z)
{
	uint x,y;
	for(x=z;z>0;z--)
		for(y=112;y>0;y--);
}

void write_sfm(uchar add,uchar date)
{
	uchar shi,ge;
	shi = date / 10;
	ge = date % 10;
	LCD_Write_Com(0x80+0x40+add);
	LCD_Write_Data(0x30+shi);
	LCD_Write_Data(0x30+ge);
}

void key()
{
	 rd = 0;
	 if(S1 == 0)
	 {
	 	delay(5);
		if(S1 == 0)
		{
		    S1num++;
		    while(!S1);		   //等松手
			if(S1num == 1)
			{
				TR0 = 0;  //定时器关闭，时间停止
				LCD_Write_Com(0x80+0x40+10);   //光标回到秒
				LCD_Write_Com(0x0f);   //光标闪烁
			}
			if(S1num == 2)
			{
				LCD_Write_Com(0x80+0x40+7);   //光标回到秒
				LCD_Write_Com(0x0f);   //光标闪烁
			}
			if(S1num == 3)
			{
				LCD_Write_Com(0x80+0x40+4);   //光标回到秒
				LCD_Write_Com(0x0f);   //光标闪烁
			}
			if(S1num == 4)
			{
			    S1num = 0;
				LCD_Write_Com(0x0c);   //光标不闪烁+不显示
				TR0 = 1;
			}				
		}
	 }
	 if(S1num != 0)
	{
		if(S2 == 0)
		{
			delay(5);
			if(S2 == 0)
			{
			    while(!S2);
				if(S1num == 1)	  //秒位上
				{
					miao++;
					if(miao == 60)
					{
						miao = 0;
					}
					write_sfm(10,miao);
					LCD_Write_Com(0x80+0x40+10);
				}
				if(S1num == 2)	   //分位上
				{
					fen++;
					if(fen == 60)
					{
						fen = 0;
					}
					write_sfm(7,fen);
					LCD_Write_Com(0x80+0x40+7);
				}
				if(S1num == 3)	   //十位上
				{
					shi++;
					if(shi == 24)
					{
						shi = 0;
					}
					write_sfm(4,shi);
					LCD_Write_Com(0x80+0x40+4);
				}	
			}
		}
		if(S3 == 0)
		{
			delay(5);
			if(S3 == 0)
			{
				while(!S3);
				if(S1num == 1)
				{					
					if(miao == 0)
					{
						miao = 60;
						write_sfm(10,miao);
						LCD_Write_Com(0x80+0x40+10);
					}
					miao--;	
					write_sfm(10,miao);
					LCD_Write_Com(0x80+0x40+10);
				}
				if(S1num == 2)
				{
					if(fen == 0)
					{
						fen = 60;
						write_sfm(7,fen);
						LCD_Write_Com(0x80+0x40+7);
					}
					fen--;	
					write_sfm(7,fen);
					LCD_Write_Com(0x80+0x40+7);
				}
				if(S1num == 3)
				{
					if(shi == 0)
					{
						shi = 24;
						write_sfm(4,shi);
						LCD_Write_Com(0x80+0x40+4);
					}
					shi--;	
					write_sfm(4,shi);
					LCD_Write_Com(0x80+0x40+4);
				}
			}
		}
	}
}

void main(void)
{
	LCD_Init();   //静态初始化
	while(1)
	{
		key();
	}	
}

void timer0()interrupt 1
{
	TH0 = (65536 - 50000) / 256;
    TL0 = (65536 - 50000) % 256;
	count++;
	if(count == 18)
		{
			count = 0;
			miao++;
			if(miao == 60)
			{
				miao = 0;
				fen++;
				if(fen == 60)
				{
					fen = 0;
					shi++;
					if(shi == 24)
					{
						shi = 0;
					}
					write_sfm(4,shi);
				}
				write_sfm(7,fen);
			}
			write_sfm(10,miao);			 
		}
}











/*#include <reg52.h>
#define uchar unsigned char
#define uint unsigned int
sbit DU = P2^6;
sbit WE = P2^7;
sbit RS = P1^0;
sbit EN = P2^5;
uchar code table[]="   2017-8-11";
uchar code table1[]="    00:00:00";

void delay(uchar z)
{
	uint x,y;
	for(x=z;x>0;x--)
		for(y=112;y>0;y--);
}

void write_com(uchar com)
{
	RS = 0;
	EN = 0;
	P0 = com;
	delay(5);
	EN = 1;
	delay(5);
	EN = 0;	
}

void write_date(uchar date)
{
	RS = 1;
	EN = 0;
	P0 = date;
	delay(5);
	EN = 1;
	delay(5);
	EN = 0;	
}

void init()
{
	uchar num;
	DU = 1;
	P0 = 0x00;
	DU = 0;
	WE = 1;
	P0 = 0x00;
	WE = 0;

    write_com(0x38);
	write_com(0x0c);
	write_com(0x06);
	write_com(0x01);
	write_com(0x80);

	for(num = 0;num < 12;num++)
	{
		write_date(table[num]);
		delay(5);
	}
	write_com(0x80+0x40);
	for(num = 0;num < 12;num++)
	{
		write_date(table1[num]);
		delay(5);
	}
}

 void LCD_Init(void)		//静态初始化
 {
    uchar num;
	DU = 1;
	P0 = 0x00;
	DU = 0;
	WE = 1;
	P0 = 0x00;
	WE = 0;

    write_com(0x38);    /*显示模式设置*/
/*    delay(5);
    write_com(0x38);
    delay(5);
    write_com(0x38);
    delay(5);
    write_com(0x38);
    write_com(0x08);    /*显示关闭*/
/*    write_com(0x01);    /*显示清屏*/
/*    write_com(0x06);    /*显示光标移动设置*/
/*    delay(5);
	write_com(0x0c);
    write_com(0x80);    

	for(num = 0;num < 12;num++)
	{
		write_date(table[num]);
		delay(5);
	}
	write_com(0x80+0x40);
	for(num = 0;num < 12;num++)
	{
		write_date(table1[num]);
		delay(5);
	}
 }

void main()
{
//	init();
	LCD_Init();
	while(1);
}
*/

