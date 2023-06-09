#include "oled.h"
#include "stdlib.h"
//#include "font.h"  	 
#include "delay.h"
//#include "stm32f4xx_rcc.h"
#include "sys.h" 
//#include "fontupd.h"
//#include "w25qxx.h"
//#include "lcd.h"
//#include "text.h"	
#include "string.h"												    
#include "usart.h"											    
#include "oledfont.h"												    

//code 字符指针开始
//从字库中查找出字模
//code 字符串的开始地址,GBK码
//mat  数据存放地址 (size/8+((size%8)?1:0))*(size) bytes大小	
//size:字体大小
//void Get_HzMat(unsigned char *code,unsigned char *mat,u8 size)
//{		    
//	unsigned char qh,ql;
//	unsigned char i;					  
//	unsigned long foffset; 
//	u8 csize=(size/8+((size%8)?1:0))*(size);//得到字体一个字符对应点阵集所占的字节数	 
//	qh=*code;
//	ql=*(++code);
//	if(qh<0x81||ql<0x40||ql==0xff||qh==0xff)//非 常用汉字
//	{   		    
//	    for(i=0;i<csize;i++)*mat++=0x00;//填充满格
//	    return; //结束访问
//	}          
//	if(ql<0x7f)ql-=0x40;//注意!
//	else ql-=0x41;
//	qh-=0x81;   
//	foffset=((unsigned long)190*qh+ql)*csize;	//得到字库中的字节偏移量  		  
//	switch(size)
//	{
//		case 12:
//			W25QXX_Read(mat,foffset+ftinfo.f12addr,csize);
//			break;
//		case 16:
//			W25QXX_Read(mat,foffset+ftinfo.f16addr,csize);
//			break;
//		case 24:
//			W25QXX_Read(mat,foffset+ftinfo.f24addr,csize);
//			break;
//			
//	}     												    
//}  
////显示一个指定大小的汉字
////x,y :汉字的坐标
////font:汉字GBK码
////size:字体大小
////mode:0,正常显示,1,叠加显示	   
//void Show_Font(u16 x,u16 y,u8 *font,u8 size,u8 mode)
//{
//	u8 temp,t,t1;
//	u16 y0=y;
//	u8 dzk[72];   
//	u8 csize=(size/8+((size%8)?1:0))*(size);//得到字体一个字符对应点阵集所占的字节数	 
//	if(size!=12&&size!=16&&size!=24)return;	//不支持的size
//	Get_HzMat(font,dzk,size);	//得到相应大小的点阵数据 
//	for(t=0;t<csize;t++)
//	{   												   
//		temp=dzk[t];			//得到点阵数据                          
//		for(t1=0;t1<8;t1++)
//		{
//			if(temp&0x80)OLED_DrawPoint(x,y,1);//LCD_Fast_DrawPoint(x,y,POINT_COLOR);
//			else if(mode==0)OLED_DrawPoint(x,y,0);//LCD_Fast_DrawPoint(x,y,BACK_COLOR); 
//			temp<<=1;
//			y++;
//			if((y-y0)==size)
//			{
//				y=y0;
//				x++;
//				break;
//			}
//		}  	 
//	}  
//}
// 			 		 
////在指定宽度的中间显示字符串
////如果字符长度超过了len,则用Show_Str显示
////len:指定要显示的宽度			  
//void Show_Str_Mid(u16 x,u16 y,u8*str,u8 size,u8 len)
//{
//	u16 strlenth=0;
//   	strlenth=strlen((const char*)str);
//	strlenth*=size/2;
////	if(strlenth>len)Show_Str(x,y,lcddev.width,lcddev.height,str,size,1);
////	else
////	{
////		strlenth=(len-strlenth)/2;
////	    Show_Str(strlenth+x,y,lcddev.width,lcddev.height,str,size,1);
////	}
//}   


//OLED的显存
//存放格式如下.
//[0]0 1 2 3 ... 127	
//[1]0 1 2 3 ... 127	
//[2]0 1 2 3 ... 127	
//[3]0 1 2 3 ... 127	
//[4]0 1 2 3 ... 127	
//[5]0 1 2 3 ... 127	
//[6]0 1 2 3 ... 127	
//[7]0 1 2 3 ... 127 		   
u8 OLED_GRAM[128][8];	 

