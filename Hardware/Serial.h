#ifndef __SERIAL_H
#define __SERIAL_H

#include <stdio.h>
#include <stdint.h>

extern uint8_t Serial_TxPacket[];
extern uint8_t Serial_RxPacket[]; //蓝牙收到的数据存入全局变量内存中

void Serial_Init(void);
void Serial_SendByte(uint8_t Byte);//蓝牙用于串口打印到串口助手
void Serial2_SendByte(uint8_t Byte);
void Serial_SendArray(uint8_t *Array, uint16_t Length);
void Serial_SendString(char *String);
void Serial_SendNumber(uint32_t Number, uint8_t Length);
void Serial_Printf(char *format, ...);

void Serial_SendPacket(void);
uint8_t Serial_GetRxFlag(void);

#endif
