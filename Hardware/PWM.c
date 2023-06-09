#include "stm32f10x.h"                  // Device header
#include "LED.h"
void PWM_Init(void)
	
{	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);//开启RCC时钟，整个内外时钟都被打开
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);

	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;//选择复用推挽输出
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_11;		//GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	
	TIM_InternalClockConfig(TIM1);//选择内部时钟源时钟
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;//不分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up;//向上计数
	TIM_TimeBaseInitStructure.TIM_Period=20000-1;//ARR自动重装值
	TIM_TimeBaseInitStructure.TIM_Prescaler=72-1;//PSC预分频值
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter=0;//重复计数的值(高级TIM才有)
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseInitStructure);//时基单元初始化
	
	
	
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);//给结构体赋初值，否则未用到赋初值的没有值，比如导致高级PWM不正常
	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse=0;//CRR
	TIM_OC4Init(TIM1,&TIM_OCInitStructure);
	
	TIM_Cmd(TIM1,ENABLE);//TIM1控制使能
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
}

void PWM_SetCompare4(uint16_t Compare)
{
	TIM_SetCompare4(TIM1,Compare);
}