//更新显存到LCD		 
void OLED_Refresh_Gram(void)
{
	u8 i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
		OLED_WR_Byte (0x00,OLED_CMD);      //设置显示位置—列低地址
		OLED_WR_Byte (0x10,OLED_CMD);      //设置显示位置—列高地址   
		for(n=0;n<128;n++)OLED_WR_Byte(OLED_GRAM[n][i],OLED_DATA); 
	}   
}
#if OLED_MODE==1	//8080并口
//通过拼凑的方法向OLED输出一个8位数据
//data:要输出的数据
void OLED_Data_Out(u8 data)
{
	u16 dat=data&0X0F;
	GPIOC->ODR&=~(0XF<<6);		//清空6~9
	GPIOC->ODR|=dat<<6;			//D[3:0]-->PC[9:6]
	GPIO_Write(GPIOC,dat<<6);
	PCout(11)=(data>>4)&0X01;	//D4
	PBout(6)=(data>>5)&0X01;	//D5
	PEout(5)=(data>>6)&0X01;	//D6
	PEout(6)=(data>>7)&0X01;	//D7 
} 
//向SSD1306写入一个字节。
//dat:要写入的数据/命令
//cmd:数据/命令标志 0,表示命令;1,表示数据;
void OLED_WR_Byte(u8 dat,u8 cmd)
{
	OLED_Data_Out(dat);	    
 	OLED_RS=cmd;
	OLED_CS=0;	   
	OLED_WR=0;	 
	OLED_WR=1;
	OLED_CS=1;	  
	OLED_RS=1;	 
} 	    	    
#else
//向SSD1306写入一个字节。
//dat:要写入的数据/命令
//cmd:数据/命令标志 0,表示命令;1,表示数据;
void OLED_WR_Byte(u8 dat,u8 cmd)
{	
	u8 i;			  
	OLED_RS=cmd; //写命令 
	OLED_CS=0;		  
	for(i=0;i<8;i++)
	{			  
		OLED_SCLK=0;
		if(dat&0x80)OLED_SDIN=1;
		else OLED_SDIN=0;
		OLED_SCLK=1;
		dat<<=1;   
	}				 
	OLED_CS=1;		  
	OLED_RS=1;   	  
} 
#endif
	  	  
//开启OLED显示    
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}
//关闭OLED显示     
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}		   			 
//清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!	  
void OLED_Clear(void)  
{  
	u8 i,n;  
	for(i=0;i<8;i++)for(n=0;n<128;n++)OLED_GRAM[n][i]=0X00;  
	OLED_Refresh_Gram();//更新显示
}
void OLED_Clear_NOupdate(void)  
{  
	u8 i,n;  
	for(i=0;i<8;i++)for(n=0;n<128;n++)OLED_GRAM[n][i]=0X00;  
	OLED_Refresh_Gram();//更新显示
}
//画点 
//x:0~127
//y:0~63
//t:1 填充 0,清空				   
void OLED_DrawPoint(u8 x,u8 y,u8 t)
{
	u8 pos,bx,temp=0;
	if(x>127||y>63)return;//超出范围了.
	pos=7-y/8;
	bx=y%8;
	temp=1<<(7-bx);
	if(t)OLED_GRAM[x][pos]|=temp;
	else OLED_GRAM[x][pos]&=~temp;	    
}
//x1,y1,x2,y2 填充区域的对角坐标
//确保x1<=x2;y1<=y2 0<=x1<=127 0<=y1<=63	 	 
//dot:0,清空;1,填充	  
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot)  
{  
	u8 x,y;  
	for(x=x1;x<=x2;x++)
	{
		for(y=y1;y<=y2;y++)OLED_DrawPoint(x,y,dot);
	}													    
//	OLED_Refresh_Gram();//更新显示
}
//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//mode:0,反白显示;1,正常显示				 
//size:选择字体 12/16/24
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode)
{      			    
	u8 temp,t,t1;
	u8 y0=y;
	u8 csize=(size/8+((size%8)?1:0))*(size/2);		//得到字体一个字符对应点阵集所占的字节数
	chr=chr-' ';//得到偏移后的值		 
    for(t=0;t<csize;t++)
    {   
		if(size==12)temp=asc2_1206[chr][t]; 	 	//调用1206字体
		else if(size==16)temp=asc2_1608[chr][t];	//调用1608字体
		else if(size==24)temp=asc2_2412[chr][t];	//调用2412字体
		else return;								//没有的字库
        for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)OLED_DrawPoint(x,y,mode);
			else OLED_DrawPoint(x,y,!mode);
			temp<<=1;
			y++;
			if((y-y0)==size)
			{
				y=y0;
				x++;
				break;
			}
		}  	
			
    }    
	//OLED_Refresh_Gram();//更新OLED			
}
//m^n函数
u32 mypow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}				  
//显示2个数字
//x,y :起点坐标	 
//len :数字的位数
//size:字体大小
//mode:模式	0,填充模式;1,叠加模式
//num:数值(0~4294967295);	 		  
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/mypow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+(size/2)*t,y,' ',size,1);
				continue;
			}else enshow=1; 
		 	 
		}
	 	OLED_ShowChar(x+(size/2)*t,y,temp+'0',size,1); 
	}
