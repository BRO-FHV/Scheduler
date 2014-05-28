/*
 * Scheduler: scheduler.c
 * Part of BRO Project, 2014 <<https://github.com/BRO-FHV>>
 *
 * Created on: 05.03.2014
 * Description: 
 * The scheduler is responsible for switching processes.
 * It uses the TODO strategy for deciding which process is next.
 *
 */

#include <string.h>
#include <stdio.h>
#include <setjmp.h>
#include <stdlib.h>
#include <cpu/hw_cpu.h>
#include "scheduler.h"
#include "context.h"
#include "mmu/sc_mmu.h"
#include "mem/sc_mem.h"

/*
 * Defines
 */
#define THREAD_SIZE		32
#define INVALID_ID		-1

typedef struct ctx {
	/* Control Process Status Register */
	cpsrValue cpsr;
	programCounter pc;
	registerCache reg[15];

} Context;

/*
 * Methods
 */
processID getNextProcess();
processID getNextProcessID();
void atomicStart();
void atomicEnd();

/*
 * Constants
 */
processID gRunningThread = INVALID_ID;
Process gThreads[MAX_PROCESSES];

void SchedulerStartProcess(processFunc func) {
	atomicStart();

	processID newthreadID = getNextProcessID();

	if (INVALID_ID == newthreadID) {
		atomicEnd();
	} else {
		gThreads[newthreadID].id = newthreadID;
		gThreads[newthreadID].state = READY;
		gThreads[newthreadID].func = func;

		gThreads[newthreadID].pc = (programCounter) func;
		gThreads[newthreadID].pc = gThreads[newthreadID].pc + 1;
		gThreads[newthreadID].cpsr = 0x00000110;

		gThreads[newthreadID].reg[13] = (void*) (PROCESS_STACK_START
				+ PROCESS_STACK_SIZE);
		gThreads[newthreadID].masterTable = (tablePointer) MmuCreateMasterTable();

		MmuInitProcess(&gThreads[newthreadID]);
	}
}

void SchedulerRunNextProcess(Context* context) {
	atomicStart();

	processID nextthreadID = getNextProcess();

	if (INVALID_ID != nextthreadID) {
		if (RUNNING == gThreads[gRunningThread].state) {
			gThreads[gRunningThread].state = READY;
			gThreads[gRunningThread].pc = context->pc;
			gThreads[gRunningThread].cpsr = context->cpsr;
			memcpy(gThreads[gRunningThread].reg, context->reg,
					sizeof(gThreads[gRunningThread].reg));

		}

		gRunningThread = nextthreadID;
		gThreads[gRunningThread].state = RUNNING;
		context->pc = gThreads[gRunningThread].pc;
		context->cpsr = gThreads[gRunningThread].cpsr;

		memcpy(context->reg, gThreads[gRunningThread].reg,
				sizeof(gThreads[gRunningThread].reg));

		MmuSwitchToProcess(&gThreads[gRunningThread]);

		uint32_t* x = malloc(sizeof(uint32_t));
		*x = 0x00;
	}

	atomicEnd();
}

void KillProcess(processID threadID) {
	gThreads[threadID].state = FINISHED;
	gThreads[threadID].func = NULL;
}

processID getNextProcess() {
	processID nextID = gRunningThread + 1;
	processID check = gRunningThread;

	if (gRunningThread == INVALID_ID) {
		check = 0;
	}
	do {
		if (READY == gThreads[nextID].state) {
			return nextID;
		}

		nextID = ++nextID <= MAX_PROCESSES ? nextID : 0;
	} while (nextID != check);

	return INVALID_ID;
}

processID getNextProcessID() {
	int i;
	for (i = 0; i < MAX_PROCESSES; i++) {
		if (FINISHED == gThreads[i].state) {
			return i;
		}
	}

	return INVALID_ID;
}

void atomicStart() {
	//CPUirqd();
}

void atomicEnd() {
	//CPUirqe();
}

Process* SchedulerCurrentProcess(void) {
	return &gThreads[gRunningThread];
}
