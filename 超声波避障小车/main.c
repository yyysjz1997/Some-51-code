#include <reg52.h>
#include <intrins.h>
#define uint unsigned int
#define uchar unsigned char
sbit RS = P1^0;   //定义端口
sbit RW = P1^1;
sbit EN = P2^5;
sbit DU = P2^6;
sbit WE = P2^7;
sbit RX = P2^0 ;//接收端
sbit TX = P2^1 ;//控制端
sbit FM = P2^3;
sbit key = P3^7;
sbit IN1=P1^2; // 高电平1 后退（反转）
sbit IN2=P1^3; // 高电平1 前进（正转）
sbit IN3=P1^6; // 高电平1 前进（正转）	
sbit IN4=P1^7; // 高电平1 后退（反转）
sbit EN1=P1^4; // 高电平使能 
sbit EN2=P1^5; // 高电平使能 

uint x,y;
uchar a;
uchar num,num_m;
void delay(uint z);
void DelayUs2x(unsigned char t);
void DelayMs(unsigned char t);
bit LCD_Check_Busy(void);
void LCD_Write_Com(unsigned char com) ;
void LCD_Write_Data(unsigned char Data);
void LCD_Write_String(unsigned char x,unsigned char y,unsigned char *s);
void LCD_Write_Char(unsigned char x,unsigned char y,unsigned char Data);
void LCD_Clear(void);
void distance_show();
void gogogo();
uchar shu;
uint time;
unsigned long dis;
unsigned long distance[4];
unsigned char code ASCII[10] = {0,1,2,3,4
,5,6,7,8,9};
/************************************超声波模块*****************************/
void start_ultrasonic()	 //开启超声波
{
	TX = 1;
	delay(1);	 //两次测距之间的时间间隔
	TX = 0;
}

void  count_high()
{
	TR0 = 1;
	while(RX);	 //计数
	TR0 = 0;
	shu = 0;
	distance_show();
	gogogo();	
}

void time0()interrupt 1
{
	shu++;	
}

void distance_show()
{
	time = TH0 * 256 + TL0;
	TH0 = 0;
	TL0 = 0;

	dis = (time * 1.7)/10 + 10;
	if(dis > 2800||dis < 25)
	{
		LCD_Write_String(0,1,"out of range!   ");
	}
	else
	{
		distance[0]=dis%10;
	    distance[1]=dis/10%10;
	    distance[2]=dis/100%10;
	    distance[3]=dis/1000;

	    LCD_Write_String(0,1,"Distance:");
	    LCD_Write_Char(9, 1, ASCII[distance[3]]+'0');
	    LCD_Write_Char(10, 1, ASCII[distance[2]]+'0');
	    LCD_Write_Char(11, 1, ASCII[distance[1]]+'0');
        LCD_Write_Char(12, 1, '.');
	    LCD_Write_Char(13, 1, ASCII[distance[0]]+'0');
		LCD_Write_Char(14, 1, 'C');
		LCD_Write_Char(15, 1, 'M');
	}
	
}




/************************************超声波模块*****************************/

/**********************************1602液晶******************************/
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

void delay(uint z)
{
	uint x,y;
	for(x=z;z>0;z--)
		for(y=112;y>0;y--);
}

void _1602_init()
{
	cmg88();//关数码管，点阵函数
	LCD_Init();   //静态初始化
	LCD_Clear();//清屏	
}

/************************1602液晶************************************/

/***************************车控制***********************************/
void run(void)			  
{
	EN1=1;
	EN2=1;
    IN1=0;
	IN2=1; //左电机的正转
	IN3=1;
	IN4=0;//右电机的正转
}

void back(void)			
{
	EN1=1;
	EN2=1;
    IN1=1;
	IN2=0; //左电机的反转
	IN3=0;
	IN4=1;//右电机的反转
}

void right(void)		 
{
	EN1=1;
	EN2=1;
    IN1=0;
	IN2=1; //左电机的正转
	IN3=0;
	IN4=0;//右电机不动
}

void left(void)		
{
	EN1=1;
	EN2=1;
    IN1=0;
	IN2=0; //左电机不动
	IN3=1;
	IN4=0;//右电机正转
}

void stop(void)			 
{
	EN1=1;
	EN2=1;
    IN1=0;
	IN2=0; //左电机不动
	IN3=0;
	IN4=0;//右电机正转
}

void spin_right(void)			  //向右打转函数
{
	EN1=1;
	EN2=1;//电机使能
    IN1=0;
	IN2=1; //左电机正转
	IN3=0;
	IN4=1;//右电机反转
}

void gogogo()
{
     if(dis <= 240)  
     {  
          a++;
          while(a > 3)
          {
              //a=0;
              FM=0;
              back(); 
              delay(230);

                right();
                delay(100);        
                //stop();
                delay(100);     
                start_ultrasonic();                 
                while(RX==0);
                TR0 = 1;             
                while(RX);      
                TR0 = 0;            
                distance_show();

                if(dis>340)  
                {
                    a=0;
                    right();
                    delay(90);  
                    stop();               
                    delay(200);        
                    FM=1;
                    run();
                }             
           } 
     }
     else
     {
          a=0;
          run();       //无障碍物 直行
     }
} 

void keyscan()
{
  A:	if(key == 0)
		{
			delay(10);
			if(key == 0)
			{
				FM = 0;
				while(!key);
				FM = 1;
			}
			else
			{
				goto A;
			}
		 }
		 else
		 {
		 	goto A;
		 }
}

/**************************车控制*************************************/
void main(void)
{
    uint i;
	_1602_init();

	TMOD=0x01;
    EA=1;		  
    TH0=0;
    TL0=0;
    ET0=1; 
	LCD_Write_String(2,0,"come on baby");         
	keyscan();
	
 	while(1)
	  {	   
		RX=1;
		start_ultrasonic();		 //启动模块 
        for(i = 951;i > 0;i--)
	    {
	       if(RX==1)
		   {
               count_high();		 //超声波高电平脉冲宽度计算函数
		   }
    	 }	
	  }	
}