//	OLED_Refresh_Gram();//更新OLED		
} 
//显示字符串
//x,y:起点坐标  
//size:字体大小 
//*p:字符串起始地址 
void OLED_ShowString(u8 x,u8 y,const u8 *p,u8 size)
{	
    while((*p<='~')&&(*p>=' '))//判断是不是非法字符!
    {       
        if(x>(128-(size/2))){x=0;y+=size;}
        if(y>(64-size)){y=x=0;OLED_Clear();}
        OLED_ShowChar(x,y,*p,size,1);	 
        x+=size/2;
        p++;
    }  
	
}	
//初始化SSD1306					    
void OLED_Init(void)
{ 	 		 
  GPIO_InitTypeDef  GPIO_InitStructure;
	
	//RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE|RCC_AHB1Periph_GPIOG, ENABLE);//使能PORTA~E,PORTG时钟
  
#if OLED_MODE==1		//使用8080并口模式		
	
	//GPIO初始化设置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7 ;	
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_11;	
	GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化	
  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;	
	GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_5;	
	GPIO_Init(GPIOE, &GPIO_InitStructure);//初始化	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;	
	GPIO_Init(GPIOG, &GPIO_InitStructure);//初始化	
 
	
 	OLED_WR=1;
	OLED_RD=1; 
#else					//使用4线SPI 串口模式
RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOD, ENABLE);	 //使能PB,PE端口时钟
RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 ;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
//  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
//  GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化
//	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;	
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化		

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化		
	
	GPIO_PinRemapConfig(GPIO_Remap_PD01, ENABLE); // 启用PD0和PD1端口作为GPIO
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);//JTAG-DP 失能 + SW-DP 使能

	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;	
	GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化
	

	
	OLED_SDIN=1;
	OLED_SCLK=1;
#endif
	OLED_CS=1;
	OLED_RS=1;	 
	
	OLED_RST=0;
	delay_ms(100);
	OLED_RST=1; 
					  
	OLED_WR_Byte(0xAE,OLED_CMD); //关闭显示
	OLED_WR_Byte(0xD5,OLED_CMD); //设置时钟分频因子,震荡频率
	OLED_WR_Byte(80,OLED_CMD);   //[3:0],分频因子;[7:4],震荡频率
	OLED_WR_Byte(0xA8,OLED_CMD); //设置驱动路数
	OLED_WR_Byte(0X3F,OLED_CMD); //默认0X3F(1/64) 
	OLED_WR_Byte(0xD3,OLED_CMD); //设置显示偏移
	OLED_WR_Byte(0X00,OLED_CMD); //默认为0

	OLED_WR_Byte(0x40,OLED_CMD); //设置显示开始行 [5:0],行数.
													    
	OLED_WR_Byte(0x8D,OLED_CMD); //电荷泵设置
	OLED_WR_Byte(0x14,OLED_CMD); //bit2，开启/关闭
	OLED_WR_Byte(0x20,OLED_CMD); //设置内存地址模式
	OLED_WR_Byte(0x02,OLED_CMD); //[1:0],00，列地址模式;01，行地址模式;10,页地址模式;默认10;
	OLED_WR_Byte(0xA1,OLED_CMD); //段重定义设置,bit0:0,0->0;1,0->127;
	OLED_WR_Byte(0xC0,OLED_CMD); //设置COM扫描方向;bit3:0,普通模式;1,重定义模式 COM[N-1]->COM0;N:驱动路数
	OLED_WR_Byte(0xDA,OLED_CMD); //设置COM硬件引脚配置
	OLED_WR_Byte(0x12,OLED_CMD); //[5:4]配置
		 
	OLED_WR_Byte(0x81,OLED_CMD); //对比度设置
	OLED_WR_Byte(0xEF,OLED_CMD); //1~255;默认0X7F (亮度设置,越大越亮)
	OLED_WR_Byte(0xD9,OLED_CMD); //设置预充电周期
	OLED_WR_Byte(0xf1,OLED_CMD); //[3:0],PHASE 1;[7:4],PHASE 2;
	OLED_WR_Byte(0xDB,OLED_CMD); //设置VCOMH 电压倍率
	OLED_WR_Byte(0x30,OLED_CMD); //[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;

	OLED_WR_Byte(0xA4,OLED_CMD); //全局显示开启;bit0:1,开启;0,关闭;(白屏/黑屏)
	OLED_WR_Byte(0xA6,OLED_CMD); //设置显示方式;bit0:1,反相显示;0,正常显示	    						   
	OLED_WR_Byte(0xAF,OLED_CMD); //开启显示	 
	OLED_Clear();
}  






