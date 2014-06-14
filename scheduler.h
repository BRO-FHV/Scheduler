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
#define REG_COUNT			15

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
	registerCache reg[REG_COUNT];

	/* Control Process Status Register */
	cpsrValue cpsr;

	uint32_t* masterTable;

} Process;


typedef struct ctx Context;
/*
 * methods
 */
void SchedulerRunNextProcess(Context* context);
void SchedulerStartProcess(processFunc func);
Process* SchedulerCurrentProcess(void);
void KillProcess(processID);
void loadProcessFromElf(uint32_t length, uint8_t* data);
#endif /* SCHEDULER_H_ */
