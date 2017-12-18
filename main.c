#include <msp430.h>
#include "math.h"
#include <intrinsics.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
//====MSP430��Դ����====//
#include "HAL_UCS.h"
#include "HAL_PMM.h"
//=====ģ��ǰ������=====//
#include "AFE4400.h"
//====��ʾ����ͷ�ļ�====//
#include "oled.h"
//====���ӹ���ͷ�ļ�====//
#include "BH1750FVI.h"
#include "HTU21D.h"
#include "UV6070.h"
/************һЩ�����ĺ궨��************/

//=======���͵�У���========//
#define CR					0x0D

//====������г���====//
#define MAX_LENGTH 	128
//====���ڶ��г���====//
#define WIN_LENGTH	32
//====IIR���г���====//
#define IIR_NSEC			3
//====IIR�˲����ṹ��====//
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
//====IIR FILTER ����====//
const double IR_B[IIR_NSEC] = { 0.00782020 ,  0.01564041, 0.00782020 };
const double IR_A[IIR_NSEC] = { 1        , -1.73472576, 	0.76600660 };
const double RD_B[IIR_NSEC] = {0.00782020 ,  0.01564041, 0.00782020};
const double RD_A[IIR_NSEC] = {1        , -1.73472576, 	0.76600660 };
//========//
IROLD IR_Old;
RDOLD RD_Old;
/**********ȫ�ֱ���***********/

//====���Ͷ���====//
unsigned char txString[13];

//====����Ѫ���ź�====//
unsigned long IR_RAW=0;							// �����
unsigned long RD_RAW=0;						// ���
long IR_Signal=0;										// ����⣬�˲��������㴰��
long RD_Signal=0;										// ���  �� �˲��������㴰��

//====Ѫ���������====//
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

//====�������====//
long RD_group[MAX_LENGTH] ;					//������ʾ��ѭ�����У��洢���������ڵ��ź�
long IR_group[MAX_LENGTH];
long Diff_RD_group[MAX_LENGTH];
long Diff_IR_group[MAX_LENGTH];

long moving_window[WIN_LENGTH];			//ѭ�����У��Ի������ڵ���ʽ�жϵ�ǰ�Ƿ�Ϊ����������
u8 offset_wave = 0;
u8 window_offset_wave = 0;						//����ͷλ��
long min;													//��������Сֵ
u8 location_min;				 							//��Сֵλ��
u8 location_min_adjust;								//��Сֵ��������ͷ��λ�ã������32��ȷ��һ������
//====���ּ�����λ====//
u8 readflag = 0;					//�ɶ���־
u8 flag_initial = 1;				//�����ʼ����־λ
u8 num_beat = 0;				//��ʼֵΪ1����һ��������Ϊ2�������������ڣ�������1
u8 update=0;						//��ȡ���ݺ󣬸��±�־λ
u8 flag_jump = 0;				//�����������жϣ��Ƿ����뿪���ȵ�״̬
u8 sample_jump = 0;			//�뿪����ʱ�Ĳ�����������20�����뿪���ȣ���0 ��flag_jump ��1

int sample_count = 0;			//����������ÿ��������գ����¼���
int beats=0;						//���ʼ������
int in=0;							//�жϽ�����
int s1=0;							//���������������
u8 figner=1;						//�����ָ�Ƿ������

//====���ռ�����====//
unsigned int heart_rate = 80;							//�������ղ����������ʼֵΪ80.00
unsigned int group_heart_rate[2]={80,80};		//���8���ڵ����ʣ�ѭ�����У���ʼ��Ϊ8000
unsigned int sample_heart_rate=80;				//���ʵ�ǰԭʼ���
int heartdiff=0;												//����ǰ�����ʲ��ֵ

unsigned int SpO2 = 9700;							//Ѫ�����Ͷ����ղ����������ʼֵΪ97.00
unsigned int group_SpO2[8]={9700,9700,9700,9700,9700,9700,9700,9700};			//���8���ڵ�Ѫ�����Ͷȣ�ѭ�����У���ʼ��Ϊ9500
int32_t sum_SpO2=77600;
int offset_SpO2 = 0;										//����ͷ

//====��ʾ&������־λ====//
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

//====���ӹ��ܱ���====//
int light=0;
int T=0;
int RH=0;
int UV=0;

//====��������====//
long IR_Filter(long input);
void IR_reset(void);
long RD_Filter(long input);
void RD_reset(void);
long diff(long *group, int j);
void WARNING(void);

