#include <reg52.h>
#define uint unsigned int
#define uchar unsigned char
sbit WE = P2^7;
sbit DU = P2^6;
sbit D1 = P1^3;
sbit K1 = P3^4;
sbit K2 = P3^5;
unsigned char code table[10]={0x3f,0x06,0x5b,0x4f,
0x66,0x6d,0x7d,0x07,0x7f,0x6f};// 显示段码值0~9

uchar speed;
uchar ge,shi,num;

void delay(uint z)
{
    uint x,y;
    for(x = z;x > 0;x--)
        for(y = 112;y > 0;y--);
}

void display(uchar speed2)
{   
    shi = speed2/10;
    ge = speed2%10;

    P0 = table[ge];
    DU = 1;
    DU = 0;
    P0 = 0xfd;
    WE = 1;
    WE = 0;
    delay(5);

    P0 = table[shi];
    DU = 1;
    DU = 0;
    P0 = 0xfe;
    WE = 1;
    WE = 0;
    delay(5);
}

void init()
{
    WE = 0;
    DU = 0;
    EA = 1;
    TMOD = 0x01;
    ET0 = 1;
    TR0 = 1;   
}

void main()
{
    init();
    speed = 1;
    while(1)
    {	   
	    display(speed);
        if(K1 == 0)
        {
            delay(5);
            if(K1 == 0)
            {
                while(!K1);//等松手
                if(speed<18)
                {
                    speed++;                    
                }
            }
        }
        else if(K2 == 0)
        {
            delay(5);
            if(K2 == 0)
            {
                while(!K2);
                if(speed>1)
                {
                    speed--;
                }
            }
        }
    }
} 

void time0()interrupt 1
{  
    static uchar time;
    TH0 = (65536 - 1000)/256;
    TL0 = (65536 - 1000)%256;  
	if(time > speed)
	{
	        D1 = 0;
	}
	else
	{
	        D1 = 1;
	}
	time++;
	if(time == 19)
	{
	        time = 0;
	}
	       
} 