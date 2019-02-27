#include <reg52.h>
#include <intrins.h>
#define uchar unsigned char
#define uint unsigned int
sbit DU=P2^6;
sbit WE=P2^7;
uchar code table[]={
0x3f,0x06,0x5b,0x4f,
0x66,0x6d,0x7d,0x07,
0x7f,0x6f,0x77,0x7c,
0x39,0x5e,0x79,0x71,
0x76,0x79,0x38,0x3f,0};
uchar temp,aa,bb,cc,bai,shi,ge,t0,t1,flag,flag1;
uint shu;
void delay(uint z);
void init();
void display(uchar aa,uchar bb, uchar cc, uchar bai, uchar shi, uchar ge);

void main()
{
	init();
	while(1)
	{
	    if(flag1 != 1 )
		{
			display(7, 6, 5, bai ,shi,ge);
		}
		else
		{
			//display( table[16],table[17],table[18],table[18],table[19],table[20]);
			display( 16,17,18,18,19,20);
		}
		
	}
}

void time0()interrupt 1			  //控制发光二极管的定时器
{
	TH0 = (65536 - 50000) / 256;
	TL0 = (65536 - 50000) % 256;
	t0++;
	if(flag != 1)
	{
	   if(t0 ==10)
		{
			t0 = 0;
			temp = _crol_(temp , 1);
			P1 = temp;
		}
	}
	else
    {
		if((t0%4) == 0)
		{
	    	P1=~P1;	
		}
		if(t0 == 60)
		{
			flag1 = 1;
			P1 = 0xff;	
			TR0 = 0;		
		}
	}
	
}

void time1()interrupt  3			  //控制数码管的定时器
{
	TH1 = (65536 - 50000) / 256;
	TL1 = (65536 - 50000) % 256;
	t1++;
	if(t1 == 5)
	{
		t1 = 0;
		shu--;
		bai = shu / 100;
		shi = shu % 100 / 10;
		ge = shu % 10;
	
		if(shu == 398)
		{
			TR0 = 0;
			TH0 = (65536 - 50000) / 256;
			TL0 = (65536 - 50000) % 256;
			TR0 = 1;
			t0 = 0;
			P1 = 0xff;
			TR1 = 0;
			flag = 1;
		}



	}	
}

void init()
{     
    shu = 432;
	flag = 0;
	flag1 = 0;	
	aa = 0;
	bb = 0;
	cc = 0;
	bai = 0;
	shi = 0;
	ge = 0;
	temp = 0;

    temp = 0xfe;
	P1 = temp; 

	TMOD = 0x11;
	TH0 = (65536 - 50000) / 256;
	TL0 = (65536 - 50000) % 256;
	TH1 = (65536 - 50000) / 256;
	TL1 = (65536 - 50000) % 256;
	EA = 1;
	ET0 = 1;
	ET1 = 1;
	TR0 = 1;
	TR1 = 1;		
}

void delay(uint z)
{
	uint t1,y;
	for(t1=z;t1>0;t1--)
		for(y=112;y>0;y--);
}

void display(uchar aa,uchar bb, uchar cc, uchar bai, uchar shi, uchar ge)
{
    DU = 1;
	P0 = table[aa];
	DU = 0;
	
	P0 = 0x00;   //和教程不同,试试  

	WE = 1;
	P0 = 0xfe;
	WE = 0;
	delay(1);

	DU = 1;
	P0 = table[bb];
	DU = 0;
	
	P0 = 0xff;   //和教程不同,试试  

	WE = 1;
	P0 = 0xfd;
	WE = 0;
	delay(1);

	DU = 1;
	P0 = table[cc];
	DU = 0;
	
	P0 = 0xff;   //和教程不同,试试  

	WE = 1;
	P0 = 0xfb;
	WE = 0;
	delay(1);

	DU = 1;
	P0 = table[bai];
	DU = 0;
	
	P0 = 0xff;   //和教程不同,试试  

	WE = 1;
	P0 = 0xf7;
	WE = 0;
	delay(1);

	DU = 1;
	P0 = table[shi];
	DU = 0;
	
	P0 = 0xff;   //和教程不同,试试  
			
	WE = 1;
	P0 = 0xef;
	WE = 0;
	delay(1);

	DU = 1;
	P0 = table[ge];
	DU = 0;
	
	P0 = 0xff;   //和教程不同,试试  

	WE = 1;
	P0 = 0xdf;
	WE = 0;
	delay(1);
}