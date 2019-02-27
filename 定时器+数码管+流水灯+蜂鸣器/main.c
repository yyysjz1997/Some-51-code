#include <reg52.h>
#include <intrins.h>  

#define uint unsigned int
#define uchar unsigned char
#define ON 0
#define OFF 1

sbit DU=P2^6;
sbit WE=P2^7;
sbit FM=P2^3;

uchar code table[]={
0x3f,0x06,0x5b,0x4f,
0x66,0x6d,0x7d,0x07,
0x7f,0x6f,0x77,0x7c,
0x39,0x5e,0x79,0x71};

uchar code table2[]={
0xfe,0xfd,0xfb,0xf7,
0xef,0xdf,0xbf,0x7f};

uchar temp,aa,num,num2;

void delay(uint z);

void main()
{
       temp = 0;
	   aa = 0;
	   num = 0;
	   num2 = 0;
	   TMOD = 0x01;
	   TH0 = (65536 - 50000) / 256;
	   TL0 = (65536 - 50000) % 256;
	   EA = 1;
	   ET0 = 1;
	   TR0 = 1;

	   temp = 0xfe;
	   P1 = temp;

	   WE = 1;
	   P0 = table2[0];
	   WE = 0;

	   DU = 1;
	   P0 = table[0];
	   DU = 0;

	   while(1);


}

void delay(uint z)
{
	uint x,y;
	for(x=z;x>0;x--)
		for(y =110;y>0;y--);
}

void timer0() interrupt 1 
{
	TH0=(65536-50000)/256;
	TL0=(65536-50000)%256;
	aa++;
		if(aa==20)
			{

			   
			    temp = _crol_(temp,1);
			    P1=temp;


				aa=0;
				num++;
				num2++;

				if(num==8)
					num=0;	
				DU=1;
				P0=table[num];
				DU=0;	
				
				if(num2==8)
					num2=0;					  					 	
				WE=1;
				P0=table2[num];
				WE=0;

				if(num==0)
				{
					FM = ON;
					delay(100);
					FM=~FM;
				}
					
			}	
}
