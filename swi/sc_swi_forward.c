/*
 * Driver: sc_sswi_forward.c
 * Part of BRO Project, 2014 <<https://github.com/BRO-FHV>>
 *
 * Created on: 04.06.2014
 * Description:
 * TODO
 */

#include<inttypes.h>
#include "sc_swi_forward.h"
#include <stdio.h>
#include <stdlib.h>
#include <led/dr_led.h>
#include <gpio/dr_gpio.h>
#include <eth/dr_eth_udp.h>

void SwiStdioPrintf(const char* text) {
	printf(text);
}

void SwiLed0On(void) {

	int i = GPIOPinRead(SOC_GPIO_1_REGS, LED0_PIN);
	if (i == 0) {
		LedOn0();
	}
}

void SwiLed1On(void) {

	int i = GPIOPinRead(SOC_GPIO_1_REGS, LED1_PIN);
	if (i == 0) {
		LedOn1();
	}
}

void SwiLed2On(void) {

	int i = GPIOPinRead(SOC_GPIO_1_REGS, LED2_PIN);
	if (i == 0) {
		LedOn2();
	}
}

void SwiLed3On(void) {

	int i = GPIOPinRead(SOC_GPIO_1_REGS, LED3_PIN);
	if (i == 0) {
		LedOn3();
	}
}

void SwiLed0Off(void) {

	int i = GPIOPinRead(SOC_GPIO_1_REGS, LED0_PIN);
	if (i != 0) {
		LedOff0();
	}
}

void SwiLed1Off(void) {

	int i = GPIOPinRead(SOC_GPIO_1_REGS, LED1_PIN);
	if (i != 0) {
		LedOff1();
	}
}

void SwiLed2Off(void) {

	int i = GPIOPinRead(SOC_GPIO_1_REGS, LED2_PIN);
	if (i != 0) {
		LedOff2();
	}
}

void SwiLed3Off(void) {

	int i = GPIOPinRead(SOC_GPIO_1_REGS, LED3_PIN);
	if (i != 0) {
		LedOff3();
	}
}

void SwiGPIODirModeSet(unsigned int baseAdd, unsigned int pinNumber,
		unsigned int pinDirection) {

	GPIODirModeSet(baseAdd, pinNumber, pinDirection);
}

void SwiGPIOPinWrite(unsigned int baseAdd, unsigned int pinNumber,
		unsigned int pinValue) {

	GPIOPinWrite(baseAdd, pinNumber, pinValue);
}
unsigned int SwiGPIOPinRead(unsigned int baseAdd, unsigned int pinNumber) {
	return GPIOPinRead(baseAdd, pinNumber);
}

void SwiLedsInit(void) {
	LedInitRegister();
	LedInit0();
	LedInit1();
	LedInit2();
	LedInit3();
}

void SwiLedInitRegister(void) {
	LedInitRegister();
}

void SwiLedInit0(void) {

	LedInit0();
}
void SwiLedInit1(void) {

	LedInit1();
}

void SwiLedInit2(void) {

	LedInit2();
}

void SwiLedInit3(void) {

	LedInit3();
}

void SwiMmuInit(void){

	MmuInit();
}



void SwiUdpInit(uint32_t port)
{
  BroUdpInit(port);
}
upd_package_t* SwiUdpGetData(uint32_t port)
{
   return BroUdpGetData(port);
}
void SwiUdpSendData(uint8_t receiver[],uint32_t port,uint8_t* data, uint32_t datalen){

	BroUdpSendData(receiver,port,data,datalen);
}
tBoolean SwiUdpHasData(uint32_t port){

	return BroUdpHasData(port);

}

