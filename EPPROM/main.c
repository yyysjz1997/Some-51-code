//开机秒数记忆
#include<reg52.h>
#define uchar unsigned char
#define uint unsigned int
bit  write=0;           //写24C02的标志；
sbit sda=P2^0;                         
sbit scl=P2^1;
sbit dula=P2^6;
sbit wela=P2^7;
sbit RST=P2^5;   //时钟  不需要，可以关掉
sbit CS_DA =P2^4 ;  //HJ-3G DAC HL-1不需要，可以关掉
sbit JD=P0^6;
sbit FM=P2^3;


uchar sec,tcnt; 
uchar code table[]={
0x3f,0x06,0x5b,0x4f,
0x66,0x6d,0x7d,0x07,
0x7f,0x6f,0x77,0x7c,
0x39,0x5e,0x79,0x71};
void ioin()  //IO口继电器、数码管锁存函数
   {	  
	  wela=1;
     P0 = 1;  
	  JD = 0;    
     wela=0;
	  dula=1;
	  P0=0;
	  dula=0;
    }
void delay()
{ ;; }
void delay1ms(uint z)
{
	uint x,y;
	for(x=z;x>0;x--)
		for(y=110;y>0;y--);
}
void start()  //开始信号
{	
	sda=1;
	delay();
	scl=1;
	delay();
	sda=0;
	delay();
}
void stop()   //停止
{
	sda=0;
	delay();
	scl=1;
	delay();
	sda=1;
	delay();
}
void respons()  //应答
{
	uchar i;
	scl=1;
	delay();
	while((sda==1)&&(i<250))i++;
	scl=0;
	delay();
}
void init()
{
   RST=0; //总线时钟低电评
   CS_DA =0 ;
	sda=1;
	delay();
	scl=1;
	delay();
}
void write_byte(uchar date)
{
	uchar i,temp;
	temp=date;
	for(i=0;i<8;i++)
	{
		temp=temp<<1;
		scl=0;
	    delay();
		sda=CY;
		delay();
		scl=1;
		delay();
	}
	scl=0;
	delay();
	sda=1;
	delay();
}
uchar read_byte()
{
	uchar i,k;
	scl=0;
	delay();
	sda=1;
	delay();
	for(i=0;i<8;i++)
	{
		scl=1;
		delay();	
		k=(k<<1)|sda;
		scl=0;
		delay();	
	}
	return k;
}
void write_add(uchar address,uchar date)
{
	start();
	write_byte(0xa0);
	respons();
	write_byte(address);
	respons();
	write_byte(date);
	respons();
	stop();
}
uchar read_add(uchar address)
{
	uchar date;
	start();
	write_byte(0xa0);
	respons();
	write_byte(address);
	respons();
	start();
	write_byte(0xa1);
	respons();
	date=read_byte();
	stop();
	return date;
}
void display(uchar bai_c,uchar sh_c)	//显示程序
{
   dula=0;
   P0=table[bai_c];		//显示第一位
   dula=1;
   dula=0;
   wela=0;
   P0=0xfe;
   wela=1;
   wela=0;
   delay1ms(5);
   dula=0;
   P0=table[sh_c];		//显示第二位
   dula=1;
   dula=0;
   wela=0;
   P0=0xfd;
   wela=1;
   wela=0;
   delay1ms(5);
}
void main()
{
   ioin();
	init();
	sec=read_add(1);    //读出保存的数据赋于sec
	if(sec>100)        //防止首次读取出错误数据
		sec=0;
	TMOD=0x01;  			//定时器工作在方式1
	ET0=1; 
	EA=1;
	TH0=(65536-50000)/256; //对TH0 TL0赋值
	TL0=(65536-50000)%256; //使定时器0.05秒中断一次
	TR0=1;                 //开始计时
	while(1)
	{
		display(sec/10,sec%10);
		if(write==1) 		      //判断计时器是否计时一秒
		{
			write=0;              //清零
			write_add(1,sec);  	  //在24c02的地址2中写入数据sec
		}
	} 
}
void t0() interrupt 1 //定时中断服务函数
{
	TH0=(65536-50000)/256; //对TH0 TL0赋值
	TL0=(65536-50000)%256; //重装计数初值
	tcnt++;        //每过50ms tcnt加一
	if(tcnt==5)  //计满20次（1秒）时
	{
	    tcnt=0;   //重新再计
	    sec++;
	    write=1;  //1秒写一次24C02
	    if(sec==100) //定时100秒，再从零开始计时
	    	sec=0;
	}
}
