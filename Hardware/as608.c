#include <string.h>
#include <stdio.h>
#include "delay.h"
#include "usart3.h"
#include "as608.h"
#include "oled_iic.h"
#include "key1.h"
#include "LED.h"
#include "SERVO.h"

u32 AS608Addr = 0XFFFFFFFF; //Ĭ��		0016 0016 0016 0016 0016 0016 0016 0016
char str2[6] = {0};


//���ڷ���һ���ֽ�
static void MYUSART_SendData(u8 data)
{
  while((USART2->SR & 0X40) == 0);
  USART2->DR = data;
}
//���Ͱ�ͷ
static void SendHead(void)
{
  MYUSART_SendData(0xEF);
  MYUSART_SendData(0x01);
}
//���͵�ַ
static void SendAddr(void)
{
  MYUSART_SendData(AS608Addr >> 24);		
  MYUSART_SendData(AS608Addr >> 16);		
  MYUSART_SendData(AS608Addr >> 8);			
  MYUSART_SendData(AS608Addr);				
}
//���Ͱ���ʶ,
static void SendFlag(u8 flag)
{
  MYUSART_SendData(flag);
}
//���Ͱ�����
static void SendLength(int length)
{
  MYUSART_SendData(length >> 8);
  MYUSART_SendData(length);
}
//����ָ����
static void Sendcmd(u8 cmd)
{
  MYUSART_SendData(cmd);
}
//����У���
static void SendCheck(u16 check)
{
  MYUSART_SendData(check >> 8);
  MYUSART_SendData(check);
}
//�ж��жϽ��յ�������û��Ӧ���
//waittimeΪ�ȴ��жϽ������ݵ�ʱ�䣨��λ1ms��
//����ֵ�����ݰ��׵�ַ

