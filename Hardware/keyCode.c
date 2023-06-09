#include "sys.h"
#include "oled_iic.h"
#include "flash.h"
#include "KEY1.h"
#include "Delay.h"
#include "SERVO.h"
#include "LED.h"
#include "Beep.h"

unsigned char Password2[7] = {1, 1, 1, 1, 1, 1, 0};
unsigned char Password[6] = {1,1,1,1,1,1};//存放临时按键按下的密码
#define SIZE sizeof(Password2)	   //数组长度
#define FLASH_SAVE_ADDR 0X0800FF40 //原储存密码的地址0X08070000，移植C8T6后0X0800FF40
#define FLASH_SAVE_ADDR1 0X08020000
u8 flag = 0;
unsigned char Password1[7] = {1, 1, 2, 2, 3, 3, 0};//设置的是修改后密码放入在这个数组中，然后密码实际长度为6，数组最后一位是输入新密码的标志位
;
unsigned char Password_count = 0;

// unsigned char Password1[6]={0,0,0,0,0,0};
unsigned int a_c, b_c, c_c, d_c;
unsigned char KeyVal;
unsigned char flag_KeyVal = 0, flag_KeyVal1 = 0;
u16 flag_key_data = 0;
u16 *flag_key = &flag_key_data;


void data_key()
{
	volatile unsigned char flag_data_key1 = 0;
	

	if (KeyVal == 'C')	//提示输入密码键
	{
		OLED_Clear();
		OLED_ShowCH(16,0,(u8*)"请输入密码");   //OLED_ShowString(0, 8, "input:", 16);
		flag = 1;//类似状态转移
		Password_count = 0;
		flag_KeyVal = 0;
		flag_KeyVal1 = 1;
		flag_data_key1 = 1;

	}

	if (flag == 1)   //flag == 1类似状态转移
	{
		if (KeyVal > 0 && KeyVal <= 9) //输入密码，1-9
		{
			if (Password_count < 6)//Password_count密码计数，本程序6位密码
			{
				Password[Password_count] = KeyVal;//给数组放入输入的密码
				Password_count++;//可以加到6
				
				if (Password_count == 1)
				{
					OLED_ShowHexNum(16*Password_count,2,Password[Password_count-1],1);
				}
				if (Password_count == 2)
				{
					OLED_ShowHexNum(16*Password_count,2,Password[Password_count-1],1);
				}
				if (Password_count == 3)
				{
					OLED_ShowHexNum(16*Password_count,2,Password[Password_count-1],1);
				}
				if (Password_count == 4)
				{
					OLED_ShowHexNum(16*Password_count,2,Password[Password_count-1],1);
				}
				if (Password_count == 5)
				{
					OLED_ShowHexNum(16*Password_count,2,Password[Password_count-1],1);
				}
				if (Password_count == 6)
				{
					OLED_ShowHexNum(16*Password_count,2,Password[Password_count-1],1);
				}


			}
		}

		else if ((KeyVal == 'B') && (Password2[6] == 0)) //确认按键
		{
			if ((Password[0] == 1 && Password[1] == 1 && Password[2] == 1 && Password[3] == 1 && Password[4] == 1 && Password[5] == 1) || (Password[0] == Password1[0] && Password[1] == Password1[1] && Password[2] == Password1[2] && Password[3] == Password1[3] && Password[4] == Password1[4] && Password[5] == Password1[5]))
			{
				//密码正确
				OLED_Clear();
				OLED_ShowCH(16,0,(u8*)"密码正确"); 	
				flag_KeyVal1 = 0;
				flag_data_key1 = 0;
				Servo_SetAngle(180);
				LED2_ON();
				Delay_ms(5000);
				Servo_SetAngle(0);
				LED2_OFF();
				
			}
			else
			{
				OLED_Clear();
				OLED_ShowCH(16,0,(u8*)"密码错误");
				flag_KeyVal1 = 0;
				flag_data_key1 = 0;
				LED1_ON();
				Beep_ON();
				Delay_ms(1500);
				LED1_OFF();
				Beep_OFF();
				
			 }

				OLED_Clear();
				OLED_ShowCH(0,0,(u8*)"K1 蓝牙  ");
				OLED_ShowCH(0,2,(u8*)"K3 刷卡  ");	
				OLED_ShowCH(0,4,(u8*)"K6 密码  ");
				//OLED_ShowCH(0,6,(u8*)"* 返回	 ");
				MEAU=0;
			}

		
}

}

void KeyCode_Init(void)	
{
	STMFLASH_Read(FLASH_SAVE_ADDR, (u16 *)Password1, SIZE);//将FLASH_SAVE_ADDR数据读入Password1
//	STMFLASH_Write(FLASH_SAVE_ADDR,(u16 *)Password2, SIZE);//将Password2数据写入FLASH_SAVE_ADDR
}

