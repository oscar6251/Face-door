#include "KEY1.h"
#include "delay.h"
#include "sys.h"
#include "usart3.h"
#include <stdio.h>
unsigned char Y1,Y2,Y3,Y4;
extern uchar KeyVal;
extern uint8_t MEAU;

void Matrix_Key_Init(void)
{
   GPIO_InitTypeDef GPIO_InitStructure;   

	
   RCC_APB2PeriphClockCmd(X1_RCC|X2_RCC|X3_RCC|X4_RCC|Y1_RCC|Y2_RCC|Y3_RCC|Y4_RCC, ENABLE);
	
   GPIO_InitStructure.GPIO_Pin =  X1_GPIO_PIN ;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	 
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_Init(X1_GPIO_PORT, &GPIO_InitStructure);
    
   GPIO_InitStructure.GPIO_Pin =  X2_GPIO_PIN ;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	 
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_Init(X2_GPIO_PORT, &GPIO_InitStructure);
    
   GPIO_InitStructure.GPIO_Pin =  X3_GPIO_PIN ;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	 
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_Init(X3_GPIO_PORT, &GPIO_InitStructure);
	
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	 
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Pin = X4_GPIO_PIN ;    
   GPIO_Init(X4_GPIO_PORT, &GPIO_InitStructure);
   
   GPIO_InitStructure.GPIO_Pin =  Y1_GPIO_PIN ;   
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;	 
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_Init(Y1_GPIO_PORT, &GPIO_InitStructure);	
   
   GPIO_InitStructure.GPIO_Pin =  Y2_GPIO_PIN ;   
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;	 
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_Init(Y2_GPIO_PORT, &GPIO_InitStructure);	
   
   GPIO_InitStructure.GPIO_Pin =  Y3_GPIO_PIN ;   
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;	 
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_Init(Y3_GPIO_PORT, &GPIO_InitStructure);	
	
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;	 
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Pin = Y4_GPIO_PIN;      
   GPIO_Init(Y4_GPIO_PORT, &GPIO_InitStructure);	
	 
	 GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_13 ;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	 
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_Init(GPIOF, &GPIO_InitStructure);
}

