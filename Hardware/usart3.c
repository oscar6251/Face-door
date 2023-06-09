#include "Delay.h"
#include "usart3.h"
#include <stdio.h>
#include <stdarg.h>	 	 
#include "string.h"	 
#include "timer.h"
#include "LED.h" 
#include "OLED.h"
#include "PWM.h"
#include "SERVO.h"


//串口接收缓存区 	
u8 USART3_RX_BUF[USART3_MAX_RECV_LEN]; 				//接收缓冲,最大USART3_MAX_RECV_LEN个字节.
u8 USART3_TX_BUF[USART3_MAX_SEND_LEN]; 			  //发送缓冲,最大USART3_MAX_SEND_LEN字节

//通过判断接收连续2个字符之间的时间差不大于10ms来决定是不是一次连续的数据.
//如果2个字符接收间隔超过10ms,则认为不是1次连续数据.也就是超过10ms没有接收到
//任何数据,则表示此次接收完毕.
//接收到的数据状态
//[15]:0,没有接收到数据;1,接收到了一批数据.
//[14:0]:接收到的数据长度
vu16 USART3_RX_STA=0;   	




//初始化IO 串口2
//pclk1:PCLK1时钟频率(Mhz)
//bound:波特率	  
void usart3_init(u32 bound)
{  

	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	// GPIOA时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE); //串口2时钟使能

		 //USART3_TX   PA2
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA2
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化PA2
   
    //USART3_RX	  PA3
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//浮空输入
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);  //初始化PA3
	
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = bound;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
  
	USART_Init(USART2, &USART_InitStructure); //初始化串口2
  	
	//使能接收中断
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启中断   
	
	//设置中断优先级
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	
	USART_Cmd(USART2, ENABLE);                    //使能串口 
	
	
	TIM2_Int_Init(99,7199);		//10ms中断
	USART3_RX_STA=0;		//清零
	TIM_Cmd(TIM2,DISABLE);			//关闭定时器2

}


void Serial_SendByte1(uint8_t Byte)
{
	USART_SendData(USART2, Byte);
	while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
}

int fputc(int ch, FILE *f)	//单个字符发出
{
	Serial_SendByte1(ch);
	return ch;
}


void USART2_IRQHandler(void)
{


	u8 res;	//接收一字节数据      
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)//接收到数据
	{	
		
		res =USART_ReceiveData(USART2);		 
		if((USART3_RX_STA&(1<<15))==0)//接收完的一批数据,还没有被处理,则不再接收其他数据
		{ 
			if(USART3_RX_STA<USART3_MAX_RECV_LEN)	//还可以接收数据
			{
				TIM_SetCounter(TIM2,0);//每接收一字节数据，计数器清空 为了不超时 不进入定时器10ms中断      //计数器清空
				if(USART3_RX_STA==0) 	//可以理解：每次收下一个包的时候，使能定时器7的中断 
				{
					TIM_Cmd(TIM2,ENABLE);//使能定时器7
				}
				USART3_RX_BUF[USART3_RX_STA++]=res;	//记录接收到的值	 
			}
			else 
			{
				USART3_RX_STA|=1<<15;				//强制标记接收完成
			} 
		}
		// 判断接收到的数据是否为"ffabcdfe"
		if (USART3_RX_STA >= 8)
		{
				if (USART3_RX_BUF[0] == 0xFF && USART3_RX_BUF[1] == 0xAB && USART3_RX_BUF[2] == 0xCD && USART3_RX_BUF[3] == 0xFE)
				{
						// 调用舵机控制函数
						Servo_SetAngle(180);
						Delay_ms(5000);
						Servo_SetAngle(0);
				}
				
				// 清空接收缓冲区
				USART3_RX_STA = 0;
		}
	}  				 											 
}   

















