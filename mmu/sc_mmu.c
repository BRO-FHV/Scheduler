/*
 * Scheduler: sc_mmu.c
 * Part of BRO Project, 2014 <<https://github.com/BRO-FHV>>
 *
 * Created on: 24.05.2014
 * Description:
 * TODO
 */

#include <inttypes.h>
#include <string.h>
#include <basic.h>
#include <stdio.h>
#include "mmu_internals.h"
#include "../context.h"
#include "../scheduler.h"
#include "../mem/sc_mem.h"

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

// variable definitions
tablePointer kernelMasterTable;

volatile uint32_t mmuCurrentMasterTable;
volatile uint32_t mmuAccessedAddress;
volatile uint32_t mmuFaultState;

// internal function definitions
tablePointer _mmuCreateMasterTable();
void _mmuSetKernelTable(tablePointer table);
void _mmuSetProcessTable(tablePointer table);
Boolean _mmuHandleDataAbort();
void _mmuSwitchTokernel();
void _mmuSwitchToProcess(Process* process);
tablePointer _mmuGetOrCreateL2Table(tablePointer masterTable,
		uint32_t masterTableEntry, uint8_t domain);
void _mmuCreateAddressMapping(tablePointer masterTable, uint32_t virtualAddress,
		uint32_t physicalAddress, uint8_t domain);
Boolean _mmuIsLegal(uint32_t accessedAddress, uint32_t faultState);
void _mmuCreatePageMapping(tablePointer masterTable, uint32_t virtualAddress,
		uint8_t domain);

void MmuInit() {
	// init regions
	MemInit();

	// disable mmu
	__mmu_disable();

	// reserve direct mapped regions
	uint32_t i;
	for (i = 0; i < MEM_REGION_COUNT; i++) {
		Region* region = MemGet(i);
		// reserve all pages if region is direct mapping
		if (region->direct == TRUE) {
			MemReservePages(i, 0, region->totalPageCount);
		}
	}

	// create master page table
	kernelMasterTable = _mmuCreateMasterTable();

	// initialize the kernel master table so the lookup works for the kernel
	tablePointer currentTableEntry = kernelMasterTable;
	for (i = 0x00000000; i < MMU_MAX_PROCESS_SPACE; i += MMU_SECTION_ENTRY_SIZE)
	{
		*currentTableEntry = i | MMU_SECTION_ENTRY_KERNEL_INITIAL;
		currentTableEntry++;
	}
	*currentTableEntry = 0xFFF00000 | MMU_SECTION_ENTRY_KERNEL_INITIAL; // don't miss the last one

	// activate mmu
	_mmuSetKernelTable(kernelMasterTable);
	_mmuSetProcessTable(kernelMasterTable);

	// full access
	__mmu_set_domain_access(0xFFFFFFFF);
	__mmu_enable();
}

tablePointer _mmuCreateMasterTable() {
	// find some free pages in our memory to place the table
	tablePointer tableAddress = (tablePointer) MemFindFree(
			MMU_MASTER_TABLE_PAGE_COUNT, TRUE, TRUE);

	memset(tableAddress, 0, MMU_MASTER_TABLE_SIZE);
	return tableAddress;
}

void _mmuSetKernelTable(tablePointer table) {
	uint32_t tableAddress = (uint32_t) table & MMU_ALIGN_ADDRESS;
	mmuCurrentMasterTable = tableAddress;
	__mmu_flush_tlb();
	__mmu_set_kernel_table(tableAddress);
	__mmu_set_process_table(tableAddress);

}
void _mmuSetProcessTable(tablePointer table) {
	uint32_t tableAddress = (uint32_t) table & MMU_ALIGN_ADDRESS;
	mmuCurrentMasterTable = tableAddress;
	__mmu_flush_tlb();
	__mmu_set_process_table(tableAddress);
}

void MMUHandleDabt(Context* context) {
	if (!_mmuHandleDataAbort()) {
		scheduler_runNextProcess(context);
	}

	Process* process = SchedulerCurrentProcess();
	if (process != NULL) {
		_mmuSwitchToProcess(process);
	}
}

Boolean _mmuHandleDataAbort() {
	mmuAccessedAddress = 0;
	mmuFaultState = 0;
	__mmu_load_dabt_data();

	_mmuSwitchTokernel();
	printf("data abort!\n");

	Process* currentProcess = SchedulerCurrentProcess();

	if (currentProcess == NULL) {
		printf("No process running, we keep going");
		return FALSE;
	} else {
		if (_mmuIsLegal(mmuAccessedAddress, mmuFaultState)) {
			printf("create mapped page\n");
			_mmuCreatePageMapping(currentProcess->masterTable,
					mmuAccessedAddress, 0);
			return TRUE;
		} else {
			// TODO last interrupt source (0x4711) set in datbt_handler (R14)
			printf(
					"illegal accessed address (0x%x) at position (0x%x) bye bye pid %i\n",
					mmuAccessedAddress, 0x4711, currentProcess->id);
			killThread(currentProcess->id);
			return FALSE;
		}
	}
}

