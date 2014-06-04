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

	switch (data->swiNumber) {

	case 0:
		SwiStdioPrintf((const char*)data->arg1);
		break;

	default:
		break;

	}

}

//#pragma INTERRUPT(swi_handler, SWI)
//interrupt void swi_handler(SyscallArgData* data) {

void SwiHandler(SyscallArgData* data,Context* context) {

	    printf("SwiHandler Swi Forward\n");
	    SwiForward(data);

	    Process* proc =  SchedulerCurrentProcess();

	    if(proc != NULL && proc->state==RUNNING)
	    {
	    	printf("SwiHandler: MMU Switch Process\n");
	    	MmuSwitchToProcess(proc);
	    }

}
