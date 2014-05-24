/*
 * Driver: sc_mmu.h
 * Part of BRO Project, 2014 <<https://github.com/BRO-FHV>>
 *
 * Created on: May 07, 2014
 * Description:
 * Virtual Memory
 */

#ifndef MMU_H_
#define MMU_H_

#include "../scheduler.h"
#include <basic.h>

extern volatile uint32_t mmu_current_master_table;
//extern volatile uint32_t last_interrupt_source;

//
// General
//
#define MMU_ALIGN_ADDRESS 0xFFFFC000

//
// Master Table (4 pages)
//
#define MMU_MASTER_TABLE_PAGE_SIZE 0x1000
#define MMU_MASTER_TABLE_PAGE_COUNT 4
#define MMU_MASTER_TABLE_SIZE MMU_MASTER_TABLE_PAGE_SIZE * MMU_MASTER_TABLE_PAGE_COUNT

// the upper 12 bits of a virtual address are the index of the
// L2 Pointer in the master table
#define MMU_VIRTUAL_TO_MASTER_TABLE_ENTRY(v) ((v) >> 20)
#define MMU_MASTER_TABLE_ENTRY_TO_L2_ADDRESS(e) ( (e) & ~0x7FF)

#define MMU_MAX_PROCESS_SPACE   0xFFF00000
#define MMU_SECTION_ENTRY_SIZE  0x00100000
#define MMU_SECTION_ENTRY_KERNEL_INITIAL   0x00000C02
#define MMU_MASTER_TABLE_TO_L2_INITIAL_VALUE 0x00000001


//
// L2 Table
//
#define MMU_L2_PAGE_SIZE 0x100
#define MMU_L2_PAGE_COUNT 4
#define MMU_L2_TABLE_SIZE MMU_L2_PAGE_SIZE * MMU_L2_PAGE_COUNT


// the upper 20 bits of the physical address represent the page address
#define MMU_L2_TABLE_ENTRY_TO_PAGE(p) (((p) >> 12) << 12)
#define MMU_VIRTUAL_TO_L2_TABLE_ENTRY(v) ( (v >> 12) & 0xFF)


#define MMU_L2_INITIAL_VALUE 0x00000002

//
// Handler
//

#define MMU_DEBUG_EVENT 0x02
#define MMU_TRANSLATION_FAULT_SECTION 0x05
#define MMU_TRANSLATION_FAULT_PAGE 0x07

//
// Functions
//
void mmu_init(void);
void mmu_set_kernel_table(tablePointer table);
void mmu_set_process_table(tablePointer table);
void mmu_switch_to_kernel();
void mmu_switch_to_process(Process* process);
void mmu_init_process(Process* process);


Boolean mmu_handle_data_abort();
Boolean mmu_handle_prefetch_abort();

tablePointer mmu_create_master_table(void);
void mmu_create_page_mapping(tablePointer masterTable, uint32_t virtualAddress, uint8_t domain);
void mmu_create_direct_mapping_range(tablePointer masterTable,
        uint32_t physicalStartAddress, uint32_t physicalEndAddress,
        uint8_t domain);
void mmu_create_address_mapping(tablePointer masterTable, uint32_t virtualAddress, uint32_t physicalAddress, uint8_t domain);
void mmu_create_address_mapping_range(tablePointer masterTable, uint32_t virtualStartAddress,
        uint32_t physicalStartAddress, uint32_t physicalEndAddress, uint8_t domain);
tablePointer mmu_get_or_create_l2_table(tablePointer masterTable, uint32_t masterTableEntry, uint8_t domain);

Boolean mmu_is_legal(uint32_t accessedAddress, uint32_t faultState);
Boolean mmu_is_process_page(uint32_t address);
void mmu_delete_process_memory(Process* proc);

uint32_t mmu_virtual_to_physical(tablePointer masterTable, uint32_t virtualAddress);

#endif /* MMU_H_ */
