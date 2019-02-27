#include<reg51.h>
#include<intrins.h>

#define uchar unsigned char
#define uint unsigned int 
#define Write_PCF8591 0x90
#define Read_PCF8591 0x91
#define Control_Byte 0x80	  //0xf0为实验板上的L3亮
#define LED P1

sbit SCL=P2^1;
sbit SDA=P2^0;

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

void DAC_PCF8591(uchar add,uchar dat)
{
    start();
    Write_Byte(add);
    ack();
    Write_Byte(Control_Byte);
    ack();
    Write_Byte(dat);
    ack();
    stop();
}

uchar ADC_Read(uchar add0,uchar add1,uchar chl)		//chl=0，则重复一遍停止；chl=1，则循环
{
    uchar dat;
    start();
    Write_Byte(add0);
    ack();
    Write_Byte(Control_Byte|chl);
    ack();
    start();
    Write_Byte(add1);
    ack();
    dat=Read_Byte();
    noack();
    stop();
    return dat;
}

void main()
{
    temp = 0;
    num = 0;
    init_IIC();
    while(1)
    {
        DAC_PCF8591(Write_PCF8591,num);
        num++;
        delayms(500);
        temp=ADC_Read(Write_PCF8591,Read_PCF8591,1);
        LED=temp;
    }
}
