#include <reg52.h>
#define uint unsigned int
#define uchar unsigned char

uchar temp,aa,bai,shi,ge;

sbit DU=P2^6;
sbit WE=P2^7;

uchar code table[]={
0x3f,0x06,0x5b,0x4f,
0x66,0x6d,0x7d,0x07,
0x7f,0x6f,0x77,0x7c,
0x39,0x5e,0x79,0x71};

void delay(uint z);
void init();
void display(uchar bai,uchar shi,uchar ge);

void main()
{	
	init();
	while(1)
	{
		if(aa==10)
		  {
			aa=0;
			temp++;
			if(temp==255)
			   {
					temp=0;
			   }
			bai=temp/100;
			shi=temp%100/10;
			ge=temp%10;
		  }
		display(bai,shi,ge);
	}
}


void delay(uint z)
{
	uint x,y;
	for(x=z;x>0;x--)
		for(y=112;y>0;y--);
}

void init()
{	
	WE=0;
	DU=0;

	temp=0;
	aa=0;
	bai=0;
	shi=0;
	ge=0;

	TMOD=0x01;
	TH0=(65536-50000)/256;
	TL0=(65536-50000)%256;
	EA=1;
	ET0=1;
	TR0=1;	
}


void display(uchar bai,uchar shi,uchar ge)
{
	    DU=1;
		P0=table[bai];
		DU=0;
		P0=0xff;
		WE=1;
		P0=0xfe;
		WE=0;
		delay(1);

		DU=1;
		P0=table[shi];
		DU=0;
		P0=0xff;
		WE=1;
		P0=0xfd;
		WE=0;
		delay(1);

		DU=1;
		P0=table[ge];
		DU=0;
		P0=0xff;
		WE=1;
		P0=0xfb;
		WE=0;
		delay(1);
}
  
void time0() interrupt 1
{
    TH0=(65536-50000)/256;
	TL0=(65536-50000)%256;
	aa++;
}
