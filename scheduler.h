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
#define MAX_PROCESSES		10

/*
 * Typedefs
 */
typedef int32_t processID;
typedef uint32_t* programCounter;
typedef uint32_t registerCache;
typedef uint32_t cpsrValue;
typedef void (*processFunc)();


typedef struct ctx Context;
/*
 * methods
 */
void scheduler_runNextProcess(Context* context);
void scheduler_startProcess(processFunc func);

#endif /* SCHEDULER_H_ */
