#include<reg52.h>   
#include<intrins.h>
#define uint unsigned int
#define uchar unsigned char
sbit RS = P1^0;   //定义端口
sbit RW = P1^1;
sbit EN = P2^5;
sbit DU = P2^6;
sbit WE = P2^7;
sbit FM = P2^3;
sbit IR=P3^2;  //红外接口标志
sbit IN1=P1^2; // 高电平1 后退（反转）
sbit IN2=P1^3; // 高电平1 前进（正转）
sbit IN3=P1^6; // 高电平1 前进（正转）	
sbit IN4=P1^7; // 高电平1 后退（反转）
sbit EN1=P1^4; // 高电平使能 
sbit EN2=P1^5; // 高电平使能 
#define DataPort P0 //定义数据端口 程序中遇到DataPort 则用P0 替换
/*------------------------------------------------
                全局变量声明
------------------------------------------------*/
unsigned char  irtime;//红外用全局变量

bit irpro_ok,irok;
unsigned char IRcord[4];
unsigned char irdata[33];

/*------------------------------------------------
                  函数声明
------------------------------------------------*/

void Ir_work(void);
void Ircordpro(void);
void LCD_Write_Char(unsigned char x,unsigned char y,unsigned char Data)	;
void run();
void back();
void right();
void left();
void stop();

/*------------------------------------------------
                  定时器0中断处理
------------------------------------------------*/

void tim0_isr (void) interrupt 1 using 1
{
  irtime++;  //用于计数2个下降沿之间的时间
}

/*------------------------------------------------
                  外部中断0中断处理
------------------------------------------------*/
void EX0_ISR (void) interrupt 0 //外部中断0服务函数
{
  static unsigned char  i;             //接收红外信号处理
  static bit startflag;                //是否开始处理标志位

if(startflag)                         
   {
    if(irtime<63&&irtime>=33)//引导码 TC9012的头码，9ms+4.5ms
                        i=0;
    		irdata[i]=irtime;//存储每个电平的持续时间，用于以后判断是0还是1
    		irtime=0;
    		i++;
   			 if(i==33)
      		{
	  			  irok=1;   //做好了准备
				    i=0;
	  			}
          }
  	 else
		{
		irtime=0;
		startflag=1;
		}

}

/*------------------------------------------------
                定时器0初始化
------------------------------------------------*/
void TIM0init(void)//定时器0初始化
{

  TMOD=0x02;//定时器0工作方式2，TH0是重装值，TL0是初值
  TH0=0x00; //重载值
  TL0=0x00; //初始化值
  ET0=1;    //开中断
  TR0=1;    
}
/*------------------------------------------------
                  外部中断0初始化
------------------------------------------------*/
void EX0init(void)
{
 IT0 = 1;   //指定外部中断0下降沿触发，INT0 (P3.2)
 EX0 = 1;   //使能外部中断
 EA = 1;    //开总中断
}
/*------------------------------------------------
                  键值处理
------------------------------------------------*/

void Ir_work(void)//红外键值散转程序
{
       switch(IRcord[2])//判断第三个数码值
	         {
			 case 0x18:run();break;//前
			 case 0x52:back();break;//后
			 case 0x08:left();;break;//左
			 case 0x5a:right();break;//右
			 case 0x1c:stop();break;//停

             default:break;
			 }

		  irpro_ok=0;//处理完成标志

  }
/*------------------------------------------------
                红外码值处理
------------------------------------------------*/
void Ircordpro(void)//红外码值处理函数
{ 
  unsigned char i, j, k;
  unsigned char cord,value;

  k=1;
  for(i=0;i<4;i++)      //处理4个字节
     {
      for(j=1;j<=8;j++) //处理1个字节8位
         {
          cord=irdata[k];
          if(cord>7)//大于某值为1，这个和晶振有绝对关系，这里使用12M计算，此值可以有一定误差
             value|=0x80;
          if(j<8)
		    {
			 value>>=1;
			}
           k++;
         }
     IRcord[i]=value;
     value=0;     
     } 
	 irpro_ok=1;//处理完毕标志位置1
}

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

 void run(void)			  //前进函数
{
	EN1=1;
	EN2=1;//电机使能
    IN1=0;
	IN2=1; //左电机的正转
	IN3=1;
	IN4=0;//右电机的正转
}

void back(void)			  //后退函数
{
	EN1=1;
	EN2=1;//电机使能
    IN1=1;
	IN2=0; //左电机的反转
	IN3=0;
	IN4=1;//右电机的反转
}

void right(void)		 //右转函数
{
	EN1=1;
	EN2=1;//电机使能
    IN1=0;
	IN2=1; //左电机的正转
	IN3=0;
	IN4=0;//右电机不动
}

void left(void)			  //左转函数
{
	EN1=1;
	EN2=1;//电机使能
    IN1=0;
	IN2=0; //左电机不动
	IN3=1;
	IN4=0;//右电机正转
}

void stop(void)			  //停止函数
{
	EN1=1;
	EN2=1;//电机使能
    IN1=0;
	IN2=0; //左电机不动
	IN3=0;
	IN4=0;//右电机正转
}

/*------------------------------------------------
                    主函数
------------------------------------------------*/
void main(void)
{
    EX0init(); //初始化外部中断
    TIM0init();//初始化定时器
	FM = 1;
 	cmg88();//关数码管，点阵函数
	LCD_Init();   //静态初始化
	LCD_Clear();//清屏

    //LCD_Write_String(0,0," value of RC:");
 while(1)//主循环
   {
    if(irok)                        //如果接收好了进行红外处理
	  {   
	   Ircordpro();
 	   irok=0;
	  }

    if(irpro_ok)                   //如果处理好后进行工作处理，如按对应的按键后显示对应的数字等
	  {
	   Ir_work();
	   FM = 0;
	   DelayMs(100);
	   FM = 1;

  	  }
   }
}
  
  