static u8 *JudgeStr(u16 waittime)
{
  char *data;
  u8 str[8];
  str[0] = 0xef;
  str[1] = 0x01;
  str[2] = AS608Addr >> 24;
  str[3] = AS608Addr >> 16;
  str[4] = AS608Addr >> 8;
  str[5] = AS608Addr;
  str[6] = 0x07;
  str[7] = '\0';
  
  USART3_RX_STA = 0;
  while(--waittime)
  {
    Delay_ms(1);
    if(USART3_RX_STA & 0X8000) //���յ�һ������		
    {
      USART3_RX_STA = 0;
      data = strstr((const char*)USART3_RX_BUF, (const char*)str);
      if(data)
        return (u8*)data;
    }
  }
  return 0;
}
//¼��ͼ�� PS_GetImage
//����:̽����ָ��̽�⵽��¼��ָ��ͼ�����ImageBuffer��
//ģ�鷵��ȷ����
u8 PS_GetImage(void)
{
  u16 temp;
  u8  ensure;
  u8  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//�������ʶ
  SendLength(0x03);
  Sendcmd(0x01);
  temp =  0x01 + 0x03 + 0x01;
  SendCheck(temp);
  data = JudgeStr(2000);
  if(data)
    ensure = data[9];//��9λλȷ����
  else
    ensure = 0xff;
  return ensure;
}
//�������� PS_GenChar
//����:��ImageBuffer�е�ԭʼͼ������ָ�������ļ�����CharBuffer1��CharBuffer2
//����:BufferID --> charBuffer1:0x01	charBuffer1:0x02
//ģ�鷵��ȷ����
u8 PS_GenChar(u8 BufferID)
{
  u16 temp;
  u8  ensure;
  u8  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//�������ʶ
  SendLength(0x04);
  Sendcmd(0x02);
  MYUSART_SendData(BufferID);
  temp = 0x01 + 0x04 + 0x02 + BufferID;
  SendCheck(temp);
  data = JudgeStr(2000);
  if(data)
    ensure = data[9];
  else
    ensure = 0xff;
  return ensure;
}
//��ȷ�ȶ���öָ������ PS_Match
//����:��ȷ�ȶ�CharBuffer1 ��CharBuffer2 �е������ļ�
//ģ�鷵��ȷ����
u8 PS_Match(void)
{
  u16 temp;
  u8  ensure;
  u8  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//�������ʶ
  SendLength(0x03);
  Sendcmd(0x03);
  temp = 0x01 + 0x03 + 0x03;
  SendCheck(temp);
  data = JudgeStr(2000);
  if(data)
    ensure = data[9];
  else
    ensure = 0xff;
  return ensure;
}
//����ָ�� PS_Search
//����:��CharBuffer1��CharBuffer2�е������ļ����������򲿷�ָ�ƿ�.�����������򷵻�ҳ�롣
//����:  BufferID @ref CharBuffer1	CharBuffer2
//˵��:  ģ�鷵��ȷ���֣�ҳ�루����ָ��ģ�壩
u8 PS_Search(u8 BufferID, u16 StartPage, u16 PageNum, SearchResult *p)
{
  u16 temp;
  u8  ensure;
  u8  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//�������ʶ
  SendLength(0x08);
  Sendcmd(0x04);
  MYUSART_SendData(BufferID);
  MYUSART_SendData(StartPage >> 8);
  MYUSART_SendData(StartPage);
  MYUSART_SendData(PageNum >> 8);
  MYUSART_SendData(PageNum);
  temp = 0x01 + 0x08 + 0x04 + BufferID
         + (StartPage >> 8) + (u8)StartPage
         + (PageNum >> 8) + (u8)PageNum;
  SendCheck(temp);
  data = JudgeStr(2000);
  if(data)
  {
    ensure = data[9];
    p->pageID   = (data[10] << 8) + data[11];
    p->mathscore = (data[12] << 8) + data[13];
  }
  else
    ensure = 0xff;
  return ensure;
}
//�ϲ�����������ģ�壩PS_RegModel
//����:��CharBuffer1��CharBuffer2�е������ļ��ϲ����� ģ��,�������CharBuffer1��CharBuffer2
//˵��:  ģ�鷵��ȷ����
u8 PS_RegModel(void)
{
  u16 temp;
  u8  ensure;
  u8  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//�������ʶ
  SendLength(0x03);
  Sendcmd(0x05);
  temp = 0x01 + 0x03 + 0x05;
  SendCheck(temp);
  data = JudgeStr(2000);
  if(data)
    ensure = data[9];
  else
    ensure = 0xff;
  return ensure;
}
//����ģ�� PS_StoreChar
//����:�� CharBuffer1 �� CharBuffer2 �е�ģ���ļ��浽 PageID ��flash���ݿ�λ�á�
//����:  BufferID @ref charBuffer1:0x01	charBuffer1:0x02
//       PageID��ָ�ƿ�λ�úţ�
//˵��:  ģ�鷵��ȷ����
u8 PS_StoreChar(u8 BufferID, u16 PageID)
{
  u16 temp;
  u8  ensure;
  u8  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//�������ʶ
  SendLength(0x06);
  Sendcmd(0x06);
  MYUSART_SendData(BufferID);
  MYUSART_SendData(PageID >> 8);
  MYUSART_SendData(PageID);
  temp = 0x01 + 0x06 + 0x06 + BufferID
         + (PageID >> 8) + (u8)PageID;
  SendCheck(temp);
  data = JudgeStr(2000);
  if(data)
    ensure = data[9];
  else
    ensure = 0xff;
  return ensure;
}
//ɾ��ģ�� PS_DeletChar
//����:  ɾ��flash���ݿ���ָ��ID�ſ�ʼ��N��ָ��ģ��
//����:  PageID(ָ�ƿ�ģ���)��Nɾ����ģ�������
//˵��:  ģ�鷵��ȷ����
u8 PS_DeletChar(u16 PageID, u16 N)
{
  u16 temp;
  u8  ensure;
  u8  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//�������ʶ
  SendLength(0x07);
  Sendcmd(0x0C);
  MYUSART_SendData(PageID >> 8);
  MYUSART_SendData(PageID);
  MYUSART_SendData(N >> 8);
  MYUSART_SendData(N);
  temp = 0x01 + 0x07 + 0x0C
         + (PageID >> 8) + (u8)PageID
         + (N >> 8) + (u8)N;
  SendCheck(temp);
  data = JudgeStr(2000);
  if(data)
    ensure = data[9];
  else
    ensure = 0xff;
  return ensure;
}
//���ָ�ƿ� PS_Empty
//����:  ɾ��flash���ݿ�������ָ��ģ��
//����:  ��
//˵��:  ģ�鷵��ȷ����
u8 PS_Empty(void)
{
  u16 temp;
  u8  ensure;
  u8  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//�������ʶ
  SendLength(0x03);
  Sendcmd(0x0D);
  temp = 0x01 + 0x03 + 0x0D;
  SendCheck(temp);
  data = JudgeStr(2000);
  if(data)
    ensure = data[9];
  else
    ensure = 0xff;
  return ensure;
}
//дϵͳ�Ĵ��� PS_WriteReg
//����:  дģ��Ĵ���
//����:  �Ĵ������RegNum:4\5\6
//˵��:  ģ�鷵��ȷ����
u8 PS_WriteReg(u8 RegNum, u8 DATA)
{
  u16 temp;
  u8  ensure;
  u8  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//�������ʶ
  SendLength(0x05);
  Sendcmd(0x0E);
  MYUSART_SendData(RegNum);
  MYUSART_SendData(DATA);
  temp = RegNum + DATA + 0x01 + 0x05 + 0x0E;
  SendCheck(temp);
  data = JudgeStr(2000);
  if(data)
    ensure = data[9];
  else
    ensure = 0xff;
  if(ensure == 0)
    printf("\r\n���ò����ɹ���");
  else
    printf("\r\n%s", EnsureMessage(ensure));
  return ensure;
}
//��ϵͳ�������� PS_ReadSysPara
//����:  ��ȡģ��Ļ��������������ʣ�����С��)
//����:  ��
//˵��:  ģ�鷵��ȷ���� + ����������16bytes��
u8 PS_ReadSysPara(SysPara *p)
{
  u16 temp;
  u8  ensure;
  u8  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//�������ʶ
  SendLength(0x03);
  Sendcmd(0x0F);
  temp = 0x01 + 0x03 + 0x0F;
  SendCheck(temp);
  data = JudgeStr(1000);
  if(data)
  {
    ensure = data[9];
    p->PS_max = (data[14] << 8) + data[15];
    p->PS_level = data[17];
    p->PS_addr = (data[18] << 24) + (data[19] << 16) + (data[20] << 8) + data[21];
    p->PS_size = data[23];
    p->PS_N = data[25];
  }
  else
    ensure = 0xff;
  if(ensure == 0x00)
  {
    printf("\r\nģ�����ָ������=%d", p->PS_max);
    printf("\r\n�Աȵȼ�=%d", p->PS_level);
    printf("\r\n��ַ=%x", p->PS_addr);
    printf("\r\n������=%d", p->PS_N * 9600);
  }
  else
    printf("\r\n%s", EnsureMessage(ensure));
  return ensure;
}
//����ģ���ַ PS_SetAddr
//����:  ����ģ���ַ
//����:  PS_addr
//˵��:  ģ�鷵��ȷ����
u8 PS_SetAddr(u32 PS_addr)
{
  u16 temp;
  u8  ensure;
  u8  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//�������ʶ
  SendLength(0x07);
  Sendcmd(0x15);
  MYUSART_SendData(PS_addr >> 24);
  MYUSART_SendData(PS_addr >> 16);
  MYUSART_SendData(PS_addr >> 8);
  MYUSART_SendData(PS_addr);
  temp = 0x01 + 0x07 + 0x15
         + (u8)(PS_addr >> 24) + (u8)(PS_addr >> 16)
         + (u8)(PS_addr >> 8) + (u8)PS_addr;
  SendCheck(temp);
  AS608Addr = PS_addr; //������ָ�������ַ
  data = JudgeStr(2000);
  if(data)
    ensure = data[9];
  else
    ensure = 0xff;
  AS608Addr = PS_addr;
  if(ensure == 0x00)
    printf("\r\n���õ�ַ�ɹ���");
  else
    printf("\r\n%s", EnsureMessage(ensure));
  return ensure;
}
//���ܣ� ģ���ڲ�Ϊ�û�������256bytes��FLASH�ռ����ڴ��û����±�,
//	�ü��±��߼��ϱ��ֳ� 16 ��ҳ��
//����:  NotePageNum(0~15),Byte32(Ҫд�����ݣ�32���ֽ�)
//˵��:  ģ�鷵��ȷ����
u8 PS_WriteNotepad(u8 NotePageNum, u8 *Byte32)
{
  u16 temp;
  u8  ensure, i;
  u8  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//�������ʶ
  SendLength(36);
  Sendcmd(0x18);
  MYUSART_SendData(NotePageNum);
  for(i = 0; i < 32; i++)
  {
    MYUSART_SendData(Byte32[i]);
    temp += Byte32[i];
  }
  temp = 0x01 + 36 + 0x18 + NotePageNum + temp;
  SendCheck(temp);
  data = JudgeStr(2000);
  if(data)
    ensure = data[9];
  else
    ensure = 0xff;
  return ensure;
}
//������PS_ReadNotepad
//���ܣ�  ��ȡFLASH�û�����128bytes����
//����:  NotePageNum(0~15)
//˵��:  ģ�鷵��ȷ����+�û���Ϣ
u8 PS_ReadNotepad(u8 NotePageNum, u8 *Byte32)
{
  u16 temp;
  u8  ensure, i;
  u8  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//�������ʶ
  SendLength(0x04);
  Sendcmd(0x19);
  MYUSART_SendData(NotePageNum);
  temp = 0x01 + 0x04 + 0x19 + NotePageNum;
  SendCheck(temp);
  data = JudgeStr(2000);
  if(data)
  {
    ensure = data[9];
    for(i = 0; i < 32; i++)
    {
      Byte32[i] = data[10 + i];
    }
  }
  else
    ensure = 0xff;
  return ensure;
}
//��������PS_HighSpeedSearch
//���ܣ��� CharBuffer1��CharBuffer2�е������ļ��������������򲿷�ָ�ƿ⡣
//		  �����������򷵻�ҳ��,��ָ����ڵ�ȷ������ָ�ƿ��� ���ҵ�¼ʱ����
//		  �ܺõ�ָ�ƣ���ܿ�������������
//����:  BufferID�� StartPage(��ʼҳ)��PageNum��ҳ����
//˵��:  ģ�鷵��ȷ����+ҳ�루����ָ��ģ�壩
u8 PS_HighSpeedSearch(u8 BufferID, u16 StartPage, u16 PageNum, SearchResult *p)
{
  u16 temp;
  u8  ensure;
  u8  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//�������ʶ
  SendLength(0x08);
  Sendcmd(0x1b);
  MYUSART_SendData(BufferID);
  MYUSART_SendData(StartPage >> 8);
  MYUSART_SendData(StartPage);
  MYUSART_SendData(PageNum >> 8);
  MYUSART_SendData(PageNum);
  temp = 0x01 + 0x08 + 0x1b + BufferID
         + (StartPage >> 8) + (u8)StartPage
         + (PageNum >> 8) + (u8)PageNum;
  SendCheck(temp);
  data = JudgeStr(2000);
  if(data)
  {
    ensure = data[9];
    p->pageID 	= (data[10] << 8) + data[11];
    p->mathscore = (data[12] << 8) + data[13];
  }
  else
    ensure = 0xff;
  return ensure;
}
//����Чģ����� PS_ValidTempleteNum
//���ܣ�����Чģ�����
//����: ��
//˵��: ģ�鷵��ȷ����+��Чģ�����ValidN
u8 PS_ValidTempleteNum(u16 *ValidN)
{
  u16 temp;
  u8  ensure;
  u8  *data;
  SendHead();
  SendAddr();
  SendFlag(0x01);//�������ʶ
  SendLength(0x03);
  Sendcmd(0x1d);
  temp = 0x01 + 0x03 + 0x1d;
  SendCheck(temp);
  data = JudgeStr(2000);
  if(data)
  {
    ensure = data[9];
    *ValidN = (data[10] << 8) + data[11];
  }
  else
    ensure = 0xff;

  if(ensure == 0x00)
  {
    printf("\r\n��Чָ�Ƹ���=%d", (data[10] << 8) + data[11]);
  }
  else
    printf("\r\n%s", EnsureMessage(ensure));
  return ensure;
}
//��AS608���� PS_HandShake
//����: PS_Addr��ַָ��
//˵��: ģ�鷵�µ�ַ����ȷ��ַ��
u8 PS_HandShake(u32 *PS_Addr)
{
  SendHead();
  SendAddr();
  MYUSART_SendData(0X01);
  MYUSART_SendData(0X00);
  MYUSART_SendData(0X00);
  Delay_ms(200);
  if(USART3_RX_STA & 0X8000) //���յ�����
  {
    if(//�ж��ǲ���ģ�鷵�ص�Ӧ���
      USART3_RX_BUF[0] == 0XEF
      && USART3_RX_BUF[1] == 0X01
      && USART3_RX_BUF[6] == 0X07
    )
    {
      *PS_Addr = (USART3_RX_BUF[2] << 24) + (USART3_RX_BUF[3] << 16)
                 + (USART3_RX_BUF[4] << 8) + (USART3_RX_BUF[5]);
      USART3_RX_STA = 0;
      return 0;
    }
    USART3_RX_STA = 0;
  }
  return 1;
}
//ģ��Ӧ���ȷ������Ϣ����
//���ܣ�����ȷ���������Ϣ������Ϣ
//����: ensure
const char *EnsureMessage(u8 ensure)
{
  const char *p;
  switch(ensure)
  {
  case  0x00:
    p = "       OK       ";
    break;
  case  0x01:
    p = " ���ݰ����մ��� ";
    break;
  case  0x02:
    p = "��������û����ָ";
    break;
  case  0x03:
    p = "¼��ָ��ͼ��ʧ��";
    break;
  case  0x04:
    p = " ָ��̫�ɻ�̫�� ";
    break;
  case  0x05:
    p = " ָ��̫ʪ��̫�� ";
    break;
  case  0x06:
    p = "  ָ��ͼ��̫��  ";
    break;
  case  0x07:
    p = " ָ��������̫�� ";
    break;
  case  0x08:
    p = "  ָ�Ʋ�ƥ��    ";
    break;
  case  0x09:
    p = " û��������ָ�� ";
    break;
  case  0x0a:
    p = "   �����ϲ�ʧ�� ";
    break;
  case  0x0b:
    p = "��ַ��ų�����Χ";
  case  0x10:
    p = "  ɾ��ģ��ʧ��  ";
    break;
  case  0x11:
    p = " ���ָ�ƿ�ʧ�� ";
    break;
  case  0x15:
    p = "������������Чͼ";
    break;
  case  0x18:
    p = " ��дFLASH����  ";
    break;
  case  0x19:
    p = "   δ�������   ";
    break;
  case  0x1a:
    p = "  ��Ч�Ĵ�����  ";
    break;
  case  0x1b:
    p = " �Ĵ������ݴ��� ";
    break;
  case  0x1c:
    p = " ���±�ҳ����� ";
    break;
  case  0x1f:
    p = "    ָ�ƿ���    ";
    break;
  case  0x20:
    p = "    ��ַ����    ";
    break;
  default :
    p = " ����ȷ�������� ";
    break;
  }
  return p;
}

