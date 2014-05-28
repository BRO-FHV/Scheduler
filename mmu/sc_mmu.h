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

// Defines of Table and Sections
#define MMU_MASTER_TABLE_PAGE_COUNT 4
#define MMU_MASTER_TABLE_SIZE MEM_PAGE_SIZE * MMU_MASTER_TABLE_PAGE_COUNT
#define MMU_MAX_PROCESS_SPACE 0xC1200000
#define MMU_SECTION_ENTRY_SIZE 0x00100000
#define MMU_SECTION_ENTRY_KERNEL_INITIAL 0x00000C02
#define MMU_ALIGN_ADDRESS 0xFFFFC000

// dabt handler
#define MMU_DEBUG_EVENT 0x02
#define MMU_TRANSLATION_FAULT_SECTION 0x05
#define MMU_TRANSLATION_FAULT_PAGE 0x07

// L2 Pointer in the master table
#define MMU_VIRTUAL_TO_MASTER_TABLE_ENTRY(v) ((v) >> 20)
#define MMU_MASTER_TABLE_ENTRY_TO_L2_ADDRESS(e) ( (e) & ~0x7FF)
#define MMU_L2_TABLE_ENTRY_TO_PAGE(p) (((p) >> 12) << 12)  // the upper 20 bits of the physical address represent the page address
#define MMU_VIRTUAL_TO_L2_TABLE_ENTRY(v) ( (v >> 12) & 0xFF)
#define MMU_MASTER_TABLE_TO_L2_INITIAL_VALUE 0x00000001

// L2 Table
#define MMU_L2_PAGE_SIZE 0x100
#define MMU_L2_PAGE_COUNT 4
#define MMU_L2_TABLE_SIZE MMU_L2_PAGE_SIZE * MMU_L2_PAGE_COUNT
#define MMU_L2_INITIAL_VALUE 0x00000002


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

/**
 * \brief  Method needed for the elf loader
 *
 * \param  UserContext
 *
 * \return void
 *
 * \note
 */
void MmuCreateAddressMappingRange(tablePointer masterTable, uint32_t virtualStartAddress,uint32_t physicalStartAddress, uint32_t physicalEndAddress, uint8_t domain);

#endif /* SC_MMU_C_ */
