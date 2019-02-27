
//  实验效果：在1602液晶屏上显示各通道的电压值
#include<reg52.h>    //包含单片机寄存器的头文件
#include <intrins.h> 

#define  AddWr 0x90    //PCF8591 地址

// 变量定义
unsigned char AD_CHANNEL;
sbit RST=P2^4;   //时钟 加上后可以关掉DS1302芯片输出
sbit scl=P2^1;       //I2C  时钟 
sbit sda=P2^0;       //I2C  数据 
bit ack;                 /*应答标志位*/

sbit RS = P1^0;//Pin4
sbit RW = P1^1; //Pin5
sbit E  = P2^5;//Pin6


#define Data  P0    //数据端口

unsigned char TempData[8];
unsigned char  SecondLine[]="                ";
unsigned char  FirstLine[] ="                ";
/*******************************************************************
                     起动总线函数               
函数原型: void  Start_I2c();  
功能:     启动I2C总线,即发送I2C起始条件.  
********************************************************************/
void Start_I2c()
{
  sda=1;         /*发送起始条件的数据信号*/
  _nop_();
  scl=1;
  _nop_();        /*起始条件建立时间大于4.7us,延时*/
  _nop_();
  _nop_();
  _nop_();
  _nop_();    
  sda=0;         /*发送起始信号*/
  _nop_();        /* 起始条件锁定时间大于4μs*/
  _nop_();
  _nop_();
  _nop_();
  _nop_();       
  scl=0;       /*钳住I2C总线，准备发送或接收数据 */
  _nop_();
  _nop_();
}

/*******************************************************************
                      结束总线函数               
函数原型: void  Stop_I2c();  
功能:     结束I2C总线,即发送I2C结束条件.  
********************************************************************/
void Stop_I2c()
{
  sda=0;      /*发送结束条件的数据信号*/
  _nop_();       /*发送结束条件的时钟信号*/
  scl=1;      /*结束条件建立时间大于4μs*/
  _nop_();
  _nop_();
  _nop_();
  _nop_();
  _nop_();
  sda=1;      /*发送I2C总线结束信号*/
  _nop_();
  _nop_();
  _nop_();
  _nop_();
}

/*******************************************************************
                 字节数据发送函数               
函数原型: void  I2C_SendByte(UCHAR c);
功能:     将数据c发送出去,可以是地址,也可以是数据,发完后等待应答,并对
          此状态位进行操作.(不应答或非应答都使ack=0)     
           发送数据正常，ack=1; ack=0表示被控器无应答或损坏。
********************************************************************/
void  I2C_SendByte(unsigned char  c)
{
 unsigned char  i;
 
 for(i=0;i<8;i++)  /*要传送的数据长度为8位*/
    {
     if((c<<i)&0x80)sda=1;   /*判断发送位*/
       else  sda=0;                
     _nop_();
     scl=1;               /*置时钟线为高，通知被控器开始接收数据位*/
      _nop_(); 
      _nop_();             /*保证时钟高电平周期大于4μs*/
      _nop_();
      _nop_();
      _nop_();         
     scl=0; 
    }
    
    _nop_();
    _nop_();
    sda=1;                /*8位发送完后释放数据线，准备接收应答位*/
    _nop_();
    _nop_();   
    scl=1;
    _nop_();
    _nop_();
    _nop_();
    if(sda==1)ack=0;     
       else ack=1;        /*判断是否接收到应答信号*/
    scl=0;
    _nop_();
    _nop_();
}

/*******************************************************************
                 字节数据接收函数               
函数原型: UCHAR  I2C_RcvByte();
功能:        用来接收从器件传来的数据,并判断总线错误(不发应答信号)，
          发完后请用应答函数应答从机。  
********************************************************************/    
unsigned char   I2C_RcvByte()
{
  unsigned char  retc=0,i; 
  sda=1;                     /*置数据线为输入方式*/
  for(i=0;i<8;i++)
      {
        _nop_();           
        scl=0;                  /*置时钟线为低，准备接收数据位*/
        _nop_();
        _nop_();                 /*时钟低电平周期大于4.7μs*/
        _nop_();
        _nop_();
        _nop_();
        scl=1;                  /*置时钟线为高使数据线上数据有效*/
        _nop_();
        _nop_();
        retc=retc<<1;
        if(sda==1)retc=retc+1;  /*读数据位,接收的数据位放入retc中 */
        _nop_();
        _nop_(); 
      }
  scl=0;    
  _nop_();
  _nop_();
  return(retc);
}