void OLED_Show_Font(u16 x,u16 y,u8 fnum)
{
	u8 temp,t,t1;
	u16 y0=y;
	u8 *dzk;   
	u8 csize=32;
	dzk=(u8*)OLED_HZK_TBL[fnum]; //???????????? 
	for(t=0;t<csize;t++)
	{       
		temp=dzk[t];                          
		for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)OLED_DrawPoint(x,y,1);
			else OLED_DrawPoint(x,y,0); 
			temp<<=1;
			y++;
			if((y-y0)==16)
			{
				y=y0;
				x++;
				break;
			}
		}    
	}  
//	OLED_Refresh_Gram();//更新OLED	
}
////显示图像
void OLED_Show_Image(u8 fnum)
{
	u16 temp,t,t1;
	u16 x=0,y=0;
	u8 *dzk;   
	u16 csize=1024;
	dzk=(u8*)OLED_Image[fnum];
	for(t=0;t<csize;t++)
	{       
		temp=dzk[t];                          
		for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)OLED_DrawPoint(x,y,1);
			else OLED_DrawPoint(x,y,0); 
			temp<<=1;
			x++;
			if((x)==128)
			{
				x=0;
				y++;
				break;
			}
		}    
	}  
//	OLED_Refresh_Gram();//更新OLED	
}





#define Gui_DrawPoint OLED_DrawPoint

//从ILI93xx读出的数据为GBR格式，而我们写入的时候为RGB格式。
//通过该函数转换
//c:GBR格式的颜色值
//返回值：RGB格式的颜色值
u16 LCD_BGR2RGB(u16 c)
{
  u16  r,g,b,rgb;   
  b=(c>>0)&0x1f;
  g=(c>>5)&0x3f;
  r=(c>>11)&0x1f;	 
  rgb=(b<<11)+(g<<5)+(r<<0);		 
  return(rgb);

}




