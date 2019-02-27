
/**********************BST-V51实验开发板例程************************
*  平台：BST-V51 + Keil U3 + STC89C52
*  名称：LCD1602模块实验
*  公司：深圳市亚博软件开发有限公司
*  淘宝：bstmcu.taobao.com
*  日期：2013-11
*  晶振:11.0592MHZ
*  说明：免费开源，不提供源代码分析.
******************************************************************/
//实验目的：
/*-----------------------------------------------
  名称：LCD1602
  内容：通过标准程序静态显示字符
  引脚定义如下：1-VSS 2-VDD 3-V0 4-RS 5-R/W 6-E 7-14 DB0-DB7 15-BLA 16-BLK
------------------------------------------------*/
#include<reg52.h> //包含头文件，一般情况不需要改动，头文件包含特殊功能寄存器的定义
#include<intrins.h>

sbit RS = P1^0;   //定义端口
sbit RW = P1^1;
sbit EN = P2^5;
sbit DU = P2^0;
sbit WE = P2^1;

#define RS_CLR RS=0
#define RS_SET RS=1

#define RW_CLR RW=0
#define RW_SET RW=1

#define EN_CLR EN=0
#define EN_SET EN=1

#define DataPort P0

void cmg88()//关数码管，点阵函数
{
DU=1;
P0=0X00;
DU=0;
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
/*------------------------------------------------
              判忙函数
------------------------------------------------*/
 bit LCD_Check_Busy(void)
 {
 DataPort= 0xFF;
 RS_CLR;
 RW_SET;
 EN_CLR;
 _nop_();
 EN_SET;
 return (bit)(DataPort & 0x80);
 }
/*------------------------------------------------
              写入命令函数
------------------------------------------------*/
 void LCD_Write_Com(unsigned char com)
 {
 while(LCD_Check_Busy()); //忙则等待
 RS_CLR;
 RW_CLR;
 EN_SET;
 DataPort= com;
 _nop_();
 EN_CLR;
 }
/*------------------------------------------------
              写入数据函数
------------------------------------------------*/
 void LCD_Write_Data(unsigned char Data)
 {
 while(LCD_Check_Busy()); //忙则等待
 RS_SET;
 RW_CLR;
 EN_SET;
 DataPort= Data;
 _nop_();
 EN_CLR;
 }

/*------------------------------------------------
                清屏函数
------------------------------------------------*/
 void LCD_Clear(void)
 {
 LCD_Write_Com(0x01);
 DelayMs(5);
 }
/*------------------------------------------------
              写入字符串函数
------------------------------------------------*/
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
/*------------------------------------------------
              写入字符函数
------------------------------------------------*/
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
/*------------------------------------------------
              初始化函数
------------------------------------------------*/
 void LCD_Init(void)
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
   }

/*------------------------------------------------
                    主函数
------------------------------------------------*/
void main(void)
{
cmg88();//关数码管，点阵函数
LCD_Init();
LCD_Clear();//清屏
while (1)
 {
  //LCD_Write_Char(7,0,'o');
  //LCD_Write_Char(8,0,'k');
  //LCD_Write_String(1,1,"hello world");
    //LCD_Write_String(1,0,"yyy loves yjg");
    LCD_Write_String(4,1,"forever");
    LCD_Write_String(1,0,"yjg loves yyy");

  while(1);
 }
}

