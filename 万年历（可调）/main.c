#include<reg52.h>
#define uint unsigned int
#define uchar unsigned char
uchar a,miao,shi,fen,ri,yue,nian,week,flag,temp,key1n;
#define yh 0x80  //第一行的初始位置
#define er 0x80+0x40 //第二行初始位置
//sbit cs1=P3^7;
//sbit ledk=P2^5;
sbit dula=P2^6;
sbit wela=P2^7;
sbit wr=P1^1;
sbit rs=P1^0;
sbit en=P2^5;
sbit SCLK=P2^1;  // 6  P1.1
sbit IO=P2^0;    // 7  P1.0
sbit RST=P2^4;   //时钟 5  P1.2
sbit ACC0=ACC^0;
sbit ACC7=ACC^7;
sbit key=P3^7;
sbit key1=P3^0;
sbit key2=P3^1;
sbit key3=P3^2;
sbit FM = P2^3;
uchar code tab1[]={"2005-12-31 SUN"};
uchar code tab2[]={"23:59:56"};
void delay(uint xms)//延时函数
{
	uint x,y;
	for(x=xms;x>0;x--)
	 for(y=110;y>0;y--);
}
/******************液晶写入************************/
write_1602com(uchar com)//液晶写入指令函数
{
	rs=0;//置为写入命令
	P0=com;//送入数据
	delay(1);
	en=1;//拉高使能端
	delay(1);
	en=0;//完成高脉冲
}
write_1602dat(uchar dat)
{
	rs=1;//置为写入数据
	P0=dat;//送入数据
	delay(1);
	en=1;
	delay(1);
	en=0;
}

void write_sfm(uchar add,uchar dat)//写时分秒
{
	uchar gw,sw;
	gw=dat%10;
	sw=dat/10;
	write_1602com(er+add);
	write_1602dat(0x30+sw);
	write_1602dat(0x30+gw);
}
void write_nyr(uchar add,uchar dat)
{
	uchar gw,sw;
	gw=dat%10;
	sw=dat/10;
	write_1602com(yh+add);
	write_1602dat(0x30+sw);
	write_1602dat(0x30+gw);


}
void write_week(uchar week)//写星期函数
{
	write_1602com(yh+0x0d);
	switch(week)
	{
		case 1:write_1602dat('M');//delay(5);
			   write_1602dat('O');//delay(5);
			   write_1602dat('N');
			   break;

		case 2:write_1602dat('T');//delay(5);
			   write_1602dat('U');//delay(5);
			   write_1602dat('E');
			   break;

		case 3:write_1602dat('W');//delay(5);
			   write_1602dat('E');//delay(5);
			   write_1602dat('D');
			   break;

		case 4:write_1602dat('T');//delay(5);
			   write_1602dat('H');//delay(5);
			   write_1602dat('U');
			   break;

		case 5:write_1602dat('F');//delay(5);
			   write_1602dat('R');//delay(5);
			   write_1602dat('I');
			   break;

		case 6:write_1602dat('S');//delay(5);
			   write_1602dat('T');//delay(5);
			   write_1602dat('A');
			   break;

		case 7:write_1602dat('S');//delay(5);
			   write_1602dat('U');//delay(5);
			   write_1602dat('N');
			   break;


}



}
lcd_init()//液晶初始化函数
{       wr=0;
	write_1602com(0x38);//设置液晶工作模式
	write_1602com(0x0c);//开显示不显示光标
	write_1602com(0x06);//整屏不移动，指针加一
	write_1602com(0x01);
	write_1602com(yh+2);//字符写入的位置
	for(a=0;a<14;a++)
	{
	write_1602dat(tab1[a]);
	//delay(3);
	}
	write_1602com(er+4);
	for(a=0;a<8;a++)
	{
	write_1602dat(tab2[a]);
	//delay(3);
	}

}
/*********************over***********************/
/********************ds1302****************************/
void write_byte(uchar dat)
{
	ACC=dat;
	RST=1;
	for(a=8;a>0;a--)
	{
		IO=ACC0;
		SCLK=0;
		SCLK=1;
		ACC=ACC>>1;
	}
}
uchar read_byte()
{
	RST=1;
	for(a=8;a>0;a--)
	{
		ACC7=IO;
		SCLK=1;
		SCLK=0;
		ACC=ACC>>1;

	}
	return (ACC);
}
void write_1302(uchar add,uchar dat)
{

	RST=0;
	SCLK=0;
	RST=1;
	write_byte(add);
	write_byte(dat);
	SCLK=1;
	RST=0;
}
uchar read_1302(uchar add)
{
	uchar temp;
	RST=0;
	SCLK=0;
	RST=1;
	write_byte(add);
	temp=read_byte();
	SCLK=1;
	RST=0;
	return(temp);
}
uchar BCD_Decimal(uchar bcd)   //BCD码转化为十位数
{
 uchar Decimal;
 Decimal=bcd>>4;
 return(Decimal=Decimal*10+(bcd&=0x0F));
}
void ds1302_init()
{
	RST=0;
	SCLK=0;
	write_1302(0x80,miao|0x00);//允许写
	write_1302(0x8e,0x00);

	miao= (tab2[6]-0x30)*10+(tab2[7]-0x30);
	temp=(miao)/10*16+(miao)%10;
	write_1302(0x80,0x00|temp);

	fen= (tab2[3]-0x30)*10+(tab2[4]-0x30);
	temp=(fen)/10*16+(fen)%10;
	write_1302(0x82,0x00|temp);

	shi= (tab2[0]-0x30)*10+(tab2[1]-0x30);
	temp=(shi)/10*16+(shi)%10;
	write_1302(0x84,0x00|temp);

	ri= (tab1[8]-0x30)*10+(tab1[9]-0x30);
	temp=(ri)/10*16+(ri)%10;
	write_1302(0x86,0x00|temp);

	yue= (tab1[5]-0x30)*10+(tab1[6]-0x30);
	temp=(yue)/10*16+(yue)%10;
	write_1302(0x88,0x00|temp);

	write_1302(0x8e,0x80);  //写完数据后关闭，为了保护
}