//��ʾȷ���������Ϣ
void ShowErrMessage(u8 ensure)
{
  //OLED_ShowCH(5,0,(u8*)EnsureMessage(ensure));
}


//¼ָ��
void Add_FR(void)
{
  u8 i, ensure, processnum = 0;
  u8 ID_NUM = 0;
  while(1)
  {
    switch (processnum)
    {
    case 0:
      i++;
      OLED_ShowCH(0, 2,(u8*) "    �밴��ָ    ");
      ensure = PS_GetImage();
      if(ensure == 0x00)
      {
        ensure = PS_GenChar(CharBuffer1); //��������
        if(ensure == 0x00)
        {
          OLED_ShowCH(0, 2,(u8*) "    ָ������    ");
          OLED_ShowCH(0, 4, (u8*)"                ");
          i = 0;
          processnum = 1; //�����ڶ���
        }
        else ShowErrMessage(ensure);
      }
      else ShowErrMessage(ensure);
      break;

    case 1:
      i++;
      OLED_ShowCH(0, 2,(u8*) "   ���ٰ�һ��   ");
      OLED_ShowCH(0, 4,(u8*) "                ");
      ensure = PS_GetImage();
      if(ensure == 0x00)
      {
        ensure = PS_GenChar(CharBuffer2); //��������
        if(ensure == 0x00)
        {
          OLED_ShowCH(0, 2,(u8*) "    ָ������    ");
          OLED_ShowCH(0, 4, (u8*)"                ");
          i = 0;
          processnum = 2; //����������
        }
        else ShowErrMessage(ensure);
      }
      else ShowErrMessage(ensure);
      break;

    case 2:
      OLED_ShowCH(0, 2, (u8*)"  �Ա�����ָ��  ");
      OLED_ShowCH(0, 4, (u8*)"                ");
      ensure = PS_Match();
      if(ensure == 0x00)
      {
        OLED_ShowCH(0, 2, (u8*)"    �Աȳɹ�    ");
        OLED_ShowCH(0, 4,(u8*) "                ");
        processnum = 3; //�������Ĳ�
      }
      else
      {
        OLED_ShowCH(0, 2, (u8*)"    �Ա�ʧ��    ");
        OLED_ShowCH(0, 4,(u8*) "                ");
        ShowErrMessage(ensure);
        i = 0;
        processnum = 0; //���ص�һ��
      }
      Delay_ms(500);
      break;

    case 3:
      OLED_ShowCH(0, 2,(u8*) "  ����ָ��ģ��  ");
      OLED_ShowCH(0, 4, (u8*)"                ");
      Delay_ms(500);
      ensure = PS_RegModel();
      if(ensure == 0x00)
      {
        OLED_ShowCH(0, 2, (u8*)"����ָ��ģ��ɹ�");
        OLED_ShowCH(0, 4,(u8*) "                ");
        processnum = 4; //�������岽
      }
      else
      {
        processnum = 0;
        ShowErrMessage(ensure);
      }
      Delay_ms(1000);
      break;

    case 4:
      OLED_ShowCH(0, 0, (u8*)" ��K4��,��K2�� ");
      OLED_ShowCH(0, 2,(u8*) "    ��K3����    ");
      OLED_ShowCH(0, 4,(u8*) "  0=< ID <=99   ");
      while(KeyVal != 3)
      {
		
		KeyVal=Matrix_Key_Scan();  //��������ͽ�����ѭ���޷���ȡ����
        if(KeyVal == 2)
        {

          if(ID_NUM > 0)
            ID_NUM--;
        }
        if(KeyVal == 4)
        {
			
          if(ID_NUM < 99)
            ID_NUM++;
        }
        OLED_ShowCH(40, 6, (u8*)"ID=");
        OLED_ShowNum(65, 6, ID_NUM, 2, 1);
      }
		
      ensure = PS_StoreChar(CharBuffer2, ID_NUM); //����ģ��
      if(ensure == 0x00)
      {
       
		OLED_Clear();
        OLED_ShowCH(0, 2, (u8*)"  ¼��ָ�Ƴɹ�  ");
        OLED_ShowCH(0, 4,(u8*) "                ");
        Delay_ms(1500);
		OLED_Clear();
        OLED_ShowCH(0, 0, (u8*)"ָ��ģ����Գ���");
        OLED_ShowCH(16, 2,(u8*) "K1������ָ��");
        OLED_ShowCH(16, 4,(u8*) "K3��ɾ��ָ��");
        OLED_ShowCH(16, 6, (u8*)"K5����ָ֤��");
        KeyVal=KEY_ERR;
		
		return ;
      }
      else
      {
        OLED_Clear();
        processnum = 0;
        ShowErrMessage(ensure);
      }
      break;
    }
    Delay_ms(400);
    if(i == 10) //����5��û�а���ָ���˳�
    {
      break;
    }
  }
}

