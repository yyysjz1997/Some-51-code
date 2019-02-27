#include<reg51.h>
#include <intrins.h>
#define uint unsigned int
#define uchar unsigned char
sbit sda=P2^0;                     //IO口定义
sbit scl=P2^1;

void nop()
{
    _nop_();
    _nop_();
}

/////////24C02读写驱动程序////////////////////
void delay1(unsigned int m)
{   unsigned int n;
    for(n=0;n<m;n++);
}

void delay(uint z)
{
	uint x,y;
	for(x = z;x > 0;x--)
		for(y = 112;y > 0;y--);
}

void init()  //24c02初始化子程序
{
    scl=1;
    nop();
    sda=1;
    nop();
}

void start()        //启动I2C总线
{
    sda=1;
    nop();
    scl=1;
    nop();
    sda=0;
    nop();
    scl=0;
    nop();
}

void stop()         //停止I2C总线
{
    sda=0;
    nop();
    scl=1;
    nop();
    sda=1;
    nop();
}

void writebyte(unsigned char j)  //写一个字节
{
    unsigned char i,temp;
    temp=j;
    for (i=0;i<8;i++)
   {
       temp=temp<<1;
       scl=0;
       nop();
       sda=CY;      //temp左移时，移出的值放入了CY中
       nop();
       scl=1;       //待sda线上的数据稳定后，将scl拉高
       nop();
   }
   scl=0;
   nop();
   sda=1;
   nop();
}

unsigned char readbyte()   //读一个字节
{
   unsigned char i,j,k=0;
   scl=0; nop(); sda=1;
   for (i=0;i<8;i++)
   {  
        nop(); scl=1; nop();
        if(sda==1) 
        j=1;
        else
        j=0;
        k=(k<<1)|j;
        scl=0;
    }
    nop();
    return(k);
}
 
void clock()         //I2C总线时钟 等价于  respond函数
{
   unsigned char i=0;
   scl=1;
   nop();
   while((sda==1)&&(i<255))
      i++;
   scl=0;
   nop();
}


////////从24c02的地址address中读取一个字节数据/////
unsigned char read24c02(unsigned char address)
{
   unsigned char i;
   start();
   writebyte(0xa0);
   clock();
   writebyte(address);
   clock();
   start();
   writebyte(0xa1);
   clock();
   i=readbyte();
   stop();
   delay1(100);
   return(i);
} 


//////向24c02的address地址中写入一字节数据info/////
void write24c02(unsigned char address,unsigned char info)
{
   start();
   writebyte(0xa0);
   clock();
   writebyte(address);
   clock();
   writebyte(info);
   clock();
   stop();
   delay1(5000); //这个延时一定要足够长，否则会出错。因为24c02在从sda上取得数据后，还需要一定时间的烧录过程。
}

void refresh()
{
	uint i;
    init();        //初始化24C02
    for(i=0;i<256;i++)
    {
        write24c02(i,0);//将各个地址所对应的存储单元逐一清零。
    }
    //P1=0x00;//格式化完成，发光LED全亮，作为提示。
   // delay(500);
}

void main()
{
	refresh();
	while(1)
	{
		;//自己加东西	
	}
}