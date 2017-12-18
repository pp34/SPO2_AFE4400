#include "msp430f5529.h"
#include "oledfont.h"
#include "oled.h"

u8 OLED_GRAM[128][8];

//初始化IIC
void IIC_Init(void) {

	SDA_OUT	;
	SCL_OUT	;
	SDA_H;
	SCL_H;
	//delay_ms(100);
}

void IIC_Start(void) {

	SDA_OUT;			//sda线输出
	SCL_H;
	SDA_H;
	delay_us(1);
	SDA_L;				//START:when CLK is high,DATA change form high to low
	delay_us(1);
	SCL_L;				//钳住I2C总线，准备发送或接收数据
}
//产生IIC停止信号
void IIC_Stop(void) {

	SDA_OUT;			//sda线输出
	SCL_H;
	SDA_L;			//STOP:when CLK is high DATA change form low to high
	delay_us(1);
	SDA_H;
	delay_us(1);
}
//	1:NACK; 0:ACK
void IIC_SendAck(unsigned char ack){

	SDA_OUT;
	if(ack)	SDA_H;
	else 		SDA_L;
	SCL_H;
	delay_us(1);
	SCL_L;
	delay_us(1);
}

void IIC_Wait_Ack(void){

	SCL_H;
	delay_us(1);
	SCL_L;
}

//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答
void IIC_SendByte(unsigned char txd) {

	unsigned char i;
	unsigned char m,da;
	da=txd;
	SCL_L;
	for(i=0;i<8;i++){
		m=da;
		m=m&0x80;
		if(m==0x80)
		{SDA_H;}
		else SDA_L;
				da=da<<1;
		SCL_H;
		delay_us(1);
		SCL_L;
	}
}

//读1个字节，ack=1时，发送NACK，ack=0，发送ACK
unsigned char IIC_ReadByte(void) {

	unsigned char i,receive=0;
		SDA_IN;//SDA设置为输入
	    for(i=0;i<8;i++ )
		{
	        SCL_L;
	        delay_us(1);
			SCL_H;
	        receive<<=1;
	        receive |=SDA;
			delay_us(1);
	    }
	    return receive;
}

//**************************************
//向I2C设备写入一个字节数据
//**************************************
char IIC_Write(unsigned char REG_Address, unsigned char REG_data){

	IIC_Start();                  //起始信号
	IIC_SendByte(SlaveAddress);   //发送设备地址+写信号
	IIC_Wait_Ack();	//等待应答
	IIC_SendByte(REG_Address);	//写寄存器地址
	IIC_SendByte(REG_data);//发送数据
	IIC_Wait_Ack();
	IIC_Stop();
	return 0;
}
//**************************************
//从I2C设备读取一个字节数据
//**************************************
unsigned char IIC_Read(unsigned char REG_Address){

	unsigned char data;
	IIC_Start();                   //起始信号
	IIC_SendByte(SlaveAddress);    //发送设备地址+写信号
	IIC_Wait_Ack();
	IIC_SendByte(REG_Address);     //发送存储单元地址，从0开始
	//IIC_Wait_Ack();
	IIC_Start();                   //起始信号
	IIC_SendByte(SlaveAddress+ 1);  //发送设备地址+读信号
	//IIC_Wait_Ack();
	data = IIC_ReadByte();       //读出寄存器数据
	IIC_SendAck(1);
	IIC_Stop();                    //停止信号
	return data;
}
/*****************************************************************************

名称: LED_WrDat
简要: 向OLED写数据
*****************************************************************************/
void WriteData_oled(unsigned char ucData){
	IIC_Start();
	IIC_SendByte(0x78);
	IIC_Wait_Ack();
	IIC_SendByte(0x40);//write data
	IIC_Wait_Ack();
	IIC_SendByte(ucData);
	IIC_Wait_Ack();
	IIC_Stop();
}
/*****************************************************************************

名称 : LED_WrCmd
简要 : 向OLED写命令
*****************************************************************************/

void WriteCommand_oled(unsigned char ucCmd){
	IIC_Start();
	IIC_SendByte(0x78); //Slave address,SA0=0
	IIC_Wait_Ack();
	IIC_SendByte(0x00);//write command
	IIC_Wait_Ack();
	IIC_SendByte(ucCmd);
	IIC_Wait_Ack();
	IIC_Stop();
}

