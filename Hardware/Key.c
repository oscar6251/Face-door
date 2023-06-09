#include "stm32f10x.h"                  // Device header
#include "Delay.h"



void Key_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB
	|RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);	//GPIOB
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);	//GPIOC
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	//GPIOA

}

uint8_t Key_GetNum(void)
{
	uint8_t KeyNum;
	
	if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 0)
	{

		KeyNum = 1;
	}
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11) == 0)
	{

		KeyNum = 3;
	}
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0)
	{

		KeyNum = 2; 
	}
	if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13) == 0)
	{

		KeyNum = 4;
	}
	if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_15) == 0)
	{

		KeyNum = 5;
	}
	
	return KeyNum;
}
