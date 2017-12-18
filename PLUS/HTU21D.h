/*
 * HTU21D.h
 *
 *  Created on: 2016年8月24日
 *      Author: PP
 */

#ifndef PLUS_HTU21D_H_
#define PLUS_HTU21D_H_

/*
 * HTU21D.c
 *
 *  Created on: 2016年8月24日
 *      Author: PP
 */
unsigned char IIC_Send(unsigned char Data);
void HTU21_Init(void);
int READ_T(void) ;
int READ_RH(void) ;
#endif /* PLUS_HTU21D_H_ */
