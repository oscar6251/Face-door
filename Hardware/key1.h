#ifndef _KEY1_H			//矩阵键盘按键  不影响主程序  有消抖
#define _KEY1_H	 

#include <stm32f10x.h>

#include "sys.h"

#define uint unsigned int 
#define uchar unsigned char

#define X1_GPIO_PORT GPIOC           
#define X2_GPIO_PORT GPIOC   
#define X3_GPIO_PORT GPIOB           
#define X4_GPIO_PORT GPIOB 

#define Y1_GPIO_PORT GPIOB         
#define Y2_GPIO_PORT GPIOB  
#define Y3_GPIO_PORT GPIOB           
#define Y4_GPIO_PORT GPIOB 

#define X1_GPIO_PIN GPIO_Pin_13
#define X2_GPIO_PIN GPIO_Pin_15
#define X3_GPIO_PIN GPIO_Pin_1
#define X4_GPIO_PIN GPIO_Pin_11

#define Y1_GPIO_PIN GPIO_Pin_15
#define Y2_GPIO_PIN GPIO_Pin_14
#define Y3_GPIO_PIN GPIO_Pin_13
#define Y4_GPIO_PIN GPIO_Pin_12

#define X1_RCC RCC_APB2Periph_GPIOC
#define X2_RCC RCC_APB2Periph_GPIOC
#define X3_RCC RCC_APB2Periph_GPIOB
#define X4_RCC RCC_APB2Periph_GPIOB

#define Y1_RCC RCC_APB2Periph_GPIOB
#define Y2_RCC RCC_APB2Periph_GPIOB
#define Y3_RCC RCC_APB2Periph_GPIOB
#define Y4_RCC RCC_APB2Periph_GPIOB


#define KEY_ERR 255

extern  unsigned char KeyVal;
void Matrix_Key_Init(void);
int Matrix_Key_Scan(void);
void Matrix_Key_Test(void) ;
extern uint8_t MEAU;

#endif
