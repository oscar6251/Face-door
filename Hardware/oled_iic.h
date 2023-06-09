#ifndef __OLED_IIC_H
#define	__OLED_IIC_H

#include "sys.h"
#include <inttypes.h>

#define OLED_ADDRESS	0x78 //默认0x78

//定义IIC总线连接的GPIO端口, 用户只需要修改下面代码即可任意改变SCL和SDA的引脚
#define OLED_SCL PBout(8)  //SCL引脚
#define OLED_SDA PBout(9)  //SDA引脚

#define OLED_SCL_GPIO_PORT	GPIOB			/* GPIO端口 */
#define OLED_SCL_RCC 	      RCC_APB2Periph_GPIOB		/* GPIO端口时钟 */
#define OLED_SCL_PIN		    GPIO_Pin_8			/* 连接到SCL时钟线的GPIO */

#define OLED_SDA_GPIO_PORT	GPIOB			/* GPIO端口 */
#define OLED_SDA_RCC 	      RCC_APB2Periph_GPIOB		/* GPIO端口时钟 */
#define OLED_SDA_PIN		    GPIO_Pin_9			/* 连接到SDA数据线的GPIO */


#define OLED_IIC_SDA_READ()  GPIO_ReadInputDataBit(OLED_SDA_GPIO_PORT, OLED_SDA_PIN)	/* 读SDA口线状态 */

extern unsigned int HZ;

unsigned int GB16_NUM(void);
void IIC_GPIO_Config(void);
void IIC_Start(void);
void IIC_Stop(void);
uint8_t IIC_WaitAck(void);
void Write_IIC_Byte(uint8_t _ucByte);
void Write_IIC_Command(u8 IIC_Command);
void OLED_Fill(u8 fill_Data);
void OLED_Set_Pos(u8 x, u8 y);
void OLED_ON(void);
void OLED_OFF(void);
void OLED_Clear(void);
void OLED_ShowChar(u8 x, u8 y, u8 chr);
void OLED_ShowNum(u8 x, u8 y, u32 num, u8 len,u8 mode);
void OLED_ShowCH(u8 x,u8 y,u8 *chs);
void OLED_Init(void);
void OLED_DrawBMP(u8 x0,u8 y0,u8 x1,u8 y1,u8 BMP[]);
void OLED_ShowHexNum(u8 x, u8 y, u32 num, u8 len);
#endif
