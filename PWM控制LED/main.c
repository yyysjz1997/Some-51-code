
#include<reg52.h> 





sbit LED0=P1^0;// 用sbit 关键字 定义 LED到P1.0端口，LED是自己任意定义且容易记忆的符号


void Delay(unsigned int t); //函数声明
/*------------------------------------------------
                    主函数
------------------------------------------------*/
void main (void)
{
                  
unsigned int CYCLE=600,PWM_LOW=0;//定义周期并赋值




	
while (1)         //主循环
  {
 LED0=1;
 Delay(60000);        //特意加延时，可以看到熄灭的过程
 for(PWM_LOW=1;PWM_LOW<CYCLE;PWM_LOW++){ //PWM_LOW表示低
 //电平时间，这个循环中低电平时长从1累加到CYCLE（周期）的值，即600次

   		LED0=0;        //点亮LED  
   		Delay(PWM_LOW);//延时长度，600次循环中从1加至599
   		LED0=1;        //熄灭LED
  	    Delay(CYCLE-PWM_LOW);//延时长度，600次循环中从599减至1
     
   }
 LED0=0;
 for(PWM_LOW=CYCLE-1;PWM_LOW>0;PWM_LOW--){ //与逐渐变亮相反的过程

   		LED0=0;
   		Delay(PWM_LOW);
   		LED0=1;
  	    Delay(CYCLE-PWM_LOW);
     
   }
                     //主循环中添加其他需要一直工作的程序
  }
}

void Delay(unsigned int t)
{
 while(--t);
}