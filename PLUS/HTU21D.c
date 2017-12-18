/*
 * HTU21D.c
 *
 *  Created on: 2016Äê8ÔÂ24ÈÕ
 *      Author: PP
 */
#include "HTU21D.h"
#include "math.h"
#include "oled.h"
#include <msp430.h>
#define HTU21D_ADDR		0x80
#define T_NOMASTERHOLD	0xf3	//1111 0011
#define RH_NOMASTERHOLD	0xf5	//1111 0101

u8 IIC_Send(u8 Data){
	unsigned char i;
	SDA_OUT;
	SCL_L;
	//delay_us(5);
	for(i=0;i<8;i++){
		if(Data&0x80)	SDA_H;
		else					SDA_L;
		Data<<=1;
		delay_us(5);
		SCL_H;
		delay_us(5);
		SCL_L;
		delay_us(5);
	}
	SDA_H;
	delay_us(5);
	SCL_H;
	delay_us(5);
	SDA_IN;
	if(SDA){
		SCL_L;
		return 1;
	}
	else{
		SCL_L;
		return 0;
	}
}
void HTU21_Init(void){
	IIC_Start();
	IIC_SendByte(HTU21D_ADDR&0xfe);
	IIC_SendByte(0xfe);
	IIC_Stop();
}


int READ_RH(void) {
	long rh;
	u8 buf[2];
	IIC_Start();
	if(IIC_Send(HTU21D_ADDR)==0){
		if(IIC_Send(0xf5)==0){
			do{
				delay_ms(10);
				IIC_Start();
				}while(IIC_Send(HTU21D_ADDR+1)==1);
			buf[0]=IIC_ReadByte();
			IIC_SendAck(0);
			buf[1]=IIC_ReadByte();
			IIC_SendAck(1);
			IIC_Stop();
			rh=((buf[0]<<8)|buf[1])&0xFFFC;
			rh =(rh*125) >>4;
			rh=(rh/16)>>4;
			rh=rh/16 - 6;
			}
		}
	return rh;
}

int READ_T(void) {
	long t;
	u8 buf[2];
	IIC_Start();
	if(IIC_Send(HTU21D_ADDR)==0){
		if(IIC_Send(0xf3)==0){
			do{
				delay_ms(10);
				IIC_Start();
				}while(IIC_Send(HTU21D_ADDR+1)==1);
			buf[0]=IIC_ReadByte();
			IIC_SendAck(0);
			buf[1]=IIC_ReadByte();
			IIC_SendAck(1);
			IIC_Stop();
			t=((buf[0]<<8)|buf[1])&0xFFFC;
			t =(t*125) >>4;
			t=(t/16)>>4;
			t=t/16 - 6;
			}
		}
	return t-20;
}
