//J8插光度和温度传感器


#include<reg51.h>
#include<intrins.h>

#define uchar unsigned char
#define uint unsigned int 
#define Write_PCF8591 0x90
#define Read_PCF8591 0x91
#define Control_Byte 0x80	  //0xf0为实验板上的L3亮

sbit SCL=P2^1;
sbit SDA=P2^0;
sbit RST=P2^4;   //时钟 加上后可以关掉DS1302芯片输出
sbit DU=P2^6;
sbit WE=P2^7;

unsigned char code table[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,
                        0x07,0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71};

uchar num,temp;

void delayms(uint z)
{
    uint x,y;
    for(x=0;x<z;x++)
        for(y=0;y<112;y++);
}

void delay()
{
    _nop_();_nop_();
    _nop_();_nop_();
    _nop_();_nop_();
    _nop_();_nop_();
}
/***************************************IIC***************************************/
void init_IIC()
{
    SDA=1;
    delay();
    SCL=1;
    delay();
}

void start()
{
    SDA=1;
    delay();
    SCL=1;
    delay();
    SDA=0;
    delay();
}

void stop()
{
    SDA=0;
    delay();
    SCL=1;
    delay();
    SDA=1;
    delay();
}

void ack()   //应答
{
    uchar i=0;
    SCL=1;
    delay();
    while((SDA==1)&&(i<250)) i++;
    SCL=0;
    delay();
}

void noack()  //非应答
{
    SDA=1;
    delay();
    SCL=1;
    delay();
    SCL=0;
    delay();
}

void Write_Byte(uchar dat)
{
    uchar i;
    for(i=0;i<8;i++)
    {
        SCL=0;
        delay();
        if(dat&0x80)
        {
            SDA=1;
        }
        else
        {
            SDA=0;
        }
        dat=dat<<1;
        SCL=1;
        delay();
    }
    SCL=0;
    delay();
    SDA=1;
    delay();    
}

uchar Read_Byte()
{
    uchar i,dat;
    SCL=0;
    delay();
    SDA=1;
    delay();
    for(i=0;i<8;i++)
    {
        SCL=1;
        delay();
        dat=dat<<1;
        if(SDA)
        {
            dat++;
        }
        SCL=0;
        delay();
    }
    return dat;
}
/******************************************************************IIC********************************/
/*****************************************************************AD/DA*******************************/
void DAC_PCF8591(uchar add,uchar dat)//写入DA数模转换值
{
    start();
    Write_Byte(add);		//写入芯片地址
    ack();
    Write_Byte(Control_Byte);   //写入控制位
    ack();
    Write_Byte(dat);   //写数据
    ack();
    stop();
}

uchar ADC_Read(uchar add0,uchar add1,uchar chl)		//chl=0，则重复一遍停止；chl=1，则循环 用0就好
{		//读取AD模数转换的值
    uchar dat;
    start();
    Write_Byte(add0);    //芯片地址
    ack();
    Write_Byte(Control_Byte|chl);	//选择的通道
    ack();
    start();
    Write_Byte(add1);	   //读入地址
    ack();
    dat=Read_Byte();	 //读数据
    noack();
    stop();
    return dat;
}
/****************************************************AD/DA*************************************/
void display (uchar num)
{	
        uchar bai,shi,ge;
		bai = num / 100;
		shi = num / 10 %10;
		ge = num % 10;

		DU=1;
		P0=table[ge];
		DU=0;
		WE=1;
		P0=0xfb;
		WE=0;
		delayms(5);

		DU=1;
		P0=table[shi];
		DU=0;
		WE=1;
		P0=0xfd;
		WE=0;
        delayms(5);

		DU=1;
		P0=table[bai];
		DU=0;
		WE=1;
		P0=0xfe;
		WE=0;
		delayms(5);
}

void init_time0()
{
	    TMOD |= 0x01;	  //使用模式1，16位定时器，使用"|"符号可以在使用多个定时器时不受影响		     
 		TH0=0x00;		  //重新赋值 1/68s
 		TL0=0x00;
 		EA=1;            //总中断打开
 		ET0=1;           //定时器中断打开
 		TR0=1;           //定时器开关打开	
}

void main()
{
	init_IIC();
	init_time0();
	RST = 0;
    DU = 0;
	WE = 0;
	while(1)
	{
		 display(num);
	}	
}

void time0()interrupt 1
{
	TH0 = 0;
	TL0 = 0;
	//DAC_PCF8591(Write_PCF8591,0);
	num = ADC_Read(Write_PCF8591,Read_PCF8591,0);

}