SysPara AS608Para;//ָ��ģ��AS608����
//ˢָ��
void press_FR(void)
{
  SearchResult seach;
  u8 ensure;
  char str[20];
  while(KeyVal != '*')
  {
//    KeyVal = KEY_Scan(0);
    ensure = PS_GetImage();
    if(ensure == 0x00) //��ȡͼ��ɹ�
    {
      ensure = PS_GenChar(CharBuffer1);
      if(ensure == 0x00) //���������ɹ�
      {
        ensure = PS_HighSpeedSearch(CharBuffer1, 0, 99, &seach);
        if(ensure == 0x00) //�����ɹ�
        {
          OLED_ShowCH(0, 2, (u8*)"  ָ����֤�ɹ�  ");
          //sprintf(str, " ID:%d �÷�:%d ", seach.pageID, seach.mathscore);
					sprintf(str, " ID:%d ", seach.pageID);
          OLED_ShowCH(0, 4, (u8*)str);
			Servo_SetAngle(180);
			Delay_ms(8000);
			Servo_SetAngle(0);
			MEAU=0;
			
		goto MENU ; //������ҳ��
        }
        else
        {
          OLED_ShowCH(32, 2, (u8*)"��֤ʧ��");
          Delay_ms(1500);
        }
      }
      else
			{};
      OLED_Clear();
      OLED_ShowCH(32, 2,(u8*) "�밴��ָ"); 
    }
	LED2_Turn();KeyVal=Matrix_Key_Scan();
  }
  MENU:
  		OLED_Clear();
		OLED_ShowCH(0,0,(u8*)"K1 ָ��  ");
		OLED_ShowCH(0,2,(u8*)"K3 ˢ��  ");	
		OLED_ShowCH(0,4,(u8*)"K6 ����  ");
		OLED_ShowCH(0,6,(u8*)"* ����	 ");
//  OLED_Clear();
//  OLED_ShowCH(0, 0,(u8*) "ָ��ģ����Գ���");
//  OLED_ShowCH(16, 2,(u8*) "K1������ָ��");
//  OLED_ShowCH(16, 4,(u8*) "K3��ɾ��ָ��");
//  OLED_ShowCH(16, 6,(u8*) "K5����ָ֤��");

}

