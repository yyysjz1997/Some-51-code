#include <reg52.h>
#
#define uchar unsigned char
#define uint unsigned int
sbit SDA = P2^0;
sbit SCL = P2^1;
uchar a;

void delay()
{ ; ; }

void delay1(uint z)
{
      uint x,y;
      for(x=z;x>0;x--)
            for(y=112;y>0;y--);
}

void start()      //起始信号
{
      SDA = 1;
      delay();
      SCL = 1;
      delay();
      SDA = 0;
      delay();
}

void stop()      //终止信号
{
      SDA = 0;
      delay();
      SCL = 1;
      delay();
      SDA = 1;
      delay();
}

void respons()    //应答
{
      uchar i;
      SCL = 1;
      delay();
      while((SDA == 1)&&(i < 250))   //SDA无应答就一直死循环，除非时间太长
      {
            i++;
      }
      SCL = 0;
      delay();


}

void init()       //初始化
{
      SDA = 1;
      delay();
      SCL = 1;
      delay();
}

void write_byte(uchar date)
{
      uchar i,temp;
      temp = date;
      for(i=0;i<8;i++)
      {
            temp = temp << 1;
            SCL = 0;
            delay();
            SDA = CY;
            delay();
            SCL = 1;
            delay();
      }
      SCL = 0;
      delay();
      SDA = 1;//释放总线信号，为了检验应答信号（SDA = 0）
      delay();
}

uchar read_byte()
{
      uchar i,k;
      SCL = 0;
      delay();
      SDA = 1;
      delay();
      for(i=0;i<8;i++)
      {
            SCL = 1;
            delay();
            k = (k<<1)|SDA;
            SCL = 0;
            delay();
      }
      return k;
}

void write_add(uchar address,uchar date)
{
      start();
      write_byte(0xa0);  //起始地址
      respons();
      write_byte(address);   //要传数据的地址
      respons();
      write_byte(date);   //要传的数据
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
      write_byte(0xa1);   //器件地址
      respons();
      date = read_byte();
      stop();
      return date;
}

void main()
{
      init();
      write_add(23,0xaa);
      delay1(100);
      P1 = read_add(23);      //让数码管亮
      while(1);
}