#include <reg52.h>
#define uchar unsigned char
#define uint unsigned int
sbit LED1 = P1^0;
sbit LED2 = P1^1;
sbit K1 = P3^7;
sbit K2 = P3^6;

void delay(unsigned int xms)
{
	unsigned int i,j;
	for(i=xms;i>0;i--)
		for(j=112;j>0;j--);
}

void main()
{
	EA = 1;
	EX0 = 1;
	EX1 = 1;
	
	IT0 = 0;
	IT1 = 0;	
	
	LED1 = 1;
	LED2 = 1;	
}

void in()interrupt 0
{
	LED1 = 0;
}

void in2()interrupt 2
{
	LED2 = 0;
}

