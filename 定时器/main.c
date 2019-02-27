#include<reg52.h>
#define uint unsigned int 
#define uchar unsigned char 
sbit dula=P2^6;
sbit wela=P2^7;
sbit d1=P1^0;
uchar num;
uchar tt;
uchar code table[]={
0x3f,0x06,0x5b,0x4f,
0x66,0x6d,0x7d,0x07,
0x7f,0x6f,0x77,0x7c,
0x39,0x5e,0x79,0x71};
void main()
{
	num=0;
	tt=0;
	TMOD=0x01;//设置定时器0为工作方式1
	TH0=(65536-10000)/256;
	TL0=(65536-10000)%256;
	EA=1;//开总中断
	ET0=1;//开定时器0中断
	TR0=1;//启动定时器0
	wela=1;//11101010
	P0=0x00;
	wela=0;
	dula=1;
	P0=0x3f;
	dula=0;
	while(1)
	{
		if(tt==20)
		   {
				tt=0;
				num++;
				if(num==16)
				{
					num=0;
				}
				dula=1;
				P0=table[num];
				dula=0;
					
			}
	}
}

void time() interrupt 1
{
	TH0=(65536-50000)/256;
	TL0=(65536-50000)%256;
	tt++;
}
