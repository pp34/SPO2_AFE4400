#include <msp430.h>
#include "math.h"
#include <intrinsics.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
//====MSP430电源管理====//
#include "HAL_UCS.h"
#include "HAL_PMM.h"
//=====模拟前端驱动=====//
#include "AFE4400.h"
//====显示功能头文件====//
#include "oled.h"
//====附加功能头文件====//
#include "BH1750FVI.h"
#include "HTU21D.h"
#include "UV6070.h"
/************一些参数的宏定义************/

//=======发送的校验符========//
#define CR					0x0D

//====计算队列长度====//
#define MAX_LENGTH 	128
//====窗口队列长度====//
#define WIN_LENGTH	32
//====IIR队列长度====//
#define IIR_NSEC			3
//====IIR滤波器结构体====//
typedef struct {
	long x1;	//x(n+1)
	long x0;	//x(n)
	long y1;	//y(n+1)
	long y2;	//y(n+2)
}IROLD;
typedef struct {
	long x1;	//x(n+1)
	long x0;	//x(n)
	long y1;	//y(n+1)
	long y2;	//y(n+2)
}RDOLD;
//====IIR FILTER 参数====//
const double IR_B[IIR_NSEC] = { 0.00782020 ,  0.01564041, 0.00782020 };
const double IR_A[IIR_NSEC] = { 1        , -1.73472576, 	0.76600660 };
const double RD_B[IIR_NSEC] = {0.00782020 ,  0.01564041, 0.00782020};
const double RD_A[IIR_NSEC] = {1        , -1.73472576, 	0.76600660 };
//========//
IROLD IR_Old;
RDOLD RD_Old;
/**********全局变量***********/

//====发送队列====//
unsigned char txString[13];

//====脉搏血氧信号====//
unsigned long IR_RAW=0;							// 红外光
unsigned long RD_RAW=0;						// 红光
long IR_Signal=0;										// 红外光，滤波后进入计算窗口
long RD_Signal=0;										// 红光  ， 滤波后进入计算窗口

//====血氧计算变量====//
double x=0;
double y=0;
double SUM_RD_Diff=0;	// SUM_X
double SUM_IR_Diff=0;	// SUM_Y
double SUM_X2=0;
double SUM_XY=0;
double NUM1=0;
double NUM2=0;
double DEN1=0;
double DEN2=0;
int R=0;

//====计算队列====//
long RD_group[MAX_LENGTH] ;					//用于显示，循环队列，存储几个周期内的信号
long IR_group[MAX_LENGTH];
long Diff_RD_group[MAX_LENGTH];
long Diff_IR_group[MAX_LENGTH];

long moving_window[WIN_LENGTH];			//循环队列，以滑动窗口的形式判断当前是否为脉搏波波谷
u8 offset_wave = 0;
u8 window_offset_wave = 0;						//队列头位置
long min;													//队列中最小值
u8 location_min;				 							//最小值位置
u8 location_min_adjust;								//最小值相对与队列头的位置，如果是32则确认一个波谷
//====各种计算标记位====//
u8 readflag = 0;					//可读标志
u8 flag_initial = 1;				//程序初始化标志位
u8 num_beat = 0;				//初始值为1，下一个脉搏后为2，计算脉搏周期，重新置1
u8 update=0;						//读取数据后，更新标志位
u8 flag_jump = 0;				//脉搏波周期判断，是否处于离开波谷的状态
u8 sample_jump = 0;			//离开波谷时的采样计数，到20则已离开波谷，置0 ，flag_jump 置1

int sample_count = 0;			//采样计数，每个周期清空，重新计数
int beats=0;						//脉率计数标记
int in=0;							//中断进入标记
int s1=0;							//脉率跳变修正标记
u8 figner=1;						//检测手指是否放入标记

//====最终计算结果====//
unsigned int heart_rate = 80;							//脉率最终测量结果，初始值为80.00
unsigned int group_heart_rate[2]={80,80};		//最近8秒内的脉率，循环队列，初始化为8000
unsigned int sample_heart_rate=80;				//脉率当前原始结果
int heartdiff=0;												//两个前后脉率差分值

