#include <reg52.h>

#define uchar unsigned char
#define uint unsigned int
sbit out=P1^5;//发送IO口

void delay(uint xms)
{
	uint i,j;
	for(i=xms;i>0;i--)		      //i=xms即延时约xms毫秒
		for(j=112;j>0;j--);
}

void delay560us(void)	//560us延迟函数
{
	uint j;
	for(j=63;j>0;j--);
}

void delay4500us(void)	   //4.5ms延迟函数
{
  	uint j;
	for(j=516;j>0;j--);
}

void khz_2(uint num)		 //38KHZ脉冲 占空比1:2
{
    for(;num>0;num--)    //num是持续时间
	 {
//	  _nop_();
	  out=~out;
	 }
}

void send0_a(void)    //发送0
{
   khz_2(42) ;   //0.56ms的38kHZ的脉冲
   //khz_3(21) ;
   out=1;
   delay560us();
}

void send1_a(void)		//发送1
{
   khz_2(42) ;    //0.56ms的38kHZ的脉冲
   //khz_3(21) ;
   out=1;
   delay560us();
   delay560us();
   delay560us();
}

void leadcode_a(void)	//发送引导码
{
     khz_2(690) ;     //9ms的38kHZ的脉冲
	 //khz_3(345) ;
	 out=1;
   	 delay4500us();

}

const uchar TabHL1[12]={0x30,0x18,0x7a,0x10,0x38,0x5a,0x42,0x4a,0x52,0x00,0xff,0xa6};//数据码码表1-9&2字节用户码

void Send8Bit(uchar d)			  //发送一字节数据
{
    if(d&0x80){	send1_a();}   //一位一位的判断
	else{		send0_a();}
	if(d&0x40){	send1_a();}
	else{		send0_a();}
	if(d&0x20){	send1_a();}
	else{		send0_a();}
	if(d&0x10){	send1_a();}
	else{		send0_a();}
	if(d&0x08){	send1_a();}
	else{		send0_a();}
	if(d&0x04){	send1_a();}
	else{		send0_a();}
	if(d&0x02){	send1_a();}
	else{		send0_a();}
	if(d&0x01){	send1_a();}
	else{		send0_a();}

}


void usercode_xuexiban()	  //发送用户码 00FF
{
     Send8Bit(TabHL1[9]);
	 Send8Bit(TabHL1[10]);
}

void xuexiban_1()	//00110000 		发送”1“的全部信号
 {
     leadcode_a();   //引导码
     usercode_xuexiban();   //用户码
     Send8Bit(TabHL1[0]);
	 Send8Bit(TabHL1[0]);//数据反码（校验用）（直接用的源码，这样方便简单，但是稳定性不好）
 }

 void xuexiban_2()	//00011000
 {
     leadcode_a();
     usercode_xuexiban();
	 Send8Bit(TabHL1[1]);
	 Send8Bit(TabHL1[1]);
 }

 void xuexiban_3()	//01111010
 {
     leadcode_a();
     usercode_xuexiban();
	 Send8Bit(TabHL1[2]);
	 Send8Bit(TabHL1[2]);
 }

 void xuexiban_4()	//00010000
 {
     leadcode_a();
     usercode_xuexiban();
	 Send8Bit(TabHL1[3]);
	 Send8Bit(TabHL1[3]);
 }

 void xuexiban_5()	//00111000
 {
     leadcode_a();
     usercode_xuexiban();
	 Send8Bit(TabHL1[4]);
	 Send8Bit(TabHL1[4]);
 }

 void xuexiban_6()	//01011010
 {
     leadcode_a();
     usercode_xuexiban();
	 Send8Bit(TabHL1[5]);
	 Send8Bit(TabHL1[5]);
 }

 void xuexiban_7()	//01000010
 {
     leadcode_a();
     usercode_xuexiban();
	 Send8Bit(TabHL1[6]);
	 Send8Bit(TabHL1[6]);
 }

 void xuexiban_8()	//01001010
 {
     leadcode_a();
     usercode_xuexiban();
     Send8Bit(TabHL1[7]);
	 Send8Bit(TabHL1[7]);
 }

  void xuexiban_9()	//01010010
 {
     leadcode_a();
     usercode_xuexiban();
     Send8Bit(TabHL1[8]);
	 Send8Bit(TabHL1[8]);
 }

 void Remote_control()   //按键发送，矩阵键盘前九个按键分别是1-9
 {
    uchar temp;
 	P3 = 0xfe;
	temp = P3;
	temp = temp & 0xf0;
	if(temp != 0xf0)
	{
			delay(5);
			temp = P3;
			temp = temp & 0xf0;
			while(temp != 0xf0)
			{
				temp = P3;
				switch(temp)
				{
				 	case 0xee:xuexiban_1();
				 		break;
				 	case 0xde:xuexiban_2();
				 		break;
				 	case 0xbe:xuexiban_3();
				 		break;
				 	case 0x7e:xuexiban_4();
				 		break;
				}
				while (temp != 0xf0)
				{
					temp = P3;
					temp = temp & 0xf0;
				}
			}
	}	

	P3 = 0xfd;
	temp = P3;
	temp = temp & 0xf0;
	if(temp != 0xf0)
	{
		delay(5);
		temp = P3;
		temp = temp & 0xf0;
		while(temp !=0xf0)
		{
			temp = P3;
			switch(temp)
			{
				 case 0xed:xuexiban_5();
				 	break;
				 case 0xdd:xuexiban_6();
				 	break;
				 case 0xbd:xuexiban_7();
				 	break;
				 case 0x7d:xuexiban_8();
				 	break;
			}
			while (temp != 0xf0)
			{
				temp = P3;
				temp = temp & 0xf0;
			}
		}
	}

	P3 = 0xfb;
	temp = P3;
	temp = temp & 0xf0;
	if(temp != 0xf0)
	{
		delay(5);
		temp = P3;
		temp = temp & 0xf0;
		while(temp !=0xf0)
		{
			temp = P3;
			switch(temp)
			{
				 case 0xeb:xuexiban_9();
				 	break;
			}
			while (temp != 0xf0)
			{
				temp = P3;
				temp = temp & 0xf0;
			}
		}
	}	
 }

 void main()
 {
 	while(1)
	{
		Remote_control();	
	}	
 }