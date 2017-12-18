#include "msp430f5529.h"
#include "oledfont.h"
#include "oled.h"

u8 OLED_GRAM[128][8];

//��ʼ��IIC
void IIC_Init(void) {

	SDA_OUT	;
	SCL_OUT	;
	SDA_H;
	SCL_H;
	//delay_ms(100);
}

void IIC_Start(void) {

	SDA_OUT;			//sda�����
	SCL_H;
	SDA_H;
	delay_us(1);
	SDA_L;				//START:when CLK is high,DATA change form high to low
	delay_us(1);
	SCL_L;				//ǯסI2C���ߣ�׼�����ͻ��������
}
//����IICֹͣ�ź�
void IIC_Stop(void) {

	SDA_OUT;			//sda�����
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

//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��
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

//��1���ֽڣ�ack=1ʱ������NACK��ack=0������ACK
unsigned char IIC_ReadByte(void) {

	unsigned char i,receive=0;
		SDA_IN;//SDA����Ϊ����
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
//��I2C�豸д��һ���ֽ�����
//**************************************
char IIC_Write(unsigned char REG_Address, unsigned char REG_data){

	IIC_Start();                  //��ʼ�ź�
	IIC_SendByte(SlaveAddress);   //�����豸��ַ+д�ź�
	IIC_Wait_Ack();	//�ȴ�Ӧ��
	IIC_SendByte(REG_Address);	//д�Ĵ�����ַ
	IIC_SendByte(REG_data);//��������
	IIC_Wait_Ack();
	IIC_Stop();
	return 0;
}
//**************************************
//��I2C�豸��ȡһ���ֽ�����
//**************************************
unsigned char IIC_Read(unsigned char REG_Address){

	unsigned char data;
	IIC_Start();                   //��ʼ�ź�
	IIC_SendByte(SlaveAddress);    //�����豸��ַ+д�ź�
	IIC_Wait_Ack();
	IIC_SendByte(REG_Address);     //���ʹ洢��Ԫ��ַ����0��ʼ
	//IIC_Wait_Ack();
	IIC_Start();                   //��ʼ�ź�
	IIC_SendByte(SlaveAddress+ 1);  //�����豸��ַ+���ź�
	//IIC_Wait_Ack();
	data = IIC_ReadByte();       //�����Ĵ�������
	IIC_SendAck(1);
	IIC_Stop();                    //ֹͣ�ź�
	return data;
}
/*****************************************************************************

����: LED_WrDat
��Ҫ: ��OLEDд����
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

���� : LED_WrCmd
��Ҫ : ��OLEDд����
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

/*LCDģ���ʼ��*/
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
//OLED���Դ�
//��Ÿ�ʽ����.
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
//����
//x:0~127
//y:0~63
//t:1 ��� 0,���
void OLED_DrawPoint(u8 x,u8 y,u8 t)
{
	u8 pos,bx,temp=0;
	if(x>127||y>63)return;//������Χ��.
	pos=7-y/8;
	bx=y%8;
	temp=1<<(7-bx);
	if(t)OLED_GRAM[x][pos]|=temp;
	else OLED_GRAM[x][pos]&=~temp;
	//OLED_RefleshGram();
}
//x1,y1,x2,y2 �������ĶԽ�����
//ȷ��x1<=x2;y1<=y2 0<=x1<=127 0<=y1<=63
//dot:0,���;1,���
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot)
{
	u8 x,y;
	for(x=x1;x<=x2;x++)
	{
		for(y=y1;y<=y2;y++)
			OLED_DrawPoint(x,y,dot);
	}
	OLED_RefleshGram();		//������ʾ
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

//��������
void OLED_Set_Pos(unsigned char x, unsigned char y)
{
	WriteCommand_oled(0xb0 + y);
	WriteCommand_oled(((x & 0xf0) >> 4) | 0x10);
	WriteCommand_oled((x & 0x0f));
}
//����OLED��ʾ
void OLED_Display_On(void)
{
	WriteCommand_oled(0X8D);  //SET DCDC����
	WriteCommand_oled(0X14);  //DCDC ON
	WriteCommand_oled(0XAF);  //DISPLAY ON
}
//�ر�OLED��ʾ
void OLED_Display_Off(void)
{
	WriteCommand_oled(0X8D);  //SET DCDC����
	WriteCommand_oled(0X10);  //DCDC OFF
	WriteCommand_oled(0XAE);  //DISPLAY OFF
}


//��������,������,������Ļ�Ǻ�ɫ��!��û����һ��!!!
void OLED_Clear(void)
{
	u8 i, n;
	for (i = 0; i<8; i++)
	{
		WriteCommand_oled(0xb0 + i);    //����ҳ��ַ��0~7��
		WriteCommand_oled(0x00);      //������ʾλ�á��е͵�ַ
		WriteCommand_oled(0x10);      //������ʾλ�á��иߵ�ַ
		for (n = 0; n<128; n++)
			OLED_GRAM[n][i]=0X00;
	} //������ʾ
	OLED_RefleshGram();
}

void OLED_On(void)
{
	u8 i, n;
	for (i = 0; i<8; i++)
	{
		WriteCommand_oled(0xb0 + i);    //����ҳ��ַ��0~7��
		WriteCommand_oled(0x00);		 //������ʾλ�á��е͵�ַ
		WriteCommand_oled(0x10);		 //������ʾλ�á��иߵ�ַ
		for (n = 0; n<128; n++)
			WriteData_oled(1);
	} //������ʾ
}

//��ָ��λ����ʾһ���ַ�,���������ַ�
//x:0~127
//y:0~63
//mode:0,������ʾ;1,������ʾ
void OLED_ShowChar(u8 x, u8 y, u8 chr, u8 size, u8 mode)
{
	u8 i;
	chr = chr - ' ';//�õ�ƫ�ƺ��ֵ
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
//m^n����
u32 oled_pow(u8 m, u8 n)
{
	u32 result = 1;
	while (n--)	result *= m;
	return result;
}

//��ʾ2������
//x,y :�������
//len :���ֵ�λ��
//size:�����С
//mode:ģʽ	0,���ģʽ;1,����ģʽ
//num:��ֵ(0~4294967295);
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

//��ʾһ���ַ��Ŵ�
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

//��ʾ����
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

	OLED_ShowCHinese(16,2,0);	// ĳ
	OLED_ShowCHinese(32,2,1);	// ��
	OLED_ShowCHinese(48,2,2);	// ��
	OLED_ShowCHinese(64,2,3);	// ѧ
	OLED_ShowCHinese(80,2,4);	// ��
	OLED_ShowCHinese(96,2,5);	// ��
	OLED_ShowCHinese(40,5,6);	// ��
	OLED_ShowCHinese(56,5,7);	// ��
	OLED_ShowCHinese(72,5,8);	// ��
}
//
void choose(unsigned char mode){
	switch(mode){
	case 0://����
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
	case 1://��
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
	case 2://�㷨
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
	case 3://����
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
	case 4://ȡ��
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
	case 5://����
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
	case 6://����
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
	case 7://ȡ��
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
	case 10://ָ��
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

	case 11://����
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