unsigned int SpO2 = 9700;							//血氧饱和度最终测量结果，初始值为97.00
unsigned int group_SpO2[8]={9700,9700,9700,9700,9700,9700,9700,9700};			//最近8秒内的血氧饱和度，循环队列，初始化为9500
int32_t sum_SpO2=77600;
int offset_SpO2 = 0;										//队列头

//====显示&按键标志位====//
char one=0;
char two=0;
char three=1;
int select1=10;
int select2=-1;
int display=0;
unsigned char calculate=0;
unsigned char step1=0;
unsigned char step2=0;
unsigned char send_wave=0;
unsigned char orgin_wave=0;
u8 xin=0;
unsigned char num1=0;
unsigned char num2=0;
u8 warning=0;
u8 way=0;

//====附加功能变量====//
int light=0;
int T=0;
int RH=0;
int UV=0;

//====函数声明====//
long IR_Filter(long input);
void IR_reset(void);
long RD_Filter(long input);
void RD_reset(void);
long diff(long *group, int j);
void WARNING(void);

/* ======== main ========*/
 void main(void) {

	int i;				//for用

	WDTCTL = WDTPW + WDTHOLD;	//关闭看门狗

	// 时钟设置
	SetVCore(3);
	UCSCTL3 = SELREF_2;
	UCSCTL4 |= SELA_2;
	UCSCTL5 |= DIVS_1;
	__bis_SR_register(SCG0);
	UCSCTL0 = 0x0000;
	UCSCTL1 = DCORSEL_7;
	UCSCTL2 = FLLD_0 + 762;
	__bic_SR_register(SCG0);
	__delay_cycles(782000);
	do{
		UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + DCOFFG);
	    SFRIFG1 &= ~OFIFG;
	}while (SFRIFG1&OFIFG);

	//LED指示灯设置
	P1DIR |= BIT0;
	P1OUT &= ~BIT0;
	P4DIR |= BIT7;
	P4OUT &= ~BIT7;

	//usart 9600 时钟源是ACLK=32768Hz
	P3SEL |= BIT3 + BIT4;
	UCA0CTL1 |= UCSWRST;
	UCA0CTL1 |= UCSSEL__ACLK;
	UCA0BR0 = 0x03;
	UCA0BR1 = 0x00;
	UCA0MCTL = 0x4A;
	UCA0CTL1 &= ~UCSWRST;

	//重置滤波器
	IR_reset();
	RD_reset();
	//初始化发送队列格式
	txString[0] = (unsigned char) (0xff);
	txString[5] = (unsigned char) CR;
	txString[9] = (unsigned char) CR;
	txString[12] = (unsigned char) CR;
	//初始化附加功能外设
	IIC_Init();
	BH1750FVI_Init();
	UV_Init();
	HTU21_Init();
	//oled屏幕初始化
	Oled_Init();
	OLED_Clear();
	//展示作品名称2s
	team_name();

	//模拟前端初始化
	/*参数设置
	 * SEPGAIN ENABLE + STAGE2_LED1  ENABLE + STG2GAIN_LED1_12DB + CF_LED1_5P +  RF_LED1_100K
	 * 使用内部时钟-4MHz
	 * 采样率200Hz，LED 25% 占空比
	 */
	AFE44xx_PowerOn_Init();

	//按键外部中断
	P2DIR &= ~BIT1;
	P2REN |= BIT1;
	P2OUT |= BIT1;
	P2IES |= BIT1;
	P1DIR &= ~BIT1;
	P1REN |= BIT1;
	P1OUT |= BIT1;
	P1IES |= BIT1;
	P1IFG &= ~BIT1;
	P1IE |= BIT1;
	P2IFG &= ~BIT1;
	P2IE |= BIT1;

	//定时器设置，保证采样100HZ
	TA0CTL = TACLR + TAIE;
	TA0CTL = TASSEL__ACLK + MC__UP +  TACLR;
	//中断频率100HZ
	TA0CCTL0 = CCIE;
	TA0CCR0 = 327;

	//全局中断开启
	_EINT();

	delay_ms(2000);
	OLED_Clear();

	//开机后界面
	choose(10);
	select1=10;


 	while (1) {
 	if(calculate==1){
	// 刚开始要积累点数据，大概200数据再进行脉搏周期的判断
 		if (flag_initial == 1) {
 			if (offset_wave >= MAX_LENGTH-1) {
 				flag_initial = 0;
 			}
 		}
 		else{
 			//等待数据更新，100hz
 			if(update==1){
 				update=0;

 				// 脉搏周期判断
 				if (flag_jump == 0) {		 // flag_jump==0，表示处在寻找波谷状态
 					sample_jump = 0;		 // 离开波谷时的采样计数置0
 					min = moving_window[0]; //寻找group_caculate [64]循环队列中的最小值及其位置
 					location_min = 0;
 					for (i = 1; i<WIN_LENGTH; i++) {
 						if (min>moving_window[i]) {
 							min = moving_window[i];
 							location_min = i;
 						}
 					}
 					//计算最小值位置距离队列头距离
 					if (location_min <= window_offset_wave) {
 						location_min_adjust = window_offset_wave - location_min;
 					}
 					else {
 						location_min_adjust = window_offset_wave + WIN_LENGTH - location_min;
 					}
 					//最小值是否在队列正中
 					if (location_min_adjust == (WIN_LENGTH/2) || location_min_adjust == ((WIN_LENGTH/2)+1) ) {
 						flag_jump = 1;	//如果是，找到波谷，进入离开波谷状态
 						num_beat++;		//脉搏计数增加，如果是程序第一次找到，则由0到1，以后则总是由1到2
 						//P1OUT ^= BIT0;
 						beats++;
 						if(display==0)	delay_ms(4);
 						if(display==2&&calculate==1){
 							if(figner){
 							if((xin==1)&&(warning==0)){
 								OLED_ShowCHinese(0,6,52);
 								xin=0;
 							}
 							else if((xin==0)&&(warning==1)){
 								//OLED_Clear();
 								OLED_ShowCHinese(0,6,52);
 								xin=1;
 							}
 							else if((xin==0)&&(warning==0)){
 								OLED_ShowCHinese(0,6,53);
 								xin=1;
 							}
 							else if((xin==1)&&(warning==1)){
 								OLED_Clear();
 								WARNING();
 								delay_ms(500);
 								OLED_Clear();
 								xin=0;
 							}
 							else;
 						 }
 						}
 					}
 				}
 			}
 				// flag_jump==1，表示处在离开波谷状态
 				else {
 					sample_jump++;// 离开波谷时的采样计数
 					if (sample_jump >=35) {	// 离开波谷时的采样计数到达20
 						flag_jump = 0;//认为已离开波谷，则重新寻找下一个波谷
 				}
 			}
 		}
 		/*
 		 * 初始值为1，下一个脉搏后为2，计算平均功率和血氧饱和度，重新置1
 		 * 是否处于找到波谷的状态（num_beat由1变为2，找到，否则未找到）
 		 * 未找到则计算两路平方和，采样计数累加，
 		 * 找到则计算脉搏血氧饱和度和脉率，平方和、采样计数置0
 		 */
 		if((num_beat >= 1)) {
 			sample_count++;					//采样计数累加
 			x = ((RD_group[offset_wave]>>8) * (Diff_IR_group[offset_wave]));
 			y =((IR_group[offset_wave]>>8) * (Diff_RD_group[offset_wave]));
 			SUM_RD_Diff += (x);	// SUM_X
 			SUM_IR_Diff += (y);	// SUM_Y
 			SUM_X2 += ((x * x));
 			SUM_XY += ((x * y));
 		}
 		if ((num_beat >= 2)) {

 			NUM1 = ( (sample_count * SUM_XY)) ;
 			NUM2 =( (SUM_RD_Diff * SUM_IR_Diff));
 			DEN1  =(( sample_count * SUM_X2))  ;
 			DEN2  = ((SUM_RD_Diff * SUM_RD_Diff));
 			R = (100*(NUM1-NUM2))  /  ( DEN1-DEN2);

 			//SpO2 = 11000 - 25 * R;
 			sum_SpO2 -= group_SpO2[offset_SpO2];		//8秒内血氧饱和度之和减去8秒前的值
 			group_SpO2[offset_SpO2] = 11200 - 25 * R;	//计算当先新的脉搏血氧饱和度，拟合公式110-25×R，R为平均功率之比
 			//限幅正负300，(offset_SpO2-1)取低3位，使小于8，再加300
 			//调整新的脉搏血氧饱和度，变化不能超过3个百分点，范围在85到100之间，文献说明
 			if (group_SpO2[offset_SpO2]>(group_SpO2[(offset_SpO2 - 1) & 0x07] + 300))
 			group_SpO2[offset_SpO2] = group_SpO2[(offset_SpO2 - 1) & 0x07] + 300;
 			else if (group_SpO2[offset_SpO2]<(group_SpO2[(offset_SpO2 - 1) & 0x07] - 300))
 				group_SpO2[offset_SpO2] = group_SpO2[(offset_SpO2 - 1) & 0x07] - 300;
 			else;
 			//限幅8500---10000
 			if (group_SpO2[offset_SpO2]>10000)
 				group_SpO2[offset_SpO2] = 10000;
 			else if (group_SpO2[offset_SpO2]<8500)
 				group_SpO2[offset_SpO2] = 8500;
 			else;
 			//8秒内血氧饱和度之和加上当前的值
 			sum_SpO2 += group_SpO2[offset_SpO2];
 			offset_SpO2 = (offset_SpO2 + 1) & 0x07;
 			//计算平均值，得到最终结果
 			SpO2 = sum_SpO2 / 800;

 			//回复初始状态
 			SUM_RD_Diff = 0;	// SUM_X
 			SUM_IR_Diff = 0;	// SUM_Y
 			SUM_X2 = 0;
 			SUM_XY = 0;
 			num_beat = 1;
 			sample_count = 0;

 			if(display==2&&calculate==1){
 				calculate=1;
 				if(figner==1){
 					OLED_ShowNum(10,3,heart_rate,3,16);
 					OLED_ShowNum(74,3,R,3,16);
 				}
 				else{
 					OLED_ShowString(10, 3, "- -", 16);
 					OLED_ShowString(74, 3, "- -", 16);
 				}
				OLED_ShowString(64,0,"SPO2:",16);
				OLED_ShowString(0,0,"BPM:",16);
				if(SpO2>95){
					OLED_ShowString(64,6,"  normal",16);
					warning=0;
				}
				else{
					OLED_ShowString(64,6,"abnormal",16);
					warning=1;
				}
 	 		}
 		}
 	}
 	else{flag_initial = 1;}
 	if(display==5){

 		calculate=0;

 		Start_BH1750();
 		delay_ms(50);
 		light = Read_BH1750FVI();
 		light = light/2;
 		delay_ms(50);
 		OLED_ShowNum(24,0,light,7,16);
 		RH=READ_RH();
 		OLED_ShowNum(24,2,RH,7,16);
 		T=READ_T();
 		OLED_ShowNum(24,4,T,7,16);
 		UV=UV_Read();
 		OLED_ShowNum(24,6,UV,7,16);
 		if(display!=5){
 			choose(4);
 			select1=4;
 		}
 	}
 	}
}


