/*
 * Scheduler: sc_mmu.h
 * Part of BRO Project, 2014 <<https://github.com/BRO-FHV>>
 *
 * Created on: 24.05.2014
 * Description:
 * TODO
 */

#ifndef SC_MMU_C_
#define SC_MMU_C_

#include "../scheduler.h"

void MmuInit();

tablePointer MmuCreateMasterTable();

void MmuInitProcess(Process* process);

void MmuSwitchToProcess(Process* process);

void MmuHandleDabt(Context* context);

#endif /* SC_MMU_C_ */
