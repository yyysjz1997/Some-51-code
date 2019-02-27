#include <reg52.h>
#define uchar unsigned char
#define uint unsigned int
sbit DS=P2^2;      //总线     //define interface
uint temp;             // variable of temperature
uchar flag1;            // sign of the result positive or negative
sbit dula=P2^6;
sbit wela=P2^7;
unsigned char code table[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,
                        0x07,0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71};				//无小数点
unsigned char code table1[]={0xbf,0x86,0xdb,0xcf,0xe6,0xed,0xfd,
                        0x87,0xff,0xef};		  //加小数点儿

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

void display(uint temp)			//显示程序
{
   uchar A1,A2,A2t,A3,ser;
   ser=temp/10;
   SBUF=ser;
   A1=temp/100;
   A2t=temp%100;
   A2=A2t/10;
   A3=A2t%10;
   dula=0;
   P0=table[A1];		//显示百位
   dula=1;
   dula=0;

   wela=0;
   P0=0xfe;
   wela=1;
   wela=0;
   delay(1);

   dula=0;
   P0=table1[A2];		//显示十位
   dula=1;
   dula=0;

   wela=0;
   P0=0xfd;
   wela=1;
   wela=0;
   delay(1);

   P0=table[A3];		//显示个位
   dula=1;
   dula=0;

   P0=0xfb;
   wela=1;
   wela=0;
   delay(1);
}


void main()
{
 uchar a;
  do
  {
      tmpchange();
      for(a=100;a>0;a--)
      {
          display(tmp());
      }
  }while(1);
}