#pragma vector=TIMER0_A0_VECTOR
__interrupt void TimerA0(void) {
	int i;
	int den=100;
	in++;
	s1++;
	if(readflag==1){
		if(s1==200){
			heart_rate +=heartdiff/4;
		}
		if(s1==400){
			heart_rate +=heartdiff/4;
		}
		if(s1==600){
			heart_rate +=heartdiff/4;
		}
		if(s1==800){
		heart_rate +=heartdiff/4;
		s1=0;
		}
		if(in==1000){
	 		in=0;
	 		heart_rate = group_heart_rate[1];
	 		sample_heart_rate=beats*6;
	 		beats=0;
	 		//明显错误的结果，不存储
	 		if (sample_heart_rate < 30 || sample_heart_rate>200);
	 		else {
	 			group_heart_rate[0]=group_heart_rate[1];
	 			group_heart_rate[1] = sample_heart_rate;
	 			heartdiff=group_heart_rate[1]-group_heart_rate[0];
	 			//heart_rate = group_heart_rate[0];
	 		}
		}
		readflag=0;
		RD_RAW = AFE44x0_Reg_Read(LED2_ALED2VAL );   	 // read RED - Ambient Data
		IR_RAW = AFE44x0_Reg_Read(LED1_ALED1VAL );  	 // read IR - Ambient Data
		if(way==1){
			if(IR_RAW>1200000){
					figner=0;
			}
			else{
				figner=1;
			}
		}
		else	figner=1;
		if(display==4){
			calculate=0;
			txString[1] = (unsigned char)(num2);
			txString[2] = (unsigned char)(RD_RAW& 0x000000FF);
			txString[3] = (unsigned char)((RD_RAW & 0x0000FF00) >> 8);
			txString[4] = (unsigned char)((RD_RAW & 0x00FF0000) >> 16);
			txString[6] = (unsigned char)(IR_RAW & 0x000000FF);
			txString[7] = (unsigned char)((IR_RAW & 0x0000FF00) >> 8);
			txString[8] = (unsigned char)((IR_RAW & 0x00FF0000) >> 16);
			txString[10] = (unsigned char)(0x00 & 0x000000FF);
			txString[11] = (unsigned char)(0x00 & 0x000000FF);
			for (i = 0; i <13 ; i++) {
				while (!(UCA0IFG&UCTXIFG));             	// USCI_A0 TX buffer ready?);
				UCA0TXBUF = txString[i];
			}
			orgin_wave=(orgin_wave+1)&0x7F;
			num2=(num2+1)&0xFF;
		}

		RD_Signal=RD_Filter(RD_RAW);
		IR_Signal=IR_Filter(IR_RAW);
		// 循环队列更新
		moving_window[window_offset_wave] = IR_Signal;
		IR_group[offset_wave] = IR_Signal;
		RD_group[offset_wave] =RD_Signal;
		Diff_IR_group[offset_wave] = diff( IR_group , offset_wave);
		Diff_RD_group[offset_wave] = diff( RD_group, offset_wave);
		window_offset_wave = (window_offset_wave + 1) & 0x1f ;
		offset_wave = (offset_wave + 1) & 0x7f;								//循环队列更新，用于显示，不超出128
		update=1;
		if(display==1)	{
			if(way==1){ }
			else{
				den=180;
			}
			calculate=0;
			int y1=24,y2;
			if(offset_wave==0)		OLED_Clear();
			int h=(Diff_IR_group[offset_wave])/den;
			y2=y1+h;
			if(y2>127)	y2=127;
			if(y2<0) y2=0;
			OLED_Fill((u8)offset_wave,(u8)y2,(u8)(offset_wave+1),(u8)(y2+1),1);
		}
	}

		if(display==3){
			calculate=1;
 			if(step1>2)	{
 				step1=0;
 				num1=(num1+1)&0xFF;
 				send_wave=(send_wave+1)&0x7F;
 			}
 			if(step1==0){
 				txString[1] = (unsigned char)(num1);
 				txString[2] = (unsigned char)( RD_group[send_wave] & 0x000000FF);
 				txString[3] = (unsigned char)((RD_group[send_wave] & 0x0000FF00)>>8);
 				txString[4] = (unsigned char)((RD_group[send_wave] & 0x00FF0000)>>16);

 				for (i = 0; i < 5; i++) {
 					while (!(UCA0IFG&UCTXIFG));
 					UCA0TXBUF = txString[i];
 				}
 			}
 			if(step1==1){
 				txString[6] = (unsigned char)( IR_group[send_wave] & 0x000000FF);
 				txString[7] = (unsigned char)((IR_group[send_wave] & 0x0000FF00)>>8);
 				txString[8] = (unsigned char)((IR_group[send_wave] & 0x00FF0000)>>16);
 				for (i = 5; i < 9; i++) {
 					while (!(UCA0IFG&UCTXIFG));
 					UCA0TXBUF = txString[i];
 				}
 			}
 			if(step1==2){
 				txString[10] = (unsigned char)(heart_rate);
 				txString[11] = (unsigned char)(SpO2);

 				for (i = 9; i <13 ; i++) {
 					while (!(UCA0IFG&UCTXIFG));             	// USCI_A0 TX buffer ready?);
 					UCA0TXBUF = txString[i];
 				}
 			}
 			step1++;
 		}

}


