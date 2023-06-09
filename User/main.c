#include "stm32f10x.h"                  
#include "Delay.h"
#include "oled_iic.h"
#include "LED.h"
#include "rc522.h"
#include "Serial.h"
#include "timer.h"
#include "usart3.h"
#include "Key1.h"
#include <stdio.h>
#include "keyCode.h"
#include "flash.h"
#include "SERVO.h"
#include "Beep.h"



uint8_t MEAU=0;	//菜单切换：0是主菜单。2：蓝牙/3：刷卡/4：密码


unsigned char Status;
unsigned char CardType[2];//卡类型 
unsigned char Id[4];    //FRID识别出4位UID卡号，放在Id数组里
 
//  录入卡的UID,用于判断门开启关闭
unsigned char Card_0[4]= {0x97,0x2D,0xC0,0xA7};//白卡	 正确：0x97,0x2D,0xC0,0xA7
unsigned char Card_1[4]= {0x19,0x18,0x91,0x98};//蓝卡  正确：0x19,0x18,0x91,0x98
//void RC522_Test1(void);

//  蓝牙串口接收数据
uint8_t RxData;
unsigned char Blu[2]= {0xab,0xcd};//蓝牙密码
void BluetoothTest2(void);

//RC522测试函数
 void RC522_Test1(void);
	 
//主菜单
void ShowMeau(void);

int main(void)
{
	OLED_Init();	//OLED初始化
	LED_Init();		//LED初始化
	Beep_Init();	//蜂鸣器初始化

	Serial_Init();	//串口1初始化蓝牙
	

	RC522_Init();  //初始化射频卡模块
  usart3_init(9600);//串口2接收初始化
	
/********汉字库初始化程序***********/
	
	extern const u8 BMP1[];
	HZ= GB16_NUM();//汉字数量
	
/********主菜单初始化程序***********/	
	ShowMeau();
   
/********矩阵键盘初始化程序***********/
	Matrix_Key_Init();
	
	
/********上电读取密码锁初始化程序***********/
	KeyCode_Init();
	
/********舵机初始化***********/	
	Servo_Init();
	
	while(1)
  {

		KeyVal=Matrix_Key_Scan();

		if(MEAU==0)
		{	
		  if(KeyVal==1)	//进入蓝牙
		  {
				OLED_Clear();
				OLED_ShowCH(2,0,(u8*)"app发送密码");  //ffabcdfe
				MEAU=2;
		  }
		  if(KeyVal==3)	//进入RFID
		  {
				OLED_Clear();
				OLED_ShowCH(32,0,(u8*)"请刷卡");  
				MEAU=3;
		  }
		  if(KeyVal==6)	//进入密码锁
		  {
				OLED_Clear();
				OLED_ShowCH(0,0,(u8*)"C 请输入密码");
				OLED_ShowCH(0,2,(u8*)"B 确认密码 ");	
				MEAU=4;
		  }
										

		}		
		
		if(MEAU==2)//蓝牙操作
	  {			
		   BluetoothTest2();	
		}
		 
		if(MEAU==3)//RFID操作 
	  {		
			RC522_Test1();	
		}
		
	  if(MEAU==4)//密码锁操作
	  {							
			KeyVal=KEY_ERR;
			KeyVal=Matrix_Key_Scan();  
			data_key();
			KeyVal=Matrix_Key_Scan();
	  }
		
		//接收人脸识别成功发送的数据
		BluetoothTest2();
	}

}
	



