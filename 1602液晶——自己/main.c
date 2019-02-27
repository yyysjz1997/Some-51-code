#include<reg52.h> //包含头文件，一般情况不需要改动，头文件包含特殊功能寄存器的定义
#include<intrins.h>
#define uint unsigned int
#define uchar unsigned char
sbit RS = P1^0;   //定义端口
sbit RW = P1^1;
sbit EN = P2^5;
sbit DU = P2^6;
sbit WE = P2^7;
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

 void LCD_Init2(void)	//动态初始化
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
	   LCD_Write_Com(0x07);
	   //LCD_Write_Com(0x80+0x10);
 }	

void delay(uint z)
{
	uint x,y;
	for(x=z;z>0;z--)
		for(y=112;y>0;y--);
}

#if 1
void main(void)
{
	cmg88();//关数码管，点阵函数
	LCD_Init2();   //动态初始化
	LCD_Clear();//清屏
	while (1)
	{
	   	 //LCD_Write_Char(7,0,'o');
	 	 //LCD_Write_Char(8,0,'k');
	 	 //LCD_Write_String(1,1,"hello world");
		 for(num = 40;num > 0;num--)
		 {
		 	 LCD_Write_String(num,0," YYY AND YJG ");
		     LCD_Write_String(num+2,1," FOREVER ");
			 if(num == 24)
			 {
			 	LCD_Write_String(6,0," YYY AND YJG ");
		    	LCD_Write_String(8,1," FOREVER ");
				delay(5000);
				LCD_Write_Com(0x01);	  
			    for(num_m = 24;num_m > 0;num_m--)
		 		{		
					    LCD_Write_String(num_m,0," YYY AND YJG ");
						LCD_Write_String(num_m+2,1," FOREVER ");
						delay(500);
						LCD_Write_Com(0x01);
						delay(50);	  
				}
			 }
			 else
			 {
			 	 delay(500);
             	 LCD_Write_Com(0x01);
			 	 delay(50);
			 }
			 
		 }
	} 
}

#elif 0
void main(void)
{
	cmg88();//关数码管，点阵函数
	LCD_Init();   //静态初始化
	LCD_Clear();//清屏
	while (1)
	{
	   	 LCD_Write_Char(7,0,'o');
	 	 LCD_Write_Char(8,0,'k');
	 	 LCD_Write_String(2,1,"hello world!");
	}
}

#endif