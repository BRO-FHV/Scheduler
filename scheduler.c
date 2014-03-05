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
#include <stdio.h>
#include <setjmp.h>
#include <stdlib.h>
#include "scheduler.h"

/*
 * Defines
 */
#define THREAD_SIZE		32
#define INVALID_ID		-1

/*
 * Process states
 */
typedef enum {
	FINISHED = 0,
	WAITING,
	READY,
	RUNNING,
	BLOCKED
} processState;

/*
 * Process structure
 */
typedef struct {
	processID id;
	processState state;
	processFunc func;
	jmp_buf context;
} process;

/*
 * Methods
 */
processID 	getNextProcess();
processID 	getNextProcessID();
void 		killThread(processID);
void 		atomicStart();
void 		atomicEnd();

/*
 * Constants
 */
processID 	gRunningThread = INVALID_ID;
process 	gThreads[MAX_PROCESSES];

processID scheduler_startProcess(processFunc func) {
	atomicStart();

	processID newthreadID = getNextProcessID();

	if(INVALID_ID == newthreadID) {
		atomicEnd();
		//not possible to add further threads
		return INVALID_ID;
	}

	gThreads[newthreadID].id = newthreadID;
	gThreads[newthreadID].state = READY;
	gThreads[newthreadID].func = func;

	if(0 == setjmp(gThreads[newthreadID].context)) {
		atomicEnd();
		return newthreadID;
	} else {
		//coming from long jump
		//call thread functionallity, kill thread if finished..
		//TODO
		//__set_SP_register(0x3FF - (gRunningThread * THREAD_SIZE));

		atomicEnd();

		gThreads[gRunningThread].func();

		//killThread(gRunningThread);
		return INVALID_ID;
	}
}

void scheduler_runNextProcess() {
	atomicStart();

	processID nextthreadID = getNextProcess();

	if(INVALID_ID != nextthreadID) {
		if(RUNNING == gThreads[gRunningThread].state) {
			gThreads[gRunningThread].state = READY;
		}

		gRunningThread = nextthreadID;
		gThreads[gRunningThread].state = RUNNING;

		longjmp(gThreads[gRunningThread].context, 1);
	}

	atomicEnd();
}

void killThread(processID threadID) {
	gThreads[threadID].state = FINISHED;
	gThreads[threadID].func = NULL;
}

processID getNextProcess() {
	processID nextID = gRunningThread + 1;

	while(nextID != gRunningThread) {
		if(READY == gThreads[nextID].state) {
			return nextID;
		}

		nextID = ++nextID <= MAX_PROCESSES ? nextID : 0;
	}

	return INVALID_ID;
}

processID getNextProcessID() {
	int i;
	for(i = 0; i < MAX_PROCESSES; i++) {
		if(FINISHED == gThreads[i].state) {
			return i;
		}
	}

	return INVALID_ID;
}

void atomicStart() {
	//TODO
}

void atomicEnd() {
	//TODO
}