//功能：门禁模块

 void RC522_Test1()
{

    Status = PcdRequest(PICC_REQALL,CardType);//寻卡
	
    if(Status==MI_OK)// 寻卡成功
    {
        Status=MI_ERR;
				Status = PcdAnticoll(Id);// 防冲撞 获得UID 存入Id
    }

    int isExecuted = 0; // 初始化标志变量，0 表示 false

		if (Status == MI_OK) // 防冲撞成功
		{
				Status = MI_ERR;
				ShowID(Id); // 串口打印陌生卡的ID号 UID
				//录入的卡核对成功，便可以使舵机旋转
				if ((Id[0] == Card_0[0]) && (Id[1] == Card_0[1]) && (Id[2] == Card_0[2]) && (Id[3] == Card_0[3]))
				{
						OLED_Clear();
						OLED_ShowCH(32, 0, (u8 *)"刷卡成功");
						Servo_SetAngle(180);
						LED2_ON();
						Delay_ms(5000);
						Servo_SetAngle(0);
						LED2_OFF();
						isExecuted = 1; // 执行了第一个条件(第一张录入的卡匹配成功)
				}

				if (!isExecuted && (Id[0] == Card_1[0]) && (Id[1] == Card_1[1]) && (Id[2] == Card_1[2]) && (Id[3] == Card_1[3]))
				{
						OLED_Clear();
						OLED_ShowCH(32, 0, (u8 *)"刷卡成功");
						Servo_SetAngle(180);
						LED2_ON();
						Delay_ms(5000);
						Servo_SetAngle(0);
						LED2_OFF();
						isExecuted = 1; // 执行了第二个条件(第二张录入的卡匹配成功)
				}

				if (!isExecuted)
				{
						OLED_Clear();
						OLED_ShowCH(32, 0, (u8 *)"刷卡失败");
						LED1_ON();
						Beep_ON();
						Delay_ms(1500);
						LED1_OFF();
						Beep_OFF();
				}

				MEAU = 0;
				ShowMeau();
		}


		if(KeyVal=='*')	//返回按键
	  {
			OLED_Clear();
			OLED_ShowCH(0,0,(u8*)"K1 蓝牙  ");
			OLED_ShowCH(0,2,(u8*)"K3 刷卡  ");	
			OLED_ShowCH(0,4,(u8*)"K6 密码  ");
			//OLED_ShowCH(0,6,(u8*)"* 返回	 ");
			MEAU=0;
	  }

}


//功能：蓝牙模块收数据显示到OLED

void BluetoothTest2()
{
	
	uint8_t i;
	if (Serial_GetRxFlag() == 1)//收到数据，数据全部传到全局变量Serial_RxPacket[4]里面再显示，避免取得数据不对
	 {	
			//LED2_Turn();
			if((Serial_RxPacket[0]==Blu[0])&&(Serial_RxPacket[1]==Blu[1]))	//app输入的密码正确
				{
					OLED_Clear();
					OLED_ShowCH(0,0,(u8*)"成功"); 

					LED2_ON();
					Servo_SetAngle(180);
					Delay_ms(5000);
					Servo_SetAngle(0);
					MEAU=0;
					ShowMeau();
				}
			else
				{
					OLED_Clear();
					OLED_ShowCH(0,0,(u8*)"错误");
					LED1_ON();
					Beep_ON();
					Delay_ms(1500);
					LED1_OFF();
					Beep_OFF();
					MEAU=0;
					ShowMeau();
				}
				
				
		}
		if(KeyVal=='*')	//返回按键，一级一级
		{
				OLED_Clear();
				OLED_ShowCH(0,0,(u8*)"K1 蓝牙  ");
				OLED_ShowCH(0,2,(u8*)"K3 刷卡  ");	
				OLED_ShowCH(0,4,(u8*)"K6 密码  ");
				//OLED_ShowCH(0,6,(u8*)"* 返回	 ");
				MEAU=0;
		}
		else	
		{
				LED2_OFF();
		}	
				
		
}




/********主菜单*********/
void ShowMeau()
{
		OLED_Clear();
		OLED_ShowCH(0,0,(u8*)"K1 蓝牙  ");
		OLED_ShowCH(0,2,(u8*)"K3 刷卡  ");	
		OLED_ShowCH(0,4,(u8*)"K6 密码  ");
		//OLED_ShowCH(0,6,(u8*)"* 返回	 ");
}
	

