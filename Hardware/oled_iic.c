#include "oled_iic.h"
#include "Delay.h"
#include "oledfont.h"
#include "string.h"

unsigned int HZ=0;
//返回GB16字库里汉字个数
unsigned int GB16_NUM(void)
{
  unsigned int HZ_NUM;
  unsigned char *PT;
  PT = hz_index;
  while(*PT++ != '\0')
  {
  	 HZ_NUM++;
  }

  return HZ_NUM/2;
} 

void IIC_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(OLED_SCL_RCC|OLED_SDA_RCC, ENABLE);	/* 打开GPIO时钟 */

	GPIO_InitStructure.GPIO_Pin = OLED_SCL_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;  	/* 开漏输出 */
	GPIO_Init(OLED_SCL_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = OLED_SDA_PIN;
	GPIO_Init(OLED_SDA_GPIO_PORT, &GPIO_InitStructure);

	/* 给一个停止信号, 复位IIC总线上的所有设备到待机模式 */
	IIC_Stop();
}

static void IIC_Delay(void)
{
	uint8_t i;

	/*　
	 	下面的时间是通过逻辑分析仪测试得到。
    
	*/
	for (i = 0; i < 10; i++);
}

/*
*********************************************************************************************************
*	函 数 名: IIC_Start
*	功能说明: CPU发起IIC总线启动信号
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void IIC_Start(void)
{
	/* 当SCL高电平时，SDA出现一个下跳沿表示IIC总线启动信号 */
	OLED_SDA=1;
	OLED_SCL=1;
	IIC_Delay();
	OLED_SDA=0;
	IIC_Delay();
	OLED_SCL=0;
	IIC_Delay();
}

/*
*********************************************************************************************************
*	函 数 名: IIC_Start
*	功能说明: CPU发起IIC总线停止信号
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void IIC_Stop(void)
{
	/* 当SCL高电平时，SDA出现一个上跳沿表示IIC总线停止信号 */
	OLED_SDA=0;
	OLED_SCL=1;
	IIC_Delay();
	OLED_SDA=1;
}

/*
*********************************************************************************************************
*	函 数 名: IIC_WaitAck
*	功能说明: CPU产生一个时钟，并读取器件的ACK应答信号
*	形    参：无
*	返 回 值: 返回0表示正确应答，1表示无器件响应
*********************************************************************************************************
*/
uint8_t IIC_WaitAck(void)
{
	uint8_t re;

	OLED_SDA=1;	/* CPU释放SDA总线 */
	IIC_Delay();
	OLED_SCL=1;	/* CPU驱动SCL = 1, 此时器件会返回ACK应答 */
	IIC_Delay();
	if (OLED_IIC_SDA_READ())	/* CPU读取SDA口线状态 */
	{
		re = 1;
	}
	else
	{
		re = 0;
	}
	OLED_SCL=0;
	IIC_Delay();
	return re;
}

//向IIC总线写数据
void Write_IIC_Byte(uint8_t _ucByte)
{
  uint8_t i;

	/* 先发送字节的高位bit7 */
	for (i = 0; i < 8; i++)
	{		
		if (_ucByte & 0x80)
		{
			OLED_SDA=1;
		}
		else
		{
			OLED_SDA=0;
		}
		IIC_Delay();
		OLED_SCL=1;
		IIC_Delay();	
		OLED_SCL=0;
		if (i == 7)
		{
			 OLED_SDA=1; // 释放总线
		}
		_ucByte <<= 1;	/* 左移一个bit */
		IIC_Delay();
	}
}

//写命令
void Write_IIC_Command(u8 IIC_Command)
{
	IIC_Start();
	Write_IIC_Byte(OLED_ADDRESS);//OLED地址
	IIC_WaitAck();
	Write_IIC_Byte(0x00);//寄存器地址
	IIC_WaitAck();
	Write_IIC_Byte(IIC_Command);
	IIC_WaitAck();
	IIC_Stop();
}

//写数据
void Write_IIC_Data(u8 IIC_Data)
{
	IIC_Start();
	Write_IIC_Byte(OLED_ADDRESS);//OLED地址
	IIC_WaitAck();
	Write_IIC_Byte(0x40);//寄存器地址
	IIC_WaitAck();
	Write_IIC_Byte(IIC_Data);
	IIC_WaitAck();
	IIC_Stop();
}

//OLED全屏填充
void OLED_Fill(u8 fill_Data)
{
	u8 m,n;
	for(m=0;m<8;m++)
	{
		Write_IIC_Command(0xb0+m);		//page0-page1
		Write_IIC_Command(0x00);		//low column start address
		Write_IIC_Command(0x10);		//high column start address
		for(n=0;n<130;n++)
		{
			Write_IIC_Data(fill_Data);
		}
	}
}

//设置起始点坐标
void OLED_Set_Pos(u8 x, u8 y) 
{ 
	Write_IIC_Command(0xb0+y);
	Write_IIC_Command((((x+2)&0xf0)>>4)|0x10);
	Write_IIC_Command(((x+2)&0x0f)|0x01);
}

//将OLED从休眠中唤醒
void OLED_ON(void)
{
	Write_IIC_Command(0X8D);  //设置电荷泵
	Write_IIC_Command(0X14);  //开启电荷泵
	Write_IIC_Command(0XAF);  //OLED唤醒
}

//让OLED休眠 -- 休眠模式下,OLED功耗不到10uA
void OLED_OFF(void)
{
	Write_IIC_Command(0X8D);  //设置电荷泵
	Write_IIC_Command(0X10);  //关闭电荷泵
	Write_IIC_Command(0XAE);  //OLED休眠
}

//清屏
void OLED_Clear(void)
{
	OLED_Fill(0x00);
}

