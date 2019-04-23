/*
 * communication.h
 *
 *  Created on: 17 sty 2019
 *      Author: Dunajski
 */

#ifndef COMMUNICATION_H_
#define COMMUNICATION_H_

#include <avr/interrupt.h>

extern unsigned char * p_dada;

void PutUInt8ToSerial(uint8_t integer);
void PutUint16ToSerial(uint16_t  two_bytes);
void PutToSerial(unsigned char data);
unsigned char GetFromSerial(unsigned char *p_dada);
void StrToSerial(char *msg);

#endif /* COMMUNICATION_H_ */
