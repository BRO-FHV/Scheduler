/*
 * Driver: sc_swi_forward.h
 * Part of BRO Project, 2014 <<https://github.com/BRO-FHV>>
 *
 * Created on: 04.06.2014
 * Description:
 * TODO
 */

#include <stdio.h>
#include <stdlib.h>

#ifndef DR_SWI_FORWARD_H_
#define DR_SWI_FORWARD_H_

void SwiStdioPrintf(const char* text);

void SwiLed0On(void);
void SwiLed1On(void);
void SwiLed2On(void);
void SwiLed3On(void);

void SwiLed0Off(void);
void SwiLed1Off(void);
void SwiLed2Off(void);
void SwiLed3Off(void);

void SwiLedInit0(void);
void SwiLedInit1(void);
void SwiLedInit2(void);
void SwiLedInit3(void);

void SwiLedInitRegister(void);
void SwiLedsInit(void);

void SwiGPIODirModeSet(unsigned int baseAdd,unsigned int pinNumber,unsigned int pinDirection);
void SwiGPIOPinWrite(unsigned int baseAdd, unsigned int pinNumber,unsigned int pinValue);
unsigned int SwiGPIOPinRead(unsigned int baseAdd, unsigned int pinNumber);

#endif /* DR_SWI_FORWARD_H_ */
