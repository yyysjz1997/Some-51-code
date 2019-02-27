#include<reg52.h> 

sbit IN1=P1^2; // 高电平1 后退（反转）
sbit IN2=P1^3; // 高电平1 前进（正转）
sbit IN3=P1^6; // 高电平1 前进（正转）	
sbit IN4=P1^7; // 高电平1 后退（反转）
sbit EN1=P1^4; // 高电平使能 
sbit EN2=P1^5; // 高电平使能 

void run(void)			  //前进函数
{
	EN1=1;
	EN2=1;//电机使能
    IN1=0;
	IN2=1; //左电机的正转
	IN3=1;
	IN4=0;//右电机的正转
}

void back(void)			  //后退函数
{
	EN1=1;
	EN2=1;//电机使能
    IN1=1;
	IN2=0; //左电机的反转
	IN3=0;
	IN4=1;//右电机的反转
}

void right(void)		 //右转函数
{
	EN1=1;
	EN2=1;//电机使能
    IN1=0;
	IN2=1; //左电机的正转
	IN3=0;
	IN4=0;//右电机不动
}

void left(void)			  //左转函数
{
	EN1=1;
	EN2=1;//电机使能
    IN1=0;
	IN2=0; //左电机不动
	IN3=1;
	IN4=0;//右电机正转
}

void stop(void)			  //停止函数
{
	EN1=1;
	EN2=1;//电机使能
    IN1=0;
	IN2=0; //左电机不动
	IN3=0;
	IN4=0;//右电机正转
}

void main()
{
  while(1)
  {
  		right();
  }
}












