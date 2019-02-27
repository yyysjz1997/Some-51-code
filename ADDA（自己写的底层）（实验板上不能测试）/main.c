#include <reg52.h>
#define uchar unsigned char
#define uint unsigned int
sbit SCL = P2^1;
sbit SDA = P2^0;

void delay(uint z);

void main()
{
	//P0 = 0;
	while(1)
	{
		SDA = 1;
		SCL = 1;
		delay(5);
		SDA = 0;
		delay(5);
		SCL = 0; 

		delay(10);

		SCL = 1;
		delay(5);
		SDA = 1;

	}
}

void delay(uint z)
{
	uint x,y;
	for(x=z;x>0;x--)
		for(y=112;y>0;y--);
}

