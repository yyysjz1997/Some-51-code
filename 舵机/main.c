#include <reg52.h>
sbit D1 = P1^0;

void delay()
{
    unsigned int i,j;
	for(i = 1;i > 0;i-- )
		for(j = 103;j > 0;j--);
}

void main()
{
	//D1 = 1;
	while(1)
	{
		D1 = ~D1;
		delay();
	}
}		