int Matrix_Key_Scan(void)
{
	static char key_down = 0;//防止一直按键按着，避免死循环
	uchar KeyVal = KEY_ERR;
	GPIO_SetBits(X1_GPIO_PORT,X1_GPIO_PIN);  
	GPIO_SetBits(X2_GPIO_PORT,X2_GPIO_PIN);  
	GPIO_SetBits(X3_GPIO_PORT,X3_GPIO_PIN);  
	GPIO_SetBits(X4_GPIO_PORT,X4_GPIO_PIN);  

	if ((GPIO_ReadInputDataBit(Y1_GPIO_PORT,Y1_GPIO_PIN)==1) || (GPIO_ReadInputDataBit(Y1_GPIO_PORT,Y2_GPIO_PIN)==1) || (GPIO_ReadInputDataBit(Y1_GPIO_PORT,Y3_GPIO_PIN)==1) || (GPIO_ReadInputDataBit(Y1_GPIO_PORT,Y4_GPIO_PIN)==1))//如果有按键按下
	{
			Delay_ms(20);
		if ((GPIO_ReadInputDataBit(Y1_GPIO_PORT,Y1_GPIO_PIN)==1) || (GPIO_ReadInputDataBit(Y1_GPIO_PORT,Y2_GPIO_PIN)==1) || (GPIO_ReadInputDataBit(Y1_GPIO_PORT,Y3_GPIO_PIN)==1) || (GPIO_ReadInputDataBit(Y1_GPIO_PORT,Y4_GPIO_PIN)==1))//10ms抖动
		{
			if (key_down == 0)
			{	
				key_down = 1;		
				
				GPIO_SetBits(X1_GPIO_PORT,X1_GPIO_PIN);
				GPIO_ResetBits(X2_GPIO_PORT,X2_GPIO_PIN);
				GPIO_ResetBits(X3_GPIO_PORT,X3_GPIO_PIN);
				GPIO_ResetBits(X4_GPIO_PORT,X4_GPIO_PIN);
					 
				Y1=GPIO_ReadInputDataBit(Y1_GPIO_PORT,Y1_GPIO_PIN);
				Y2=GPIO_ReadInputDataBit(Y2_GPIO_PORT,Y2_GPIO_PIN);
				Y3=GPIO_ReadInputDataBit(Y3_GPIO_PORT,Y3_GPIO_PIN);
				Y4=GPIO_ReadInputDataBit(Y4_GPIO_PORT,Y4_GPIO_PIN);
				
				if(Y1==1&&Y2==0&&Y3==0&&Y4==0)
					KeyVal='A';
				if(Y1==0&&Y2==1&&Y3==0&&Y4==0)
					KeyVal='B';
				if(Y1==0&&Y2==0&&Y3==1&&Y4==0)
					KeyVal='C';
				if(Y1==0&&Y2==0&&Y3==0&&Y4==1)
					KeyVal='D';
/**************************************************/		
				
				GPIO_ResetBits(X1_GPIO_PORT,X1_GPIO_PIN);
				GPIO_SetBits(X2_GPIO_PORT,X2_GPIO_PIN);
				GPIO_ResetBits(X3_GPIO_PORT,X3_GPIO_PIN);
				GPIO_ResetBits(X4_GPIO_PORT,X4_GPIO_PIN);
								
				Y1=GPIO_ReadInputDataBit(Y1_GPIO_PORT,Y1_GPIO_PIN);
				Y2=GPIO_ReadInputDataBit(Y2_GPIO_PORT,Y2_GPIO_PIN);
				Y3=GPIO_ReadInputDataBit(Y3_GPIO_PORT,Y3_GPIO_PIN);
				Y4=GPIO_ReadInputDataBit(Y4_GPIO_PORT,Y4_GPIO_PIN);
				
				if(Y1==1&&Y2==0&&Y3==0&&Y4==0)
					KeyVal=3;
				if(Y1==0&&Y2==1&&Y3==0&&Y4==0)
					KeyVal=6;
				if(Y1==0&&Y2==0&&Y3==1&&Y4==0)
					KeyVal=9;
				if(Y1==0&&Y2==0&&Y3==0&&Y4==1)
					KeyVal='#';
				
/**************************************************/
				
				GPIO_ResetBits(X1_GPIO_PORT,X1_GPIO_PIN);
				GPIO_ResetBits(X2_GPIO_PORT,X2_GPIO_PIN);
				GPIO_SetBits(X3_GPIO_PORT,X3_GPIO_PIN);
				GPIO_ResetBits(X4_GPIO_PORT,X4_GPIO_PIN);   

				Y1=GPIO_ReadInputDataBit(Y1_GPIO_PORT,Y1_GPIO_PIN);
				Y2=GPIO_ReadInputDataBit(Y2_GPIO_PORT,Y2_GPIO_PIN);
				Y3=GPIO_ReadInputDataBit(Y3_GPIO_PORT,Y3_GPIO_PIN);
				Y4=GPIO_ReadInputDataBit(Y4_GPIO_PORT,Y4_GPIO_PIN);
				
				if(Y1==1&&Y2==0&&Y3==0&&Y4==0)
					KeyVal=2;
				if(Y1==0&&Y2==1&&Y3==0&&Y4==0)
					KeyVal=5;
				if(Y1==0&&Y2==0&&Y3==1&&Y4==0)
					KeyVal=8;
				if(Y1==0&&Y2==0&&Y3==0&&Y4==1)
					KeyVal=0;	
				
/**************************************************/
				
				GPIO_ResetBits(X1_GPIO_PORT,X1_GPIO_PIN);
				GPIO_ResetBits(X2_GPIO_PORT,X2_GPIO_PIN); 
				GPIO_ResetBits(X3_GPIO_PORT,X3_GPIO_PIN); 
				GPIO_SetBits(X4_GPIO_PORT,X4_GPIO_PIN); 
				
				Y1=GPIO_ReadInputDataBit(Y1_GPIO_PORT,Y1_GPIO_PIN);
				Y2=GPIO_ReadInputDataBit(Y2_GPIO_PORT,Y2_GPIO_PIN);
				Y3=GPIO_ReadInputDataBit(Y3_GPIO_PORT,Y3_GPIO_PIN);
				Y4=GPIO_ReadInputDataBit(Y4_GPIO_PORT,Y4_GPIO_PIN);
				
				if(Y1==1&&Y2==0&&Y3==0&&Y4==0)
					KeyVal=1;
				if(Y1==0&&Y2==1&&Y3==0&&Y4==0)
					KeyVal=4;
				if(Y1==0&&Y2==0&&Y3==1&&Y4==0)
					KeyVal=7;
				if(Y1==0&&Y2==0&&Y3==0&&Y4==1)
					KeyVal='*';
			}
		}
	}
	else
	{
		key_down = 0;
		KeyVal = KEY_ERR;		//无按键按下返回 KEY_ERR  255
	}
	
	return KeyVal;
}



