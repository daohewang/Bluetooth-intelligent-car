#include "MOTOR.h"
#include "stm32f10x.h"         //官方库
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_it.h"
#include <stdio.h>


/*

#ifdef->endif的内容部分和PUTCHAR_PROTOTYPE{}的内容属于C语言的语法规则引入，
所以头部必须引入#include<stdio.h>,这样子的话C语言的语法就可以完美引用

*/


#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */


//中断服务函数  
u8 i;
u8 flag;  //定义一个标志位
void USART1_IRQHandler(void)  
  
{  
     if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET)      //检查指定的 USART1 中断发生与否
      { 
			USART_ClearITPendingBit(USART1, USART_IT_RXNE);   //清除 USART1 的中断待处理位
			GPIO_ResetBits(GPIOC,GPIO_Pin_6);                 //设置LED灯作为中断发生与否的指示灯
			i=  USART_ReceiveData(USART1);                    //返回 USART1 最近接收到的数据
		//if语句的内容部分可以保证蓝牙接收到的flag数据是正确的，如果不加的话会导致接收不到正确的数据
		  if(i=='0')
				{                 
					 flag=0;
				}
				
			if(i=='1')
				{
					 flag=1;
				}
				
			if(i=='2')
			   {
					 flag=2;
				}
			   
			if(i=='3')
			   {
								 
					flag=3;
			   }
			   
			if(i=='4')
			  {
					flag=4;
			  }
			  
			if(i=='5')
			  {
					flag=5;
			  }
			  
			if(i=='6')
			 {
					flag=6;
			 }

		  }
		  
//		  USART_ClearITPendingBit(USART1, USART_IT_RXNE);
      }
	  
	  
int main(void)
{
	
	//利用中断进行蓝牙的收发
	
	  GPIO_InitTypeDef GPIO_InitStructure;
	  USART_InitTypeDef USART_InitStructure;
      NVIC_InitTypeDef NVIC_InitStructure;

	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
// 	USART_DeInit(USART1);  //复位串口1
//  USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化PA9
   
    //USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //初始化PA10     
	
	 
    /* USARTx configured as follow:
       - BaudRate = 9600 baud  波特率
       - Word Length = 8 Bits  数据长度
       - One Stop Bit          停止位
       - No parity             校验方式
       - Hardware flow control disabled (RTS and CTS signals) 硬件控制流
       - Receive and transmit enabled                         使能发送和接收
    */
		USART_InitStructure.USART_BaudRate = 9600;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		USART_InitStructure.USART_StopBits = USART_StopBits_1;
		USART_InitStructure.USART_Parity = USART_Parity_No;
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

		USART_Init(USART1, &USART_InitStructure);
		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启中断

    //Usart1 NVIC 配置
      NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
      NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	  NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
      USART_Cmd(USART1, ENABLE);                    //使能串口 
	  MotorInit();
	  
	  
	  //蓝牙数据接收判断函数
	  while(1)
	  {
		
		  switch(flag)
              {
					case 0:  Stop();         break  ;
					case 1:  Turnleft();     break  ;
					case 2:  Turnright();    break  ;
					case 3:  Turnback();     break  ;
					case 4:  Turnfront();    break  ;
					case 5:  Leftaround();   break  ;
					case 6:  Rightaround();  break  ;
                    default: Stop();         break  ;                                                                                                
	          }		  
	  }
	  
}

PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
 USART_SendData(USART1, (uint8_t) ch);

  /* 循环等待直到发送结束*/
  while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
  {}

  return ch;
}
