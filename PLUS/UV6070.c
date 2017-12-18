/*
 * UV6070.c
 *
 *  Created on: 2016Äê8ÔÂ24ÈÕ
 *      Author: PP
 */
#include "oled.h"
#include "UV6070.h"

#define UVADDR 0x70
//Integration Time
#define IT_1_2 0x0 //1/2T
#define IT_1   0x1 //1T
#define IT_2   0x2 //2T
#define IT_4   0x3 //4T
void UV_Init(void){
	IIC_Start();
	IIC_SendByte(UVADDR);
	IIC_SendByte(IT_1<<2|0X02);
	IIC_Stop();
}
int UV_Read(void){
	u8 buf[2];
	IIC_Start();
	IIC_SendByte(0x73);
	IIC_Wait_Ack();
	buf[0] = IIC_ReadByte();
	IIC_SendAck(1);
	IIC_Stop();
	IIC_Start();
	IIC_SendByte(0x71);
	IIC_Wait_Ack();
	buf[1] = IIC_ReadByte();
	IIC_SendAck(1);
	IIC_Stop();
	return (buf[0]<<8|buf[1]);
}