void _mmuSwitchTokernel() {
	_mmuSetKernelTable(kernelMasterTable);
}

void _mmuSwitchToProcess(Process* process) {
	// if we alread have a master table, we use it
	if (process->masterTable == NULL) {
		printf("FATAL: no master table for process\n");
		return;
	}

	_mmuSetProcessTable(process->masterTable);
}

Boolean _mmuIsLegal(uint32_t accessedAddress, uint32_t faultState) {
	uint32_t status;
	Boolean writeAccess;
	Boolean sdBit;
	Boolean sBit;

	status = faultState & 0xF;
	sBit = (faultState >> 10) & 0x01;
	writeAccess = (faultState >> 11) & 0x01;
	sdBit = (faultState >> 12) & 0x01;

	status |= (sdBit << 6);
	status |= (sBit << 5);

	if (writeAccess) {
		switch (status) {
		case MMU_TRANSLATION_FAULT_SECTION:
		case MMU_TRANSLATION_FAULT_PAGE:
			// access to stack or heap?
			return (accessedAddress >= PROCESS_STACK_START
					&& accessedAddress
							< (PROCESS_STACK_START + PROCESS_STACK_SIZE))
					|| (accessedAddress >= PROCESS_SYSMEM_START
							&& accessedAddress
									< (PROCESS_SYSMEM_START
											+ PROCESS_SYSMEM_SIZE));
		case MMU_DEBUG_EVENT:
			return TRUE;
		default:
			return FALSE;

		}
	}

	return FALSE;
}

void _mmuCreatePageMapping(tablePointer masterTable, uint32_t virtualAddress,
		uint8_t domain) {
	// we search for a new page
	void* physicalAddress = MemFindFree(1, TRUE, TRUE);

	// reset the page
	memset(physicalAddress, 0, MEM_PAGE_SIZE);

	// and map it to the given master table
	_mmuCreateAddressMapping(masterTable, virtualAddress,
			(uint32_t) physicalAddress, domain);
}

void _mmuCreateAddressMapping(tablePointer masterTable, uint32_t virtualAddress,
		uint32_t physicalAddress, uint8_t domain) {
	// For an actual mapping we need to get the L2 table
	tablePointer l2Table;
	uint32_t l2EntryNumber;
	uint32_t l2EntryValue;

	l2Table = _mmuGetOrCreateL2Table(masterTable,
			MMU_VIRTUAL_TO_MASTER_TABLE_ENTRY(virtualAddress), domain);

	if (l2Table != NULL) {
		// now we build the l2 table entry
		l2EntryValue = MMU_L2_TABLE_ENTRY_TO_PAGE(physicalAddress)
				| MMU_L2_INITIAL_VALUE;

		l2EntryValue |= (0x03 << 4); // Set all Access Bits
		l2EntryValue |= (0x01 << 9);

		// and write it to the l2 table
		l2EntryNumber = MMU_VIRTUAL_TO_L2_TABLE_ENTRY(virtualAddress);
		*(l2Table + l2EntryNumber) = l2EntryValue;
	} else {
		printf("FATAL: we ran out of memory, L2 Table could not be created\n");
	}
}

tablePointer _mmuGetOrCreateL2Table(tablePointer masterTable,
		uint32_t masterTableEntry, uint8_t domain) {
	tablePointer l2Table;
	uint32_t masterTableEntryValue;

	if (masterTableEntry < MMU_MASTER_TABLE_SIZE) {
		// we lookup the entry in the master table
		masterTableEntryValue = *(masterTable + masterTableEntry);

		// is there a valid pointer in the master table?
		if (masterTableEntryValue != 0) {
			l2Table =
					(tablePointer) MMU_MASTER_TABLE_ENTRY_TO_L2_ADDRESS(masterTableEntryValue);
		} else {
			// no L2 table found? - we create one
			l2Table = (tablePointer) MemFindFree(1, TRUE, TRUE);
			memset(l2Table, 0, MMU_L2_TABLE_SIZE);

			// compute table entry value
			masterTableEntryValue = ((uint32_t) l2Table)
					| MMU_MASTER_TABLE_TO_L2_INITIAL_VALUE | (domain << 5);

			// set the value in the masterPageTable
			*(masterTable + masterTableEntry) = masterTableEntryValue;
		}
	}

	return l2Table;
}