//ɾ��ָ��
void Del_FR(void)
{
  KeyVal=Matrix_Key_Scan();
  u8  ensure;
  u16 ID_NUM = 0;
  OLED_ShowCH(0, 0, (u8*)"K4�� K2�� K3ȷ��");
  OLED_ShowCH(0, 2,(u8*) "  K5���ָ�ƿ�  ");
  OLED_ShowCH(0, 4, (u8*)"K1����0=<ID<=99");
  
  while(KeyVal != 3)
  {
	
	 KeyVal=Matrix_Key_Scan();
//	KeyNum= 0;
    if(KeyVal == 2)
    {
//KeyNum=0;
      if(ID_NUM > 0)
        ID_NUM--;
    }
    if(KeyVal == 4)
    {
//KeyNum=0;
      if(ID_NUM < 99)
        ID_NUM++;
    }
    if(KeyVal == 1)
      goto MENU ; //������ҳ��
    if(KeyVal == 5)
    {
//KeyNum=0;
      ensure = PS_Empty(); //���ָ�ƿ�
      if(ensure == 0)
      {
        OLED_Clear();
        OLED_ShowCH(0, 2,(u8*) " ���ָ�ƿ�ɹ� ");
      }
      else
        ShowErrMessage(ensure);
      Delay_ms(1500);
      goto MENU ; //������ҳ��
    }
    OLED_ShowCH(40, 6, (u8*)"ID=");
    OLED_ShowNum(65, 6, ID_NUM, 2, 1);
  }
  
  KeyVal=Matrix_Key_Scan();  
  
  ensure = PS_DeletChar(ID_NUM, 1); //ɾ������ָ��
  if(ensure == 0)
  {
    OLED_Clear();
    OLED_ShowCH(0, 2,(u8*) "  ɾ��ָ�Ƴɹ�  ");
  }
  else
    ShowErrMessage(ensure);
  Delay_ms(1500);
MENU:
  OLED_Clear();
  OLED_ShowCH(0, 0, (u8*)"ָ��ģ����Գ���");
  OLED_ShowCH(16, 2,(u8*) "K1������ָ��");
  OLED_ShowCH(16, 4,(u8*) "K3��ɾ��ָ��");
  OLED_ShowCH(16, 6, (u8*)"K5����ָ֤��");


}