void Gui_Circle(u16 X,u16 Y,u16 R,u16 fc) 
{//Bresenham算法 
    unsigned short  a,b; 
    int c; 
    a=0; 
    b=R; 
    c=3-2*R; 
    while (a<b) 
    { 
        Gui_DrawPoint(X+a,Y+b,fc);     //        7 
        Gui_DrawPoint(X-a,Y+b,fc);     //        6 
        Gui_DrawPoint(X+a,Y-b,fc);     //        2 
        Gui_DrawPoint(X-a,Y-b,fc);     //        3 
        Gui_DrawPoint(X+b,Y+a,fc);     //        8 
        Gui_DrawPoint(X-b,Y+a,fc);     //        5 
        Gui_DrawPoint(X+b,Y-a,fc);     //        1 
        Gui_DrawPoint(X-b,Y-a,fc);     //        4 

        if(c<0) c=c+4*a+6; 
        else 
        { 
            c=c+4*(a-b)+10; 
            b-=1; 
        } 
       a+=1; 
    } 
    if (a==b) 
    { 
        Gui_DrawPoint(X+a,Y+b,fc); 
        Gui_DrawPoint(X+a,Y+b,fc); 
        Gui_DrawPoint(X+a,Y-b,fc); 
        Gui_DrawPoint(X-a,Y-b,fc); 
        Gui_DrawPoint(X+b,Y+a,fc); 
        Gui_DrawPoint(X-b,Y+a,fc); 
        Gui_DrawPoint(X+b,Y-a,fc); 
        Gui_DrawPoint(X-b,Y-a,fc); 
    } 
	
} 
//画线函数，使用Bresenham 画线算法
void Gui_DrawLine(u16 x0, u16 y0,u16 x1, u16 y1,u16 Color)   
{
int dx,             // difference in x's
    dy,             // difference in y's
    dx2,            // dx,dy * 2
    dy2, 
    x_inc,          // amount in pixel space to move during drawing
    y_inc,          // amount in pixel space to move during drawing
    error,          // the discriminant i.e. error i.e. decision variable
    index;          // used for looping	


	//Lcd_SetXY(x0,y0);
	dx = x1-x0;//计算x距离
	dy = y1-y0;//计算y距离

	if (dx>=0)
	{
		x_inc = 1;
	}
	else
	{
		x_inc = -1;
		dx    = -dx;  
	} 
	
	if (dy>=0)
	{
		y_inc = 1;
	} 
	else
	{
		y_inc = -1;
		dy    = -dy; 
	} 

	dx2 = dx << 1;
	dy2 = dy << 1;

	if (dx > dy)//x距离大于y距离，那么每个x轴上只有一个点，每个y轴上有若干个点
	{//且线的点数等于x距离，以x轴递增画点
		// initialize error term
		error = dy2 - dx; 

		// draw the line
		for (index=0; index <= dx; index++)//要画的点数不会超过x距离
		{
			//画点
			Gui_DrawPoint(x0,y0,Color);
			
			// test if error has overflowed
			if (error >= 0) //是否需要增加y坐标值
			{
				error-=dx2;

				// move to next line
				y0+=y_inc;//增加y坐标值
			} // end if error overflowed

			// adjust the error term
			error+=dy2;

			// move to the next pixel
			x0+=x_inc;//x坐标值每次画点后都递增1
		} // end for
	} // end if |slope| <= 1
	else//y轴大于x轴，则每个y轴上只有一个点，x轴若干个点
	{//以y轴为递增画点
		// initialize error term
		error = dx2 - dy; 

		// draw the line
		for (index=0; index <= dy; index++)
		{
			// set the pixel
			Gui_DrawPoint(x0,y0,Color);

			// test if error overflowed
			if (error >= 0)
			{
				error-=dy2;

				// move to next line
				x0+=x_inc;
			} // end if error overflowed

			// adjust the error term
			error+=dx2;

			// move to the next pixel
			y0+=y_inc;
		} // end for
	} // end else |slope| > 1
}



void Gui_box(u16 x, u16 y, u16 w, u16 h,u16 bc)
{
	Gui_DrawLine(x,y,x+w,y,0xEF7D);
	Gui_DrawLine(x+w-1,y+1,x+w-1,y+1+h,0x2965);
	Gui_DrawLine(x,y+h,x+w,y+h,0x2965);
	Gui_DrawLine(x,y,x,y+h,0xEF7D);
    Gui_DrawLine(x+1,y+1,x+1+w-2,y+1+h-2,bc);
}
void Gui_box2(u16 x,u16 y,u16 w,u16 h, u8 mode)
{
	if (mode==0)	{
		Gui_DrawLine(x,y,x+w,y,0xEF7D);
		Gui_DrawLine(x+w-1,y+1,x+w-1,y+1+h,0x2965);
		Gui_DrawLine(x,y+h,x+w,y+h,0x2965);
		Gui_DrawLine(x,y,x,y+h,0xEF7D);
		}
	if (mode==1)	{
		Gui_DrawLine(x,y,x+w,y,0x2965);
		Gui_DrawLine(x+w-1,y+1,x+w-1,y+1+h,0xEF7D);
		Gui_DrawLine(x,y+h,x+w,y+h,0xEF7D);
		Gui_DrawLine(x,y,x,y+h,0x2965);
	}
	if (mode==2)	{
		Gui_DrawLine(x,y,x+w,y,0xffff);
		Gui_DrawLine(x+w-1,y+1,x+w-1,y+1+h,0xffff);
		Gui_DrawLine(x,y+h,x+w,y+h,0xffff);
		Gui_DrawLine(x,y,x,y+h,0xffff);
	}
}


