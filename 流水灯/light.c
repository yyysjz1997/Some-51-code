#include <reg52.h>
#include <intrins.h>
#include <math.h>
#define uint unsigned int 
sbit FM=P2^3;

void delay(uint xms)		 //ÑÓ³Ùº¯Êý		
{
	uint i,j;
	for(i=xms;i>0;i--)		      //i=xms¼´ÑÓÊ±Ô¼xmsºÁÃë
		for(j=112;j>0;j--);
}

void main()
{
     P1=0xfe;
	 while(1)
	 {
	 	P1=_crol_(P1,1);
		FM=1;
		delay(100);
		FM=~FM;
		delay(100);	
	 }
}