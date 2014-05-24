/*
 * Scheduler: scheduler.h
 * Part of BRO Project, 2014 <<https://github.com/BRO-FHV>>
 *
 * Created on: 05.03.2014
 * Description:
 *
 */
#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include <inttypes.h>
/*
 * Defines
 */
#define MAX_PROCESSES		32

/*
 * Typedefs
 */
typedef int32_t processID;
typedef uint32_t* programCounter;
typedef void* registerCache;
typedef uint32_t cpsrValue;
typedef uint32_t* tablePointer;
typedef void (*processFunc)();

/*
 * Process states
 */
typedef enum {
	FINISHED = 0, WAITING, READY, RUNNING, BLOCKED
} processState;

/*
 * Process structure
 */
typedef struct {
	processID id;
	processState state;
	processFunc func;
	programCounter pc;
	registerCache reg[15];

	/* Control Process Status Register */
	cpsrValue cpsr;

	uint32_t* masterTable;

} Process;


typedef struct ctx Context;
/*
 * methods
 */
void scheduler_runNextProcess(Context* context);
void scheduler_startProcess(processFunc func);
Process* SchedulerCurrentProcess(void);
void killThread(processID);

#endif /* SCHEDULER_H_ */