/**************************************************************************************
功能描述: 在屏幕显示一凸起的按钮框
输    入: u16 x1,y1,x2,y2 按钮框左上角和右下角坐标
输    出: 无
**************************************************************************************/
void DisplayButtonDown(u16 x1,u16 y1,u16 x2,u16 y2)
{
//	Gui_DrawLine(x1,  y1,  x2,y1, GRAY2);  //H
//	Gui_DrawLine(x1+1,y1+1,x2,y1+1, GRAY1);  //H
//	Gui_DrawLine(x1,  y1,  x1,y2, GRAY2);  //V
//	Gui_DrawLine(x1+1,y1+1,x1+1,y2, GRAY1);  //V
//	Gui_DrawLine(x1,  y2,  x2,y2, WHITE);  //H
//	Gui_DrawLine(x2,  y1,  x2,y2, WHITE);  //V
}

/**************************************************************************************
功能描述: 在屏幕显示一凹下的按钮框
输    入: u16 x1,y1,x2,y2 按钮框左上角和右下角坐标
输    出: 无
**************************************************************************************/
void DisplayButtonUp(u16 x1,u16 y1,u16 x2,u16 y2)
{
//	Gui_DrawLine(x1,  y1,  x2,y1, WHITE); //H
//	Gui_DrawLine(x1,  y1,  x1,y2, WHITE); //V
//	
//	Gui_DrawLine(x1+1,y2-1,x2,y2-1, GRAY1);  //H
//	Gui_DrawLine(x1,  y2,  x2,y2, GRAY2);  //H
//	Gui_DrawLine(x2-1,y1+1,x2-1,y2, GRAY1);  //V
//    Gui_DrawLine(x2  ,y1  ,x2,y2, GRAY2); //V
}

void Gui_DrawFont_GBK12(u16 x, u16 y, u16 fc, u16 bc, u8 *s)
{
	unsigned char i,j;
	unsigned short k,x0;
	x0=x;

	while(*s) 
	{	
		if((*s) < 128) 
		{
			k=*s;
			if (k==13) 
			{
				x=x0;
				y+=16;
			}
			else OLED_ShowChar(x,y,*s,12,1);
//			{
//				if (k>32) k-=32; else k=0;
//	
//			    for(i=0;i<12;i++)
//				for(j=0;j<8;j++) 
//					{
//				    	if(asc2_1206[k][i]&(0x80>>j))	Gui_DrawPoint(x+j,y+i,fc);
//						else 
//						{
//							if (fc!=bc) Gui_DrawPoint(x+j,y+i,bc);
//						}
//					}
				x+=8;
//			}
			s++;
		}
			
		else 
		{
			for (k=0;k<hz12_num;k++) 
			{
			  if ((hz12[k].Index[0]==*(s))&&(hz12[k].Index[1]==*(s+1)))
			  { 
				    for(i=0;i<12;i++)
				    {
						for(j=0;j<8;j++) 
							{
						    	if(hz12[k].Msk[i*2]&(0x80>>j))	Gui_DrawPoint(x+j,y+i,fc);
								else {
									if (fc!=bc) Gui_DrawPoint(x+j,y+i,bc);
								}
							}
						for(j=0;j<8;j++) 
							{
						    	if(hz12[k].Msk[i*2+1]&(0x80>>j))	Gui_DrawPoint(x+j+8,y+i,fc);
								else 
								{
									if (fc!=bc) Gui_DrawPoint(x+j+8,y+i,bc);
								}
							}
				    }
				}
			  }
			s+=2;x+=12;
		} 
		
	}
}


