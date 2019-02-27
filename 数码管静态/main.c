#include<reg52.h>
sbit WE=P2^7;
sbit DU=P2^6;
unsigned  char  num;
unsigned char code yyy[] ={
0x3f,0x06,0x5b,0x4f,0x66,0x6d,
0x7d,0x07,0x7f,0x6f,0x77,0x7c,
0x39,0x5e,0x79,0x71
};	
void delay(unsigned int xms);
void main()
{
	WE=1;  //打开位选锁存器
	P0=0x00; //位选低电平有效
    WE=0;		//关闭

	while(1)
	{
		for(num=0;num<16;num++)
		{
			  DU=1;
			  P0=yyy[num];
	     	  DU=0;
			  delay(1000);
		}
	}
}

void delay(unsigned int xms)
{
	unsigned int i,j;
	for(i=xms;i>0;i--)
		for(j=112;j>0;j--);
}