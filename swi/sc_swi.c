/*
 * Driver: sc_swi.c
 * Part of BRO Project, 2014 <<https://github.com/BRO-FHV>>
 *
 * Created on: 04.06.2014
 * Description:
 * TODO
 */
#include <Syscall/lib_syscall.h>
#include <inttypes.h>
#include "sc_swi.h"
#include "sc_swi_forward.h"
#include "../scheduler.h"
#include "../mmu/sc_mmu.h"
#include <basic.h>
#include <stdio.h>
#include <stdlib.h>

void SwiForward(SyscallArgData* data) {

	uint32_t result = -1;

	switch (data->swiNumber) {

	case SYSCALL_PRINTF:
		SwiStdioPrintf((const char*) data->arg1);
		break;
	case SYSCALL_LED_ON_0:
		SwiLed0On();
		break;
	case SYSCALL_LED_ON_1:
		SwiLed1On();
		break;
	case SYSCALL_LED_ON_2:
		SwiLed2On();
		break;
	case SYSCALL_LED_ON_3:
		SwiLed3On();
		break;
	case SYSCALL_LED_OFF_0:
		SwiLed0Off();
		break;
	case SYSCALL_LED_OFF_1:
		SwiLed1Off();
		break;
	case SYSCALL_LED_OFF_2:
		SwiLed2Off();
		break;
	case SYSCALL_LED_OFF_3:
		SwiLed3Off();
		break;
	case SYSCALL_GPIO_DIR_SET:
		SwiGPIODirModeSet((unsigned int) data->arg1, (unsigned int) data->arg2,
				(unsigned int) data->arg3);
		break;
	case SYSCALL_GPIO_PIN_WRITE:
		SwiGPIOPinWrite((unsigned int) data->arg1, (unsigned int) data->arg2,
				(unsigned int) data->arg3);
		break;
	case SYSCALL_GPIO_PIN_READ:
		result = SwiGPIOPinRead((unsigned int) data->arg1,
				(unsigned int) data->arg2);
		data->result = result;
		break;
	case SYSCALL_LED_INIT_LED_0:
		SwiLedInit0();
		break;
	case SYSCALL_LED_INIT_LED_1:
		SwiLedInit3();
		break;
	case SYSCALL_LED_INIT_LED_2:
		SwiLedInit2();
		break;
	case SYSCALL_LED_INIT_LED_3:
		SwiLedInit1();
		break;
	case SYSCALL_LED_INIT_REGISTER:
		SwiLedInitRegister();
		break;
		case SYSCALL_LEDS_INIT:
		SwiLedsInit();
		break;

	default:
		break;

	}

}

//#pragma INTERRUPT(swi_handler, SWI)
//interrupt void swi_handler(SyscallArgData* data) {

void SwiHandler(SyscallArgData* data, Context* context) {

	printf("SwiHandler Swi Forward\n");
	SwiForward(data);

	Process* proc = SchedulerCurrentProcess();

	if (proc != NULL && proc->state == RUNNING) {
		printf("SwiHandler: MMU Switch Process\n");
		MmuSwitchToProcess(proc);
	}

}