#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void) {

	switch (P1IV) {
	case P1IV_P1IFG1:
		delay_ms(10);
		if(!(P1IN&BIT1)){
			if((select1==0)){
				one=0;
				two=1;
				three=0;
				choose(8);
				select1=8;
			}
			if (select1 == 1) {
				one = 1;
				two=0;
				three=0;
				OLED_Clear();
				OLED_ShowString(24, 3, "Loading...", 16);
				display=3;
				select1=1;
			}
			if (select1 == 2) {
				one = 1;
				two=0;
				three=0;
				OLED_Clear();
				OLED_ShowString(24, 3, "Sending...", 16);
				select1=2;
				calculate=0;
				display=4;
			}
			if (select1 == 3) {
				one = 1;two=0;three=0;
				BH1750FVI_Init();
				UV_Init();
				HTU21_Init();
				OLED_Clear();
				OLED_ShowString(0, 0, "L:", 16);
				OLED_ShowString(90,0,"LUX",16);
				OLED_ShowString(0, 2, "RH:", 16);
				OLED_ShowString(90,2,"%",16);
				OLED_ShowString(0, 4, "T:", 16);
				OLED_ShowString(90,4,"C",16);
				OLED_ShowString(0, 6, "U:", 16);
				OLED_ShowString(90,6,"UVI",16);
				display=5;
			}
			if (select1 == 4) {
				one = 0;two=0;three=1;
				OLED_Clear();
				choose(13);
				select1 = 9;
			}
			if (select1 == 5) {
				one = 0; two = 1;three=0;
				OLED_Clear();
				display=1;
				calculate=1;
			}
			if (select1 == 6) {
				one = 0; two = 1;three=0;
				OLED_Clear();
				display=2;
				calculate=1;
				OLED_ShowString(64,0,"SPO2:",16);
				OLED_ShowString(0,0,"BPM:",16);
			}
			if (select1 == 7) {
				one = 1; two = 0;three=0;
				OLED_Clear();
				choose(0);
				select1=0;
			}
			if(select1==10){
				one=1;two=0;three=0;
				choose(0);
				way=1;
				select1=0;
			}
			if(select1==11){
				one=1;two=0;three=0;
				choose(0);
				way=0;
				select1=0;
			}
			if(select1==12){
				one=0;two=0;three=1;
				OLED_Clear();
				team_name();
				way=1;
				select1=9;
			}

		}
		P1IFG &= ~BIT1;
		break;
	case P1IV_NONE:	 break;
	}
}

