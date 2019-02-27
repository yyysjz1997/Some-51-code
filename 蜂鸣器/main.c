#include<reg52.h>
#define ON 0
#define OFF 1
sbit FM=P2^3;
							   
void delay(unsigned int t)
{
	 while(--t)
	 ;
}

void main()
{
  while(1)
  {
     FM=ON;
	 delay(100000);
	 FM=OFF;
	 delay(100000);	
	}
}