void keyscan()
{
   key=0;
	if(key1==0)//key1为功能键
	{
	delay(5);
	if(key1==0)
	{
	while(!key1);
	key1n++;
	if(key1n==9)
		key1n=1;
	switch(key1n)
	{

	case 1: TR0=0;//关闭定时器
			//TR1=0;
			write_1602com(er+0x0b);//写入光标位置
		   	write_1602com(0x0f);//设置光标为闪烁
		   	temp=(miao)/10*16+(miao)%10;
		   	write_1302(0x8e,0x00);
		   	write_1302(0x80,0x80|temp);//miao
		   	write_1302(0x8e,0x80);
		   	break;
	case 2:  write_1602com(er+8);//fen

		     //write_1602com(0x0f);
			break;
	case 3: write_1602com(er+5);//shi
		     //write_1602com(0x0f);
			break;
	case 4: write_1602com(yh+0x0f);//week
		     //write_1602com(0x0f);
			break;
	case 5: write_1602com(yh+0x0b);//ri
		     //write_1602com(0x0f);
			break;
	case 6: write_1602com(yh+0x08);//yue
		     //write_1602com(0x0f);
			break;
	case 7: write_1602com(yh+0x05);//nian
		     //write_1602com(0x0f);
			break;
	case 8:
			write_1602com(0x0c);//设置光标不闪烁
			TR0=1;//打开定时器
			temp=(miao)/10*16+(miao)%10;
		   	write_1302(0x8e,0x00);
		   	write_1302(0x80,0x00|temp);//miao
		   	write_1302(0x8e,0x80);
            break;
	}
	}
	}
	if(key1n!=0)//当key1按下以下。再按以下键才有效
	{

	if(key2==0)  //上调键
	{
	delay(5);
	if(key2==0)
	{
	while(!key2);
	switch(key1n)
	{
	case 1:miao++;
			if(miao==60)
				miao=0;
			write_sfm(0x0a,miao);//写入新的秒数
			temp=(miao)/10*16+(miao)%10;
		   	write_1302(0x8e,0x00);
		   	write_1302(0x80,0x80|temp);
		   	write_1302(0x8e,0x80);
			write_1602com(er+0x0b);//因为设置液晶的模式是写入数据后，指针自动加一，在这里是写回原来的位置
			//write_1602com(0x0b);
			break;
	case 2:fen++;
			if(fen==60)
				fen=0;
			write_sfm(0x07,fen);
			temp=(fen)/10*16+(fen)%10;
		   	write_1302(0x8e,0x00);
		   	write_1302(0x82,temp);
		   	write_1302(0x8e,0x80);
			write_1602com(er+8);
			break;
	case 3:shi++;
			if(shi==24)
				shi=0;
			write_sfm(4,shi);
			temp=(shi)/10*16+(shi)%10;
		   	write_1302(0x8e,0x00);
		   	write_1302(0x84,temp);
		   	write_1302(0x8e,0x80);
			write_1602com(er+5);
			break;
	case 4:week++;
			if(week==8)
				week=0;
				write_week(week);

			write_1602com(yh+0x0f);
			break;
	case 5:ri++;
			if(ri==31)
				ri=0;
			write_nyr(10,ri);
			temp=(ri)/10*16+(ri)%10;
		   	write_1302(0x8e,0x00);  //关闭
		   	write_1302(0x86,temp);
		   	write_1302(0x8e,0x80);  //开启
			write_1602com(yh+11);
			break;
	case 6:yue++;
			if(yue==13)
				yue=0;
			write_nyr(7,yue);
			temp=(yue)/10*16+(yue)%10;
		   	write_1302(0x8e,0x00);
		   	write_1302(0x88,temp);
		   	write_1302(0x8e,0x80);
			write_1602com(yh+8);
			break;
	case 7:nian++;
		 if(nian==100)
			nian=0;
			write_nyr(4,nian);
			write_1602com(yh+5);
			break;


	}

	}

	}
	if(key3==0)
	{
	delay(5);
	if(key3==0)
	{
	while(!key3);
	switch(key1n)
	{
		case 1:miao--;
				if(miao==-1)
					miao=59;
			write_sfm(0x0a,miao);//写入新的秒数
			write_1602com(er+0x0b);//因为设置液晶的模式是写入数据后，指针自动加一，在这里是写回原来的位置
			//write_1602com(0x0b);
			break;
		case 2:fen--;
				if(fen==-1)
					fen=59;
			write_sfm(7,fen);
			temp=(fen)/10*16+(fen)%10;
		   	write_1302(0x8e,0x00);
		   	write_1302(0x82,temp);
		   	write_1302(0x8e,0x80);
			write_1602com(er+8);
			break;

		case 3:shi--;
			   if(shi==-1)
					shi=23;
			write_sfm(4,shi);
			temp=(shi)/10*16+(shi)%10;
		   	write_1302(0x8e,0x00);
		   	write_1302(0x84,temp);
		   	write_1302(0x8e,0x80);
			write_1602com(er+5);
			break;
		case 4:week--;
				if(week==-1)
					week=7;
			write_week(week);
			write_1602com(yh+0x0f);
			break;
		case 5:ri--;
				if(ri==-1)
					ri=30;
			write_nyr(10,ri);
			temp=(ri)/10*16+(ri)%10;
		   	write_1302(0x8e,0x00);
		   	write_1302(0x86,temp);
		   	write_1302(0x8e,0x80);
			write_1602com(yh+11);
			break;
		case 6:yue--;
				if(yue==-1)
					yue=12;
			write_nyr(7,yue);
			temp=(yue)/10*16+(yue)%10;
		   	write_1302(0x8e,0x00);
		   	write_1302(0x88,temp);
		   	write_1302(0x8e,0x80);
			write_1602com(yh+8);
			break;
		case 7:nian--;
		 if(nian==-1)
			nian=99;
			write_nyr(4,nian);
			write_1602com(yh+5);
			break;


	}

	}

	}

	}



}
void init()
{
	TMOD=0x11;
	TH0=0;
	TL0=0;
	EA=1;
	ET0=1;
	TR0=1;



}

void cmg(void)//数码管锁存函数
{
dula=1;
P0=0x00;
dula=0;
wela=1;
P0=0x00;
wela=0;
}
void main()
{

   cmg();//数码管锁存
	lcd_init();
	ds1302_init();
	init();
	while(1)
	{
		keyscan();
    }
}
void timer0() interrupt 1    //和数码管显示的DS1302_GetTime（）的函数相同
{
	miao = BCD_Decimal(read_1302(0x81));
	fen = BCD_Decimal(read_1302(0x83));
	shi  = BCD_Decimal(read_1302(0x85));
	ri  = BCD_Decimal(read_1302(0x87));
	yue = BCD_Decimal(read_1302(0x89));
	nian=BCD_Decimal(read_1302(0x8d));
	week=BCD_Decimal(read_1302(0x8b));
	write_sfm(10,miao);
	write_sfm(7,fen);
	write_sfm(4,shi);
	write_nyr(10,ri);
	write_week(week);
    write_nyr(7,yue);
	write_nyr(4,nian);
	if((fen == 0) && (miao == 0))
	{
		 FM = 0;
	     delay(100);
	     FM = 1;
	}
}

