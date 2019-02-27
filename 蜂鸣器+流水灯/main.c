#include<reg52.h>
#define ON 0;
#define OFF 1;
sbit FM=P2^3;
void delay(unsigned int xms);
void main()
{
	unsigned char i;
	while(1)
	{
		P1=0xfe;
		for(i=0;i<8;i++)
		{
			delay(100);
			P1<<=1;
			P1=P1|0x01;
			FM=ON;
			delay(100);
			FM=OFF;
		}
	}
}

void delay(unsigned int xms)
{
	unsigned int i,j;
	for(i=xms;i>0;i--)
		for(j=112;j>0;j--);
}