// Port 2 interrupt service routine
#pragma vector=PORT2_VECTOR  				// DRDY interrupt
__interrupt void Port_2(void) {

	switch (P2IV) {
	case P2IV_P2IFG1:
		display=0;
		delay_ms(10);
		if (!(P2IN&BIT1)) {
			display=0;
			OLED_Clear();
			if (one==1) {
				select1++;

				if(select1>4)	select1=0;
				switch (select1) {
				case 0:choose(0); break;//本机阳码
				case 1:choose(1); break;//云端阳码
				case 2:choose(2); break;//算法阳码
				case 3:choose(3); break;//附加阳码
				case 4:choose(4); break;//取消阳码
				default:break;
				}
		    }
			if(two==1){
				select1++;
				if (select1 > 7)	select1 = 5;
				switch (select1) {
				case 5:choose(5); break;//波形阳码
				case 6:choose(6); break;//数字阳码
				case 7:choose(7); break;//取消阳码
				default:break;
				}
			}
			if(three==1){
				select1++;
				if(select1>12)	select1=10;
				switch (select1) {
				case 10:choose(10); break;//波形阳码
				case 11:choose(11); break;//数字阳码
				case 12:choose(12); break;//取消阳码
				default:break;
				}
			}
		}
		P2IFG &= ~BIT1;
		break;
	case P2IV_P2IFG3:

		readflag=1;
		P2IFG &= ~BIT3;
		break;
	case P2IV_NONE:	 break;
	}
}

