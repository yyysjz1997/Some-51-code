#include <reg52.h>

void main()
{
	TMOD = 0x20;	 //确定T1的工作方式，方式2	：8位计数方式，一次两个十六进制。方便LED显示

	TH1 = 0xfd;	//根据PPT上公式，算出为波特率为9600时的TH1值
	TL1 = 0xfd;

	TR1 = 1;	   //启动T1

	REN = 1;	  //设为1为串口允许接收

	SM0 = 0;	 //SCON串行口控制方式1
	SM1 = 1;

	EA = 1;
	ES = 1;

	while(1)
	{
	/*	if(RI == 1)
		{
			RI = 0;
			P1 = SBUF;
		}  */
	}
}

void ser()interrupt 4		   //中断形式
{
	RI = 0;
	P1 = SBUF;
}
