#ifndef __OLED_H
#define __OLED_H
#include "stdlib.h"
#define OLED_MODE 	 0
#define SIZE 				 8
#define XLevelL				0x00
#define XLevelH			0x10
#define Max_Column		128
#define Max_Row			64
#define	Brightness			0xFF
#define X_WIDTH 			128
#define Y_WIDTH 			64

#define SCL_H     P3OUT |=  BIT1  		// SCL
#define SDA_H    P3OUT |=  BIT0  		// SDA
#define SCL_L      P3OUT &= ~BIT1    // SCL
#define SDA_L     P3OUT &= ~BIT0    // SDA
#define SDA_IN	  P3DIR &= ~BIT0
#define SDA_OUT	 P3DIR |=  BIT0
#define SCL_OUT	 P3DIR |=  BIT1
#define SDA			(P3IN&BIT0)

#define u8 unsigned char
#define u16 unsigned int
#define u32 unsigned long
#define CPUCLK  25000000		//8MHZ       ---一个周期0.125us
#define	 delay_us(us)		__delay_cycles((long)(CPUCLK*(double)us/1000000.0))
#define delay_ms(ms)	__delay_cycles((long)(CPUCLK*(double)ms/1000.0))
#define SlaveAddress 0x78
#define on 0xff
#define off 0x00
//IIC所有操作函数
void IIC_Init(void);                //初始化IIC的IO口
void IIC_Start(void);				//发送IIC开始信号
void IIC_Stop(void);	  			//发送IIC停止信号
void IIC_SendByte(unsigned char txd);
void IIC_Wait_Ack(void);
char IIC_Write(unsigned char REG_Address, unsigned char REG_data);
unsigned char IIC_Read(unsigned char REG_Address);
unsigned char IIC_ReadByte(void);
void IIC_SendAck(unsigned char ack);

//OLED控制用函数
void WriteData_oled(unsigned char ucData);
void WriteCommand_oled(unsigned char ucCmd);
void OLED_Display_On(void);
void OLED_Display_Off(void);
void Oled_Init(void);
void OLED_Clear(void);
void OLED_RefleshGram(void);
void OLED_Reflesharea(u8 x1, u8 y1, u8 x2, u8 y2);
void OLED_DrawPoint(u8 x, u8 y, u8 t);
void OLED_Fill(u8 x1, u8 y1, u8 x2, u8 y2, u8 dot);
void OLED_ShowChar(u8 x, u8 y, u8 chr, u8 size, u8 mode);
void OLED_ShowNum(u8 x, u8 y, u32 num, u8 len, u8 size);
void OLED_ShowString(u8 x, u8 y, u8 *p, u8 Char_Size);
void OLED_Set_Pos(unsigned char x, unsigned char y);
void OLED_ShowCHinese(u8 x, u8 y, u8 no);
void fill_picture(unsigned char m, unsigned char fill_Data);
void Picture(void);
void	team_name(void);
void choose(unsigned char mode);
#endif