/*LCD模块初始化*/
void Oled_Init(void){
	delay_ms(500);
	WriteCommand_oled(0xAE);	//display off
	WriteCommand_oled(0x00);	//---set low column address
	WriteCommand_oled(0x10);	//00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
	WriteCommand_oled(0x40);	//--set start line address
	WriteCommand_oled(0xb0);	//Set Page Start Address for Page Addressing Mode,0-7
	WriteCommand_oled(0x81);	//--set contrast control register
	WriteCommand_oled(0xff);		//--128
	WriteCommand_oled(0xa1);	//--set segment re-map 0 to 127
	WriteCommand_oled(0xa6);	//--set normal display
	WriteCommand_oled(0xa8);	//--set multiplex ratio(1 to 64)
	WriteCommand_oled(0x3F);		//
	WriteCommand_oled(0xc8);		//Set COM Output Scan Direction
	WriteCommand_oled(0xd3);	//-set display offset
	WriteCommand_oled(0x00);	//---set low column address

	WriteCommand_oled(0xd5);	//--set display clock divide ratio/oscillator frequency
	WriteCommand_oled(0x80);

	WriteCommand_oled(0xd8);	//set area color mode off
	WriteCommand_oled(0x05);

	WriteCommand_oled(0xd9);	//Set Pre-Charge Period
	WriteCommand_oled(0xf1);

	WriteCommand_oled(0xda);	//set com pin configuartion
	WriteCommand_oled(0x12);

	WriteCommand_oled(0xdb);	//set Vcomh
	WriteCommand_oled(0x30);

	WriteCommand_oled(0x8d);	//set charge pump enable
	WriteCommand_oled(0x14);

	WriteCommand_oled(0xaf);	//--turn on oled panel
}
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
void OLED_RefleshGram(void){

	u8 i=0;
	u8 n=0;
	for(i=0;i<8;i++){
		WriteCommand_oled(0xb0+i);
		WriteCommand_oled(0x00);
		WriteCommand_oled(0x10);
		for(n=0;n<128;n++){
			WriteData_oled(OLED_GRAM[n][i]);
		}
	}
}

void OLED_Reflesharea(u8 x1, u8 y1, u8 x2, u8 y2){

	u8 i=0;
	u8 n=0;
	for(i=y1;i<y2;i++){
		WriteCommand_oled(0xb0+i);
		WriteCommand_oled(0x00);
		WriteCommand_oled(0x10);
		for(n=x1;n<x2;n++){
			WriteData_oled(OLED_GRAM[n][i]);
		}
	}
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
	//OLED_RefleshGram();
}
//x1,y1,x2,y2 填充区域的对角坐标
//确保x1<=x2;y1<=y2 0<=x1<=127 0<=y1<=63
//dot:0,清空;1,填充
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot)
{
	u8 x,y;
	for(x=x1;x<=x2;x++)
	{
		for(y=y1;y<=y2;y++)
			OLED_DrawPoint(x,y,dot);
	}
	OLED_RefleshGram();		//更新显示
}

/********************************************
// fill_Picture
********************************************/
void fill_picture(unsigned char m, unsigned char fill_Data){

	unsigned char n;
	//for (m = 0; m<8; m++){
		WriteCommand_oled(0xb0 + m);		// page0-page1
		WriteCommand_oled(0x0b);			// low column start address
		WriteCommand_oled(0x13);			// high column start address
		for (n = 0; n<128; n++){
			WriteData_oled(fill_Data);
		}
//	}
}

//坐标设置
void OLED_Set_Pos(unsigned char x, unsigned char y)
{
	WriteCommand_oled(0xb0 + y);
	WriteCommand_oled(((x & 0xf0) >> 4) | 0x10);
	WriteCommand_oled((x & 0x0f));
}
//开启OLED显示
void OLED_Display_On(void)
{
	WriteCommand_oled(0X8D);  //SET DCDC命令
	WriteCommand_oled(0X14);  //DCDC ON
	WriteCommand_oled(0XAF);  //DISPLAY ON
}
//关闭OLED显示
void OLED_Display_Off(void)
{
	WriteCommand_oled(0X8D);  //SET DCDC命令
	WriteCommand_oled(0X10);  //DCDC OFF
	WriteCommand_oled(0XAE);  //DISPLAY OFF
}