void Gui_DrawFont_GBK16(u16 x, u16 y, u16 fc, u16 bc, u8 *s)
{
	unsigned char i,j;
	unsigned short k,x0;
	x0=x;

	while(*s) 
	{	
		if((*s) < 128) 
		{
			k=*s;
			if (k==13) 
			{
				x=x0;
				y+=16;
			}
			else OLED_ShowChar(x,y,*s,16,1);
//			{
//				if (k>32) k-=32; else k=0;
//	
//			    for(i=0;i<16;i++)
//				for(j=0;j<8;j++) 
//					{
//				    	if(asc2_1608[k][i]&(0x80>>j))	Gui_DrawPoint(x+j,y+i,fc);
//						else 
//						{
//							if (fc!=bc) Gui_DrawPoint(x+j,y+i,bc);
//						}
//					}
				x+=8;
//			}
			s++;
		}
			
		else 
		{
		

			for (k=0;k<hz16_num;k++) 
			{
			  if ((hz16[k].Index[0]==*(s))&&(hz16[k].Index[1]==*(s+1)))
			  { 
				    for(i=0;i<16;i++)
				    {
						for(j=0;j<8;j++) 
							{
						    	if(hz16[k].Msk[i*2]&(0x80>>j))	Gui_DrawPoint(x+j,y+i,fc);
								else {
									if (fc!=bc) Gui_DrawPoint(x+j,y+i,bc);
								}
							}
						for(j=0;j<8;j++) 
							{
						    	if(hz16[k].Msk[i*2+1]&(0x80>>j))	Gui_DrawPoint(x+j+8,y+i,fc);
								else 
								{
									if (fc!=bc) Gui_DrawPoint(x+j+8,y+i,bc);
								}
							}
				    }
				}
			  }
			s+=2;x+=16;
		} 
		
	}
}

void Gui_DrawFont_GBK24(u16 x, u16 y, u16 fc, u16 bc, u8 *s)
{
	unsigned char i,j;
	unsigned short k;

	while(*s) 
	{
		if( *s < 0x80 ) 
		{
			k=*s;
			if (k>32) k-=32; else k=0;

		    for(i=0;i<16;i++)
			for(j=0;j<8;j++) 
				{
			    	if( asc2_1608[k][i]&(0x80>>j))	
					Gui_DrawPoint(x+j,y+i,fc);
					else 
					{
						if (fc!=bc) Gui_DrawPoint(x+j,y+i,bc);
					}
				}
			s++;x+=8;
		}
		else 
		{

			for (k=0;k<hz24_num;k++) 
			{
			  if ((hz24[k].Index[0]==*(s))&&(hz24[k].Index[1]==*(s+1)))
			  { 
				    for(i=0;i<24;i++)
				    {
						for(j=0;j<8;j++) 
							{
						    	if(hz24[k].Msk[i*3]&(0x80>>j))
								Gui_DrawPoint(x+j,y+i,fc);
								else 
								{
									if (fc!=bc) Gui_DrawPoint(x+j,y+i,bc);
								}
							}
						for(j=0;j<8;j++) 
							{
						    	if(hz24[k].Msk[i*3+1]&(0x80>>j))	Gui_DrawPoint(x+j+8,y+i,fc);
								else {
									if (fc!=bc) Gui_DrawPoint(x+j+8,y+i,bc);
								}
							}
						for(j=0;j<8;j++) 
							{
						    	if(hz24[k].Msk[i*3+2]&(0x80>>j))	
								Gui_DrawPoint(x+j+16,y+i,fc);
								else 
								{
									if (fc!=bc) Gui_DrawPoint(x+j+16,y+i,bc);
								}
							}
				    }
			  }
			}
			s+=2;x+=24;
		}
	}
}
void Gui_DrawFont_Num32(u16 x, u16 y, u16 fc, u16 bc, u16 num)
{
//	unsigned char i,j,k,c;
//	//lcd_text_any(x+94+i*42,y+34,32,32,0x7E8,0x0,sz32,knum[i]);
////	w=w/8;

//    for(i=0;i<32;i++)
//	{
//		for(j=0;j<4;j++) 
//		{
//			c=*(sz32+num*32*4+i*4+j);
//			for (k=0;k<8;k++)	
//			{
//	
//		    	if(c&(0x80>>k))	Gui_DrawPoint(x+j*8+k,y+i,fc);
//				else {
//					if (fc!=bc) Gui_DrawPoint(x+j*8+k,y+i,bc);
//				}
//			}
//		}
//	}
}
























