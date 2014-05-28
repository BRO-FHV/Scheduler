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


/**
 * \brief  Used to initialize the MMU
 *
 * \param  None
 *
 * \return None
 *
 * \note  Used to initialize the MMU
 */
void MmuInit();

/**
 * \brief  Used to create the Master Table
 *
 * \param  None
 *
 * \return Pointer to the created Master Table
 *
 * \note  Used to initialize the MMU
 */
tablePointer MmuCreateMasterTable();

/**
 * \brief  Used to initialize the MMU for a process
 *
 *
 * \param  Process
 *
 * \return void
 *
 * \note  Used to initialize the MMU for a process
 */
void MmuInitProcess(Process* process);

/**
 * \brief  Used to switch Process in the mmu
 *
 *
 * \param  Process to switch
 *
 * \return void
 *
 * \note  Used to initialize the MMU for a process
 */
void MmuSwitchToProcess(Process* process);

/**
 * \brief  Method to handle the dapt interrupt to switch the solve the memory access process
 *
 * \param  UserContext
 *
 * \return void
 *
 * \note  Method to handle the dapt interrupt to switch the solve the memory access process
 */
void MmuHandleDabt(Context* context);

#endif /* SC_MMU_C_ */