#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void) {
	switch (__even_in_range(UCA0IV, 4)) {
	case 0:break;                             // Vector 0 - no interrupt
	case 2:break;								  // Vector 2 - RXIFG
	case 4:break;                             // Vector 4 - TXIFG
	default: break;
	}
}

/*
*  输入：IR 和 R 的变量
*  输出：滤波后的数值
*  设计是0-8.5HZ，采样率是200HZ
*/
long  IR_Filter(long  input) {
	long y0, x2;
	//这里是更新数据
	x2 = IR_Old.x1;
	IR_Old.x1 = IR_Old.x0;
	IR_Old.x0 = input;
	//这里是计算差分方程
	y0 =IR_Old.x0 * IR_B[0] + IR_Old.x1 * IR_B[1] + x2 * IR_B[2] - IR_Old.y1 * IR_A[1] - IR_Old.y2 * IR_A[2];
	y0 = y0 / IR_A[0];
	//这里是更新数据
	IR_Old.y2 = IR_Old.y1;
	IR_Old.y1 = y0;
	//返回计算结果
	return y0;
}

void IR_reset(void) {
	IR_Old.x0 = 0;
	IR_Old.x1 = 0;
	IR_Old.y1 = 0;
	IR_Old.y2 = 0;
}
/*
*  输入：IR 和 R 的变量
*  输出：滤波后的数值
*  设计是0-8.5HZ，采样率是200HZ
*/
long  RD_Filter(long  input) {

	long y0, x2;
	//这里是更新数据
	x2 = RD_Old.x1;
	RD_Old.x1 = RD_Old.x0;
	RD_Old.x0 = input;
	//这里是计算差分方程
	y0 =RD_Old.x0 * RD_B[0] + RD_Old.x1 * RD_B[1] + x2 * RD_B[2] - RD_Old.y1 * RD_A[1] - RD_Old.y2 * RD_A[2];
	y0 = y0 / RD_A[0];
	//这里是更新数据
	RD_Old.y2 = RD_Old.y1;
	RD_Old.y1 = y0;
	//返回计算结果
	return y0;
}

void RD_reset(void) {
	RD_Old.x0 = 0;
	RD_Old.x1 = 0;
	RD_Old.y1 = 0;
	RD_Old.y2 = 0;
}

long  diff(long  *group, int j) {

	if(j==0) return 0;//group[0];
	else{
	return (group[j] - group[j - 1]);
	}
}

void WARNING(void){
	fill_picture(0, on);
	fill_picture(1, on);
	fill_picture(2, on);
	 OLED_ShowString(4, 3, "!!  WARNING  !!", 16);
	// fill_picture(4, on);
	 fill_picture(5, on);
	 fill_picture(6, on);
	 fill_picture(7, on);
}