//清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!
void OLED_Clear(void)
{
	u8 i, n;
	for (i = 0; i<8; i++)
	{
		WriteCommand_oled(0xb0 + i);    //设置页地址（0~7）
		WriteCommand_oled(0x00);      //设置显示位置—列低地址
		WriteCommand_oled(0x10);      //设置显示位置—列高地址
		for (n = 0; n<128; n++)
			OLED_GRAM[n][i]=0X00;
	} //更新显示
	OLED_RefleshGram();
}

void OLED_On(void)
{
	u8 i, n;
	for (i = 0; i<8; i++)
	{
		WriteCommand_oled(0xb0 + i);    //设置页地址（0~7）
		WriteCommand_oled(0x00);		 //设置显示位置—列低地址
		WriteCommand_oled(0x10);		 //设置显示位置—列高地址
		for (n = 0; n<128; n++)
			WriteData_oled(1);
	} //更新显示
}

//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//mode:0,反白显示;1,正常显示
void OLED_ShowChar(u8 x, u8 y, u8 chr, u8 size, u8 mode)
{
	u8 i;
	chr = chr - ' ';//得到偏移后的值
	if (x>Max_Column - 1) { x = 0; y = y + 2; }

	if (size == 16) {
		OLED_Set_Pos(x, y);
		for (i = 0; i<8; i++)
			WriteData_oled(F8X16[chr * 16 + i]);
		OLED_Set_Pos(x, y + 1);
		for (i = 0; i<8; i++)
			WriteData_oled(F8X16[chr * 16 + i + 8]);
	}
}
//m^n函数
u32 oled_pow(u8 m, u8 n)
{
	u32 result = 1;
	while (n--)	result *= m;
	return result;
}

//显示2个数字
//x,y :起点坐标
//len :数字的位数
//size:字体大小
//mode:模式	0,填充模式;1,叠加模式
//num:数值(0~4294967295);
void OLED_ShowNum(u8 x, u8 y, u32 num, u8 len, u8 size2)
{
	u8 t, temp;
	u8 enshow = 0;
	for (t = 0; t<len; t++)
	{
		temp = (num / oled_pow(10, len - t - 1)) % 10;
		if (enshow == 0 && t<(len - 1))
		{
			if (temp == 0)
			{
				OLED_ShowChar(x + (size2 / 2)*t, y, ' ', size2,1);
				continue;
			}
			else enshow = 1;

		}
		OLED_ShowChar(x + (size2 / 2)*t, y, temp + '0', size2,1);
	}
}

//显示一个字符号串
void OLED_ShowString(u8 x, u8 y, u8 *chr, u8 Char_Size)
{
	unsigned char j = 0;
	while (chr[j] != '\0')
	{
		OLED_ShowChar(x, y, chr[j], Char_Size,1);
		x += 8;
		if (x>120) { x = 0; y += 2; }
		j++;
	}
}

//显示汉字
void OLED_ShowCHinese(u8 x, u8 y, u8 no)
{
	u8 t;
	OLED_Set_Pos(x, y);
	for (t = 0; t<16; t++)
	{
		WriteData_oled(Hzk[2 * no][t]);

	}
	OLED_Set_Pos(x, y + 1);
	for (t = 0; t<16; t++)
	{
		WriteData_oled(Hzk[2 * no + 1][t]);

	}
}