/********************************************************************
                     应答子函数
函数原型:  void Ack_I2c(bit a);
功能:      主控器进行应答信号(可以是应答或非应答信号，由位参数a决定)
********************************************************************/
void Ack_I2c(bit a)
{  
  if(a==0)sda=0;              /*在此发出应答或非应答信号 */
  else sda=1;				  /*0为发出应答，1为非应答信号 */
  _nop_();
  _nop_();
  _nop_();      
  scl=1;
  _nop_();
  _nop_();                    /*时钟低电平周期大于4μs*/
  _nop_();
  _nop_();
  _nop_();  
  scl=0;                     /*清时钟线，住I2C总线以便继续接收*/
  _nop_();
  _nop_();    
}

/************************************************************
* 函数名        : Pcf8591_DaConversion
* 函数功能      : PCF8591的输出端输出模拟量
* 输入          : addr（器件地址），channel（转换通道），value（转换的数值）
* 输出         	: 无
******************* *****************************************/
bit Pcf8591_DaConversion(unsigned char addr,unsigned char channel,  unsigned char Val)
{
   Start_I2c();              //启动总线
   I2C_SendByte(addr);            //发送器件地址
   if(ack==0)return(0);
   I2C_SendByte(0x40|channel);              //发送控制字节
   if(ack==0)return(0);
   I2C_SendByte(Val);            //发送DAC的数值  
   if(ack==0)return(0);
   Stop_I2c();               //结束总线
   return(1);
}

/************************************************************
* 函数名        : Pcf8591_SendByte
* 函数功能		: 写入一个控制命令
* 输入          : addr（器件地址），channel（转换通道）
* 输出         	: 无
************************************************************/
bit PCF8591_SendByte(unsigned char addr,unsigned char channel)
{
   Start_I2c();              //启动总线
   I2C_SendByte(addr);            //发送器件地址
   if(ack==0)return(0);
   I2C_SendByte(0x40|channel);              //发送控制字节
   if(ack==0)return(0);
   Stop_I2c();               //结束总线
   return(1);
}

/************************************************************
* 函数名       	: PCF8591_RcvByte
* 函数功能   	: 读取一个转换值
* 输入          :
* 输出          : dat
************************************************************/
unsigned char PCF8591_RcvByte(unsigned char addr)
{  
   unsigned char dat;

   Start_I2c();          //启动总线
   I2C_SendByte(addr+1);      //发送器件地址
   if(ack==0)return(0);
   dat=I2C_RcvByte();          //读取数据0

   Ack_I2c(1);           //发送非应答信号
   Stop_I2c();           //结束总线
   return(dat);
}

