/*
 * BH1750FVI.h
 *
 *  Created on: 2016Äê8ÔÂ23ÈÕ
 *      Author: PP
 */

#ifndef PLUS_BH1750FVI_H_
#define PLUS_BH1750FVI_H_

void BH1750FVI_Init(void);
void Start_BH1750(void);
int Read_BH1750FVI(void);
char BH_Write(unsigned char REG_Address);


#endif /* PLUS_BH1750FVI_H_ */