// show our team
void	team_name(void){

	OLED_ShowCHinese(16,2,0);	// 某
	OLED_ShowCHinese(32,2,1);	// 不
	OLED_ShowCHinese(48,2,2);	// 科
	OLED_ShowCHinese(64,2,3);	// 学
	OLED_ShowCHinese(80,2,4);	// 的
	OLED_ShowCHinese(96,2,5);	// 电
	OLED_ShowCHinese(40,5,6);	// 赛
	OLED_ShowCHinese(56,5,7);	// 队
	OLED_ShowCHinese(72,5,8);	// 伍
}
//
void choose(unsigned char mode){
	switch(mode){
	case 0://本机
		OLED_Clear();
		OLED_ShowNum(2,0,1,2,16);
		OLED_ShowChar(18, 0, ':', 16,1);
		OLED_ShowNum(2,3,2,2,16);
		OLED_ShowChar(18,3, ':', 16,1);
		OLED_ShowNum(2,6,3,2,16);
		OLED_ShowChar(18,6, ':', 16,1);
		//OLED_RefleshGram();

	    OLED_ShowCHinese(30,0,29);
		OLED_ShowCHinese(46,0,30);
		OLED_ShowCHinese(62,0,31);
		OLED_ShowCHinese(78,0,32);


		OLED_ShowCHinese(30,3,13);
		OLED_ShowCHinese(46,3,14);
		OLED_ShowCHinese(62,3,11);
		OLED_ShowCHinese(78,3,12);


		OLED_ShowCHinese(30,6,15);
		OLED_ShowCHinese(46,6,16);
		OLED_ShowCHinese(62,6,17);
		OLED_ShowCHinese(78,6,18);

		break;
	case 1://云
		OLED_Clear();
		OLED_ShowNum(2,0,1,2,16);
		OLED_ShowChar(18, 0, ':', 16,1);
		OLED_ShowCHinese(30,0,9);
		OLED_ShowCHinese(46,0,10);
		OLED_ShowCHinese(62,0,11);
		OLED_ShowCHinese(78,0,12);

		OLED_ShowNum(2,3,2,2,16);
		OLED_ShowChar(18,3, ':', 16,1);
		OLED_ShowCHinese(30,3,33);
		OLED_ShowCHinese(46,3,34);
		OLED_ShowCHinese(62,3,31);
		OLED_ShowCHinese(78,3,32);

		OLED_ShowNum(2,6,3,2,16);
		OLED_ShowChar(18,6, ':', 16,1);
		OLED_ShowCHinese(30,6,15);
		OLED_ShowCHinese(46,6,16);
		OLED_ShowCHinese(62,6,17);
		OLED_ShowCHinese(78,6,18);
		break;
	case 2://算法
		OLED_Clear();
		OLED_ShowNum(1,0,1,2,16);
		OLED_ShowChar(16, 0, ':', 16,1);
		OLED_ShowCHinese(30,0,9);
		OLED_ShowCHinese(46,0,10);
		OLED_ShowCHinese(62,0,11);
		OLED_ShowCHinese(78,0,12);

		OLED_ShowNum(1,3,2,2,16);
		OLED_ShowChar(16,3, ':', 16,1);
		OLED_ShowCHinese(30,3,13);
		OLED_ShowCHinese(46,3,14);
		OLED_ShowCHinese(62,3,11);
		OLED_ShowCHinese(78,3,12);

		OLED_ShowNum(2,6,3,2,16);
		OLED_ShowChar(18,6, ':', 16,1);
		OLED_ShowCHinese(30,6,35);
		OLED_ShowCHinese(46,6,36);
		OLED_ShowCHinese(62,6,37);
		OLED_ShowCHinese(78,6,38);
		break;
	case 3://附加
			OLED_Clear();
			OLED_ShowNum(2,0,2,2,16);
			OLED_ShowChar(18, 0, ':', 16,1);
			OLED_ShowCHinese(30,0,13);
			OLED_ShowCHinese(46,0,14);
			OLED_ShowCHinese(62,0,11);
			OLED_ShowCHinese(78,0,12);

			OLED_ShowNum(2,3,3,2,16);
			OLED_ShowChar(18,3, ':', 16,1);
			OLED_ShowCHinese(30,3,15);
			OLED_ShowCHinese(46,3,16);
			OLED_ShowCHinese(62,3,17);
			OLED_ShowCHinese(78,3,18);

			OLED_ShowNum(2,6,4,2,16);
			OLED_ShowChar(18,6, ':', 16,1);
			OLED_ShowCHinese(30,6,39);
			OLED_ShowCHinese(46,6,40);
			OLED_ShowCHinese(62,6,41);
			OLED_ShowCHinese(78,6,42);
			break;
	case 4://取消
				OLED_Clear();
				OLED_ShowNum(2,0,3,2,16);
				OLED_ShowChar(18, 0, ':', 16,1);
				OLED_ShowCHinese(30,0,15);
				OLED_ShowCHinese(46,0,16);
				OLED_ShowCHinese(62,0,17);
				OLED_ShowCHinese(78,0,18);

				OLED_ShowNum(2,3,4,2,16);
				OLED_ShowChar(18,3, ':', 16,1);
				OLED_ShowCHinese(30,3,19);
				OLED_ShowCHinese(46,3,20);
				OLED_ShowCHinese(62,3,21);
				OLED_ShowCHinese(78,3,22);

				OLED_ShowNum(2,6,5,2,16);
				OLED_ShowChar(18,6, ':', 16,1);
				OLED_ShowCHinese(30,6,47);
				OLED_ShowCHinese(46,6,48);
				break;
	case 5://波形
		OLED_Clear();
		OLED_ShowNum(2, 0, 1, 2, 16);
		OLED_ShowChar(18, 0, ':', 16, 1);
		OLED_ShowCHinese(30, 0, 43);
		OLED_ShowCHinese(46, 0, 44);
		OLED_ShowCHinese(62, 0, 31);
		OLED_ShowCHinese(78, 0, 32);

		OLED_ShowNum(2, 3, 2, 2, 16);
		OLED_ShowChar(18, 3, ':', 16, 1);
		OLED_ShowCHinese(30, 3, 25);
		OLED_ShowCHinese(46, 3, 26);
		OLED_ShowCHinese(62, 3, 11);
		OLED_ShowCHinese(78, 3, 12);

		OLED_ShowNum(2, 6, 3, 2, 16);
		OLED_ShowChar(18, 6, ':', 16, 1);
		OLED_ShowCHinese(30, 6, 27);
		OLED_ShowCHinese(46, 6, 28);
		break;
	case 6://数字
		OLED_Clear();
		OLED_ShowNum(2, 0, 1, 2, 16);
		OLED_ShowChar(18, 0, ':', 16, 1);
		OLED_ShowCHinese(30, 0, 23);
		OLED_ShowCHinese(46, 0, 24);
		OLED_ShowCHinese(62, 0, 11);
		OLED_ShowCHinese(78, 0, 12);

		OLED_ShowNum(2, 3, 2, 2, 16);
		OLED_ShowChar(18, 3, ':', 16, 1);
		OLED_ShowCHinese(30, 3, 45);
		OLED_ShowCHinese(46, 3, 46);
		OLED_ShowCHinese(62, 3, 31);
		OLED_ShowCHinese(78, 3, 32);

		OLED_ShowNum(2, 6, 3, 2, 16);
		OLED_ShowChar(18, 6, ':', 16, 1);
		OLED_ShowCHinese(30, 6, 27);
		OLED_ShowCHinese(46, 6, 28);
		break;
	case 7://取消
		OLED_Clear();
		OLED_ShowNum(2, 0, 1, 2, 16);
		OLED_ShowChar(18, 0, ':', 16, 1);
		OLED_ShowCHinese(30, 0, 23);
		OLED_ShowCHinese(46, 0, 24);
		OLED_ShowCHinese(62, 0, 11);
		OLED_ShowCHinese(78, 0, 12);

		OLED_ShowNum(2, 3, 2, 2, 16);
		OLED_ShowChar(18, 3, ':', 16, 1);
		OLED_ShowCHinese(30, 3, 25);
		OLED_ShowCHinese(46, 3, 26);
		OLED_ShowCHinese(62, 3, 11);
		OLED_ShowCHinese(78, 3, 12);

		OLED_ShowNum(2, 6, 3, 2, 16);
		OLED_ShowChar(18, 6, ':', 16, 1);
		OLED_ShowCHinese(30, 6, 47);
		OLED_ShowCHinese(46, 6, 48);
		break;
	case 8://sec
		OLED_Clear();
		OLED_ShowNum(2, 0, 1, 2, 16);
		OLED_ShowChar(18, 0, ':', 16, 1);
		OLED_ShowCHinese(30, 0, 23);
		OLED_ShowCHinese(46, 0, 24);
		OLED_ShowCHinese(62, 0, 11);
		OLED_ShowCHinese(78, 0, 12);

		OLED_ShowNum(2, 3, 2, 2, 16);
		OLED_ShowChar(18, 3, ':', 16, 1);
		OLED_ShowCHinese(30, 3, 25);
		OLED_ShowCHinese(46, 3, 26);
		OLED_ShowCHinese(62, 3, 11);
		OLED_ShowCHinese(78, 3, 12);

		OLED_ShowNum(2, 6, 3, 2, 16);
		OLED_ShowChar(18, 6, ':', 16, 1);
		OLED_ShowCHinese(30, 6, 27);
		OLED_ShowCHinese(46, 6, 28);
		break;
	case 9://one
		OLED_Clear();
		OLED_ShowNum(2, 0, 1, 2, 16);
		OLED_ShowChar(18, 0, ':', 16, 1);
		OLED_ShowNum(2, 3, 2, 2, 16);
		OLED_ShowChar(18, 3, ':', 16, 1);
		OLED_ShowNum(2, 6, 3, 2, 16);
		OLED_ShowChar(18, 6, ':', 16, 1);

		OLED_ShowCHinese(30, 0, 9);
		OLED_ShowCHinese(46, 0, 10);
		OLED_ShowCHinese(62, 0, 11);
		OLED_ShowCHinese(78, 0, 12);


		OLED_ShowCHinese(30, 3, 13);
		OLED_ShowCHinese(46, 3, 14);
		OLED_ShowCHinese(62, 3, 11);
		OLED_ShowCHinese(78, 3, 12);


		OLED_ShowCHinese(30, 6, 15);
		OLED_ShowCHinese(46, 6, 16);
		OLED_ShowCHinese(62, 6, 17);
		OLED_ShowCHinese(78, 6, 18);
		break;
	case 10://指架
		OLED_Clear();
		OLED_ShowNum(2, 0, 1, 2, 16);
		OLED_ShowChar(18, 0, ':', 16, 1);
		OLED_ShowNum(2, 3, 2, 2, 16);
		OLED_ShowChar(18, 3, ':', 16, 1);
		OLED_ShowNum(2, 6, 3, 2, 16);
		OLED_ShowChar(18, 6, ':', 16, 1);

		OLED_ShowCHinese(30, 0, 57);
		OLED_ShowCHinese(46, 0, 58);
		OLED_ShowCHinese(62, 0, 59);

		OLED_ShowCHinese(30, 3, 60);
		OLED_ShowCHinese(46, 3, 61);
		OLED_ShowCHinese(62, 3, 62);

		OLED_ShowCHinese(30, 6, 27);
		OLED_ShowCHinese(46, 6, 28);
		break;

	case 11://反射
		OLED_Clear();
		OLED_ShowNum(2, 0, 1, 2, 16);
		OLED_ShowChar(18, 0, ':', 16, 1);
		OLED_ShowNum(2, 3, 2, 2, 16);
		OLED_ShowChar(18, 3, ':', 16, 1);
		OLED_ShowNum(2, 6, 3, 2, 16);
		OLED_ShowChar(18, 6, ':', 16, 1);

		OLED_ShowCHinese(30, 0, 54);
		OLED_ShowCHinese(46, 0, 55);
		OLED_ShowCHinese(62, 0, 56);

		OLED_ShowCHinese(30, 3, 63);
		OLED_ShowCHinese(46, 3, 64);
		OLED_ShowCHinese(62, 3, 65);

		OLED_ShowCHinese(30, 6, 27);
		OLED_ShowCHinese(46, 6, 28);
		break;
	case 12://cancel
		OLED_Clear();
		OLED_ShowNum(2, 0, 1, 2, 16);
		OLED_ShowChar(18, 0, ':', 16, 1);
		OLED_ShowNum(2, 3, 2, 2, 16);
		OLED_ShowChar(18, 3, ':', 16, 1);
		OLED_ShowNum(2, 6, 3, 2, 16);
		OLED_ShowChar(18, 6, ':', 16, 1);

		OLED_ShowCHinese(30, 0, 54);
		OLED_ShowCHinese(46, 0, 55);
		OLED_ShowCHinese(62, 0, 56);

		OLED_ShowCHinese(30, 3, 60);
		OLED_ShowCHinese(46, 3, 61);
		OLED_ShowCHinese(62, 3, 62);

		OLED_ShowCHinese(30, 6, 47);
		OLED_ShowCHinese(46, 6, 48);
		break;
	case 13://cancel
		OLED_Clear();
		OLED_ShowNum(2, 0, 1, 2, 16);
		OLED_ShowChar(18, 0, ':', 16, 1);
		OLED_ShowNum(2, 3, 2, 2, 16);
		OLED_ShowChar(18, 3, ':', 16, 1);
		OLED_ShowNum(2, 6, 3, 2, 16);
		OLED_ShowChar(18, 6, ':', 16, 1);

		OLED_ShowCHinese(30, 0, 54);
		OLED_ShowCHinese(46, 0, 55);
		OLED_ShowCHinese(62, 0, 56);

		OLED_ShowCHinese(30, 3, 60);
		OLED_ShowCHinese(46, 3, 61);
		OLED_ShowCHinese(62, 3, 62);

		OLED_ShowCHinese(30, 6, 27);
		OLED_ShowCHinese(46, 6, 28);
		break;
	default:break;
	}
}

