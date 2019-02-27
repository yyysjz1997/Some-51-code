#include <reg52.h>
#define uchar unsigned char
#define uint unsigned int
sbit key = P3^4;
sbit WE = P2^7;
sbit DU = P2^6;
sbit LED = P1^0;
uchar code table[]={
0x3f,0x06,0x5b,0x4f,
0x66,0x6d,0x7d,0x07,
0x7f,0x6f,0x77,0x7c,
0x39,0x5e,0x79,0x71};
uchar num;
void delay(uint z);

void main()
{
         num = 0;
		 WE = 1;
		 P0 = 0xfe;
		 WE = 0;

		 DU = 1;
		 P0 = table[num];
		 DU = 0;

		 key = 1;
		 while(1)
		 {
		 	if(key == 0)
			{
				delay(5);
				if(key == 0)
				{
					LED = 0;
					num++;
					if(num == 10)
					{
						num = 0;
					}						
				    DU = 1;
					P0 = table[num];
					DU = 0;
				}
				while(!key);
				delay(5);
				while(!key);  
			}
			else
			{
				LED = 1;
			}
		 }
}

void delay(uint z)
{
	uint x,y;
	for(x=z;x>0;x--)
		for(y=112;y>0;y--);
}