/* ======== main ========*/
 void main(void) {

	int i;				//for��

	WDTCTL = WDTPW + WDTHOLD;	//�رտ��Ź�

	// ʱ������
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

	//LEDָʾ������
	P1DIR |= BIT0;
	P1OUT &= ~BIT0;
	P4DIR |= BIT7;
	P4OUT &= ~BIT7;

	//usart 9600 ʱ��Դ��ACLK=32768Hz
	P3SEL |= BIT3 + BIT4;
	UCA0CTL1 |= UCSWRST;
	UCA0CTL1 |= UCSSEL__ACLK;
	UCA0BR0 = 0x03;
	UCA0BR1 = 0x00;
	UCA0MCTL = 0x4A;
	UCA0CTL1 &= ~UCSWRST;

	//�����˲���
	IR_reset();
	RD_reset();
	//��ʼ�����Ͷ��и�ʽ
	txString[0] = (unsigned char) (0xff);
	txString[5] = (unsigned char) CR;
	txString[9] = (unsigned char) CR;
	txString[12] = (unsigned char) CR;
	//��ʼ�����ӹ�������
	IIC_Init();
	BH1750FVI_Init();
	UV_Init();
	HTU21_Init();
	//oled��Ļ��ʼ��
	Oled_Init();
	OLED_Clear();
	//չʾ��Ʒ����2s
	team_name();

	//ģ��ǰ�˳�ʼ��
	/*��������
	 * SEPGAIN ENABLE + STAGE2_LED1  ENABLE + STG2GAIN_LED1_12DB + CF_LED1_5P +  RF_LED1_100K
	 * ʹ���ڲ�ʱ��-4MHz
	 * ������200Hz��LED 25% ռ�ձ�
	 */
	AFE44xx_PowerOn_Init();

	//�����ⲿ�ж�
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

	//��ʱ�����ã���֤����100HZ
	TA0CTL = TACLR + TAIE;
	TA0CTL = TASSEL__ACLK + MC__UP +  TACLR;
	//�ж�Ƶ��100HZ
	TA0CCTL0 = CCIE;
	TA0CCR0 = 327;

	//ȫ���жϿ���
	_EINT();

	delay_ms(2000);
	OLED_Clear();

	//���������
	choose(10);
	select1=10;


 	while (1) {
 	if(calculate==1){
	// �տ�ʼҪ���۵����ݣ����200�����ٽ����������ڵ��ж�
 		if (flag_initial == 1) {
 			if (offset_wave >= MAX_LENGTH-1) {
 				flag_initial = 0;
 			}
 		}
 		else{
 			//�ȴ����ݸ��£�100hz
 			if(update==1){
 				update=0;

 				// ���������ж�
 				if (flag_jump == 0) {		 // flag_jump==0����ʾ����Ѱ�Ҳ���״̬
 					sample_jump = 0;		 // �뿪����ʱ�Ĳ���������0
 					min = moving_window[0]; //Ѱ��group_caculate [64]ѭ�������е���Сֵ����λ��
 					location_min = 0;
 					for (i = 1; i<WIN_LENGTH; i++) {
 						if (min>moving_window[i]) {
 							min = moving_window[i];
 							location_min = i;
 						}
 					}
 					//������Сֵλ�þ������ͷ����
 					if (location_min <= window_offset_wave) {
 						location_min_adjust = window_offset_wave - location_min;
 					}
 					else {
 						location_min_adjust = window_offset_wave + WIN_LENGTH - location_min;
 					}
 					//��Сֵ�Ƿ��ڶ�������
 					if (location_min_adjust == (WIN_LENGTH/2) || location_min_adjust == ((WIN_LENGTH/2)+1) ) {
 						flag_jump = 1;	//����ǣ��ҵ����ȣ������뿪����״̬
 						num_beat++;		//�����������ӣ�����ǳ����һ���ҵ�������0��1���Ժ���������1��2
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
 				// flag_jump==1����ʾ�����뿪����״̬
 				else {
 					sample_jump++;// �뿪����ʱ�Ĳ�������
 					if (sample_jump >=35) {	// �뿪����ʱ�Ĳ�����������20
 						flag_jump = 0;//��Ϊ���뿪���ȣ�������Ѱ����һ������
 				}
 			}
 		}
 		/*
 		 * ��ʼֵΪ1����һ��������Ϊ2������ƽ�����ʺ�Ѫ�����Ͷȣ�������1
 		 * �Ƿ����ҵ����ȵ�״̬��num_beat��1��Ϊ2���ҵ�������δ�ҵ���
 		 * δ�ҵ��������·ƽ���ͣ����������ۼӣ�
 		 * �ҵ����������Ѫ�����ͶȺ����ʣ�ƽ���͡�����������0
 		 */
 		if((num_beat >= 1)) {
 			sample_count++;					//���������ۼ�
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
 			sum_SpO2 -= group_SpO2[offset_SpO2];		//8����Ѫ�����Ͷ�֮�ͼ�ȥ8��ǰ��ֵ
 			group_SpO2[offset_SpO2] = 11200 - 25 * R;	//���㵱���µ�����Ѫ�����Ͷȣ���Ϲ�ʽ110-25��R��RΪƽ������֮��
 			//�޷�����300��(offset_SpO2-1)ȡ��3λ��ʹС��8���ټ�300
 			//�����µ�����Ѫ�����Ͷȣ��仯���ܳ���3���ٷֵ㣬��Χ��85��100֮�䣬����˵��
 			if (group_SpO2[offset_SpO2]>(group_SpO2[(offset_SpO2 - 1) & 0x07] + 300))
 			group_SpO2[offset_SpO2] = group_SpO2[(offset_SpO2 - 1) & 0x07] + 300;
 			else if (group_SpO2[offset_SpO2]<(group_SpO2[(offset_SpO2 - 1) & 0x07] - 300))
 				group_SpO2[offset_SpO2] = group_SpO2[(offset_SpO2 - 1) & 0x07] - 300;
 			else;
 			//�޷�8500---10000
 			if (group_SpO2[offset_SpO2]>10000)
 				group_SpO2[offset_SpO2] = 10000;
 			else if (group_SpO2[offset_SpO2]<8500)
 				group_SpO2[offset_SpO2] = 8500;
 			else;
 			//8����Ѫ�����Ͷ�֮�ͼ��ϵ�ǰ��ֵ
 			sum_SpO2 += group_SpO2[offset_SpO2];
 			offset_SpO2 = (offset_SpO2 + 1) & 0x07;
 			//����ƽ��ֵ���õ����ս��
 			SpO2 = sum_SpO2 / 800;

 			//�ظ���ʼ״̬
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
	 		//���Դ���Ľ�������洢
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
		// ѭ�����и���
		moving_window[window_offset_wave] = IR_Signal;
		IR_group[offset_wave] = IR_Signal;
		RD_group[offset_wave] =RD_Signal;
		Diff_IR_group[offset_wave] = diff( IR_group , offset_wave);
		Diff_RD_group[offset_wave] = diff( RD_group, offset_wave);
		window_offset_wave = (window_offset_wave + 1) & 0x1f ;
		offset_wave = (offset_wave + 1) & 0x7f;								//ѭ�����и��£�������ʾ��������128
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
				case 0:choose(0); break;//��������
				case 1:choose(1); break;//�ƶ�����
				case 2:choose(2); break;//�㷨����
				case 3:choose(3); break;//��������
				case 4:choose(4); break;//ȡ������
				default:break;
				}
		    }
			if(two==1){
				select1++;
				if (select1 > 7)	select1 = 5;
				switch (select1) {
				case 5:choose(5); break;//��������
				case 6:choose(6); break;//��������
				case 7:choose(7); break;//ȡ������
				default:break;
				}
			}
			if(three==1){
				select1++;
				if(select1>12)	select1=10;
				switch (select1) {
				case 10:choose(10); break;//��������
				case 11:choose(11); break;//��������
				case 12:choose(12); break;//ȡ������
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
*  ���룺IR �� R �ı���
*  ������˲������ֵ
*  �����0-8.5HZ����������200HZ
*/
long  IR_Filter(long  input) {
	long y0, x2;
	//�����Ǹ�������
	x2 = IR_Old.x1;
	IR_Old.x1 = IR_Old.x0;
	IR_Old.x0 = input;
	//�����Ǽ����ַ���
	y0 =IR_Old.x0 * IR_B[0] + IR_Old.x1 * IR_B[1] + x2 * IR_B[2] - IR_Old.y1 * IR_A[1] - IR_Old.y2 * IR_A[2];
	y0 = y0 / IR_A[0];
	//�����Ǹ�������
	IR_Old.y2 = IR_Old.y1;
	IR_Old.y1 = y0;
	//���ؼ�����
	return y0;
}

void IR_reset(void) {
	IR_Old.x0 = 0;
	IR_Old.x1 = 0;
	IR_Old.y1 = 0;
	IR_Old.y2 = 0;
}
/*
*  ���룺IR �� R �ı���
*  ������˲������ֵ
*  �����0-8.5HZ����������200HZ
*/
long  RD_Filter(long  input) {

	long y0, x2;
	//�����Ǹ�������
	x2 = RD_Old.x1;
	RD_Old.x1 = RD_Old.x0;
	RD_Old.x0 = input;
	//�����Ǽ����ַ���
	y0 =RD_Old.x0 * RD_B[0] + RD_Old.x1 * RD_B[1] + x2 * RD_B[2] - RD_Old.y1 * RD_A[1] - RD_Old.y2 * RD_A[2];
	y0 = y0 / RD_A[0];
	//�����Ǹ�������
	RD_Old.y2 = RD_Old.y1;
	RD_Old.y1 = y0;
	//���ؼ�����
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