/******************************************************************/
/*                   1602液晶屏相关函数                                     */
/******************************************************************/
void DelayUs(unsigned char us)//delay us
{
 unsigned char uscnt;
 uscnt=us>>1;/* Crystal frequency in 12MHz*/
 while(--uscnt);
}
/******************************************************************/
void DelayMs(unsigned char ms)//delay Ms
{
 while(--ms)
   {
     DelayUs(250);
     DelayUs(250);
	 DelayUs(250);
	 DelayUs(250);
   }
}
void WriteCommand(unsigned char c)
{
 DelayMs(5);//short delay before operation
 E=0;
 RS=0;
 RW=0;
 _nop_();
 E=1;
 Data=c;
 E=0;
}
/****************************************************************/
void WriteData(unsigned char c)
{
 DelayMs(5);   //short delay before operation
 E=0;
 RS=1;
 RW=0;
 _nop_();
 E=1;
 Data=c;
 E=0;
 RS=0;
}
/*********************************************************************/
void ShowChar(unsigned char pos,unsigned char c)
{
 unsigned char p;
 if (pos>=0x10)
    p=pos+0xb0; //是第二行则命令代码高4位为0xc
 else 
    p=pos+0x80; //是第二行则命令代码高4位为0x8
 WriteCommand (p);//write command
 WriteData (c);   //write data
}
/*************************************************************************/
void ShowString (unsigned char line,char *ptr)
{
 unsigned char l,i;
 l=line<<4;
 for (i=0;i<16;i++)
  ShowChar (l++,*(ptr+i));//循环显示16个字符
}
/*********************************************************************/
void InitLcd()
{
 DelayMs(15);
 WriteCommand(0x38); //display mode
 WriteCommand(0x38); //display mode
 WriteCommand(0x38); //display mode
 WriteCommand(0x06); //显示光标移动位置
 WriteCommand(0x0c); //显示开及光标设置
 WriteCommand(0x01); //显示清屏

}


/////////////////////////////////////////////////////////////////
void disp(void)

{


 FirstLine[2]='0'+TempData[0];
 FirstLine[4]='0'+TempData[1];
 FirstLine[3]='.';
 FirstLine[6]='V';

 FirstLine[9]='0'+TempData[2];
 FirstLine[11]='0'+TempData[3];
 FirstLine[10]='.';
 FirstLine[13]='V';



 
 SecondLine[2]='0'+TempData[4];
 SecondLine[4]='0'+TempData[5];
 SecondLine[3]='.';
 SecondLine[6]='V';

 SecondLine[9]='0'+TempData[6];
 SecondLine[11]='0'+TempData[7];
 SecondLine[10]='.';
 SecondLine[13]='V';

ShowString(0,FirstLine);

ShowString(1,SecondLine);

}
/*------------------------------------------------
                    延时程序
------------------------------------------------*/
 void mDelay(unsigned char j)
 {
  unsigned int i;
  for(;j>0;j--)
     {
	  for(i=0;i<125;i++)
	     {;}
	  }
  }
/*------------------------------------------------
                    主函数
------------------------------------------------*/
main()
{  
	 unsigned char ADtemp;                //定义中间变量
	 InitLcd();
     mDelay(20);
	 RST=0;		//		关时钟DS1302 
	while(1)
	{

	/********以下AD-DA处理*************/  
	
	    switch(AD_CHANNEL)
		{
			case 0: PCF8591_SendByte(AddWr,1);
			     ADtemp = PCF8591_RcvByte(AddWr);  //ADC0 模数转换1	 J8上可接任意电阻元件
				 TempData[0]=ADtemp/50;//处理0通道电压显示
  				 TempData[1]=(ADtemp%50)/10;
				 break;  
			
			case 1: PCF8591_SendByte(AddWr,2);
			     ADtemp=PCF8591_RcvByte(AddWr);  //ADC1  模数转换2
				 TempData[2]=ADtemp/50;//处理1通道电压显示
  				 TempData[3]=(ADtemp%50)/10;
				 break;  
			
			case 2: PCF8591_SendByte(AddWr,3);
			     ADtemp=PCF8591_RcvByte(AddWr);  //ADC2	模数转换3   可调电阻SW1
				 TempData[4]=ADtemp/50;//处理2通道电压显示
  				 TempData[5]=(ADtemp%50)/10;
				 break;  
			
			case 3: PCF8591_SendByte(AddWr,0);
			     ADtemp=PCF8591_RcvByte(AddWr);  //ADC3   模数转换4	 可调电阻SW2
				 TempData[6]=ADtemp/50;//处理3通道电压显示
  				 TempData[7]=(ADtemp%50)/10;
				 break;  
			
			case 4: Pcf8591_DaConversion(AddWr,0, ADtemp); //DAC	  数模转换
			     break;
		}			
	   if(++AD_CHANNEL>4) AD_CHANNEL=0;
	   disp();	 
   }
}