//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
void OLED_ShowChar(u8 x, u8 y, u8 chr)
{
  u8 c = 0, i = 0;
  c = chr - ' '; //得到偏移后的值
  if(x > 130 - 1)
  {
    x = 0;
    y = y + 2;
  }
	OLED_Set_Pos(x, y);
	for(i = 0; i < 8; i++)
		Write_IIC_Data(zf[c * 16 + i]);
	OLED_Set_Pos(x, y + 1);
	for(i = 0; i < 8; i++)
		Write_IIC_Data(zf[c * 16 + i + 8]);

}
//m^n函数
u32 oled_pow(u8 m, u8 n)
{
  u32 result = 1;
  while(n--)result *= m;
  return result;
}
//显示个数字 十进制的
//x,y :起点坐标
//len :数字的位数
//num:数值(0~4294967295);
//mode:   为1:显示0   为0:显示空格
void OLED_ShowNum(u8 x, u8 y, u32 num, u8 len,u8 mode)
{
  u8 t, temp;
  for(t = 0; t < len; t++)
  {
    temp = (num / oled_pow(10, len - t - 1)) % 10;
		if(temp == 0)
		{
			if(mode)
				OLED_ShowChar(x + 8*t, y, '0');
			else
				OLED_ShowChar(x + 8*t, y, ' ');
			continue;
		}
		else
			OLED_ShowChar(x + 8*t, y, temp + '0');
  }
}

//显示个数字 16进制的
//x,y :起点坐标
//len :数字的位数
//num:数值(0~FFFFFFFFF);

void OLED_ShowHexNum(u8 x, u8 y, u32 num, u8 len)
{
  u8 t, temp;
  for(t = 0; t < len; t++)
  {
    temp = num / oled_pow(16, len - t - 1) % 16;
		if(temp<10)
		{
		  OLED_ShowChar(x + 8*t, y, temp + '0');
		}
		else
		{ 
		  OLED_ShowChar(x + 8*t, y,temp- 10 + 'A');
		}
  }
}


//显示中英文字符
void OLED_ShowCH(u8 x,u8 y,u8 *chs)
{
  u32 i=0;
	u32 j;
	char* m;
	while (*chs != '\0')
	{
		if (*chs>0xa0)				//汉字内码都是大于0xa0
		{
			for (i=0 ;i < HZ;i++)
			{	
				if(x>112)
				{
					x=0;
					y=y+2;
				}
				if ((*chs == hz_index[i*2]) && (*(chs+1) == hz_index[i*2+1]))
				{
					OLED_Set_Pos(x, y);
					for(j=0;j<16;j++)
						Write_IIC_Data(hz[i*32+j]);
					OLED_Set_Pos(x,y+1);
					for(j=0;j<16;j++)
						Write_IIC_Data(hz[i*32+j+16]);
					x +=16;
					break;
				}
			}
			chs+=2;
		}
		else
		{
			if(x>122)
			{
				x=0;
				y=y+2;
			}
			m=strchr(zf_index,*chs);
			if (m!=NULL)
			{
				OLED_Set_Pos(x, y);
				for(j = 0; j < 8; j++)
					Write_IIC_Data(zf[((u8)*m-' ') * 16 + j]);
				OLED_Set_Pos(x, y + 1);
				for(j = 0; j < 8; j++)
					Write_IIC_Data(zf[((u8)*m-' ') * 16 + j + 8]);
				x += 8;
			}
			chs++;
		}
	}
}

//初始化SSD1306
void OLED_Init(void)
{
	IIC_GPIO_Config();
  Write_IIC_Command(0xAE); //--display off
  Write_IIC_Command(0x00); //---set low column address
  Write_IIC_Command(0x10); //---set high column address
  Write_IIC_Command(0x40); //--set start line address
  Write_IIC_Command(0xB0); //--set page address
  Write_IIC_Command(0x81); // contract control
  Write_IIC_Command(0xFF); //--128
  Write_IIC_Command(0xA1); //set segment remap
  Write_IIC_Command(0xA6); //--normal / reverse
  Write_IIC_Command(0xA8); //--set multiplex ratio(1 to 64)
  Write_IIC_Command(0x3F); //--1/32 duty
  Write_IIC_Command(0xC8); //Com scan direction
  Write_IIC_Command(0xD3); //-set display offset
  Write_IIC_Command(0x00); //
  Write_IIC_Command(0xD5); //set osc division
  Write_IIC_Command(0x80); //
  Write_IIC_Command(0xD8); //set area color mode off
  Write_IIC_Command(0x05); //
  Write_IIC_Command(0xD9); //Set Pre-Charge Period
  Write_IIC_Command(0xF1); //
  Write_IIC_Command(0xDA); //set com pin configuartion
  Write_IIC_Command(0x12); //
  Write_IIC_Command(0xDB); //set Vcomh
  Write_IIC_Command(0x30); //
  Write_IIC_Command(0x8D); //set charge pump enable
  Write_IIC_Command(0x14); //
  Write_IIC_Command(0xAF); //--turn on oled panel
}

// x0,y0 -- 起始点坐标(x0:0~127, y0:0~7); x1,y1 -- 起点对角线(结束点)的坐标(x1:1~128,y1:1~8)
void OLED_DrawBMP(u8 x0,u8 y0,u8 x1,u8 y1,u8 BMP[])
{
	u16 j=0;
	u8 x,y;

  if(y1%8==0)
		y = y1/8;
  else
		y = y1/8 + 1;
	for(y=y0;y<y1;y++)
	{
		OLED_Set_Pos(x0,y);
    for(x=x0;x<x1;x++)
		{
			Write_IIC_Data(BMP[j++]);
		}
	}
}
