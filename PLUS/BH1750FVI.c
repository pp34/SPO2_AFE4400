#include "oled.h"
#include "BH1750FVI.h"

#define BH1750FVI   0x46
#define BH1750_ON   0x01
#define BH1750_CON  0x10
#define BH1750_RSET 0x07
#define BH1750_ONE  0x11


void BH1750FVI_Init(void) {
	BH_Write(BH1750_ON);
	BH_Write(BH1750_RSET);
	BH_Write(BH1750_ONE);
}

void Start_BH1750(void) {
	BH_Write(BH1750_ON);
	BH_Write(BH1750_ONE);
//	delay_ms(200);
}

int Read_BH1750FVI(void) {
	u8 buf[3];
	IIC_Start();
	IIC_SendByte(BH1750FVI + 1);
	IIC_Wait_Ack();
	//while (IIC_Wait_Ack());
	buf[0] = IIC_ReadByte();
	IIC_SendAck(0);
	buf[1] = IIC_ReadByte();
	IIC_SendAck(0);
	buf[2] = IIC_ReadByte();
	IIC_SendAck(1);
	IIC_Stop();
	return ((buf[0] << 8) | buf[1]);
}

char BH_Write( unsigned char REG_Address) {
	IIC_Start();                  //起始信号
	IIC_SendByte(BH1750FVI);   //发送设备地址+写信号
	IIC_Wait_Ack();				  //等待应答
	IIC_SendByte(REG_Address);	  //写寄存器地址
	IIC_Wait_Ack();
	IIC_Stop();
	return 0;
}
