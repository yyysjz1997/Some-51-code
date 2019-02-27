#include <reg52.h>
#include <intrins.h>
#include <math.h>
#define uchar unsigned char
#define uint unsigned int
sbit DS = P2^2;      //总线     //define interface
sbit RS = P1^0;   //定义端口
sbit RW = P1^1;
sbit EN = P2^5;
sbit DU = P2^6;
sbit WE = P2^7;
uint temp;             // variable of temperature
//uchar flag1;            // sign of the result positive or negative

void delay(uint count)      //delay
{
  uint i;
  while(count)
  {
    i=200;
    while(i>0)
    i--;
    count--;
  }
}


void dsreset(void)       //send reset and initialization command
{
  uint i;
  DS=0;
  i=103;				   //将总线拉低480us~960us
  while(i>0)i--;
  DS=1;					   //然后拉高总线，若DS18B20做出反应会将在15us~60us后将总线拉低
  i=4;					   //15us~60us等待
  while(i>0)i--;
  //while(DS);
}

bit tmpreadbit(void)       //read a bit
{
   uint i;
   bit dat;
   DS=0;i++;          //i++ for delay
   DS=1;i++;i++;
   dat=DS;
   i=8;while(i>0)i--;
   return (dat);
}

uchar tmpread(void)   //read a byte date
{
  uchar i,j,dat;
  dat=0;
  for(i=1;i<=8;i++)
  {
    j=tmpreadbit();
    dat=(j<<7)|(dat>>1);   //读出的数据最低位在最前面，这样刚好一个字节在DAT里
  }
  return(dat);
}

void tmpwritebyte(uchar dat)   //write a byte to ds18b20
{
  uint i;
  uchar j;
  bit testb;
  for(j=1;j<=8;j++)      //一个字节8位数据，每次只能写一个位
  {
    testb=dat&0x01;  //依次将dat的每一位赋值给testb
    dat=dat>>1;
    if(testb)     //write 1
    {
      DS=0;
      i++;i++;  //大于1us
      DS=1;
      i=8;while(i>0)i--;  //至少60us
    }
    else
    {
      DS=0;       //write 0
      i=8;while(i>0)i--;    //至少60us
      DS=1;
      i++;i++;
    }

  }
}

void tmpchange(void)   //转换温度的函数（测一次温度）  //DS18B20 begin change
{
  dsreset();
  delay(1);
  tmpwritebyte(0xcc);   //跳过ROM操作  // address all drivers on bus
  tmpwritebyte(0x44);  //测一次温(温度转换命令))  //  initiates a single temperature conversion
  //delay(100);
}

uint tmp()               //get the temperature
{
  float tt;
  uchar a,b;
/*******************发送读取命令******************/
  dsreset();
  delay(1);
  tmpwritebyte(0xcc);      //跳过ROM操作
  tmpwritebyte(0xbe);     //读取（暂存器）
/************************************************/
  a=tmpread();           //读取温度值共16位，先读低字节
  b=tmpread();           //再读高字节的8位
  temp=b;
  temp<<=8;             //two byte  compose a int variable
  temp=temp|a;           //temp是uint的2个字节的16位
  tt=temp*0.0625;      //芯片本身的计算公式
  //出场设置默认R0、R1为11。也就是12位分辨率，也就是1位代表0.0625摄氏度.
  temp=tt*10+0.5;
  return temp;
}

void readrom()          //read the serial
{
  uchar sn1,sn2;
  dsreset();
  delay(1);
  tmpwritebyte(0x33);
  sn1=tmpread();
  sn2=tmpread();
}


void delay10ms()            //delay
  {
    uchar a,b;
    for(a=10;a>0;a--)
      for(b=60;b>0;b--);
   }

/******************************液晶程序*********/
void cmg88()//关数码管
{
		DU = 1;
		P0 = 0X00;
		DU = 0;
		WE = 0;
}

void DelayUs2x(unsigned char t)
{
 	while(--t);
}

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

void display(uint temp)			//1602液晶显示程序
{
   uchar q,w,e;
   uint yy = abs(temp);
   q = temp / 100;
   w = temp %100 / 10;
   e = temp % 10;
   /*LCD_Write_Char(6,1,q+'0');
   LCD_Write_Char(7,1,w+'0');
   LCD_Write_Char(8,1,'.');
   LCD_Write_Char(9,1,e+'0');*/
   LCD_Write_Char(6,1,q+48);   //ASCII码表中 1和‘1’的区别
   LCD_Write_Char(7,1,w+48);
   LCD_Write_Char(8,1,'.');
   LCD_Write_Char(9,1,e+48);	
}

void main()
{
  cmg88();//关数码管，点阵函数
  LCD_Init();   //静态初始化
  LCD_Clear();//清屏
  LCD_Write_String(3,0,"temperature");	
  while(1)
  {
      tmpchange();
      display(tmp());
  }
}

