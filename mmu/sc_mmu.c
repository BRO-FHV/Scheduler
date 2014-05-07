/*
 * Driver: sc_mmu.c
 * Part of BRO Project, 2014 <<https://github.com/BRO-FHV>>
 *
 * Created on: May 07, 2014
 * Description:
 * Virtual Memory
 */

#include "sc_mmu.h"
#include "../mem/sc_mem.h"
#include "../scheduler.h"
#include "../context.h"
#include <stdlib.h>
#include <string.h>
#include <basic.h>
#include <context/mem.h>
#include <context/mmu.h>

tablePointer kernel_master_table;

volatile uint32_t mmu_current_master_table;
volatile uint32_t mmu_accessed_address;
volatile uint32_t mmu_fault_state;

//
// General
//

void mmu_init(void)
{
    MemInit();

    uint32_t kernelPageCount;
    uint32_t i;

    __mmu_disable();

    //
    // reserve pages in all memories for the kernel so the processes can't use it
    MemoryType t;
    MemMemory* m;
    for (t = 0; t < memory_count; t++)
    {
        m = MemGet(t);
        kernelPageCount = (m->userStartAddress - m->globalStartAddress) / MMU_MASTER_TABLE_PAGE_SIZE;
        // if there is still a little space, reserve one more
        if (((m->userStartAddress - m->globalStartAddress) % MMU_MASTER_TABLE_PAGE_SIZE) > 0)
        {
            kernelPageCount++;
        }
        MemReservePages(t, 0, kernelPageCount);
    }

    //
    // create the kernel master table
    kernel_master_table = mmu_create_master_table();
    //
    // initialize the kernel master table so the lookup works for the kernel
    tablePointer currentTableEntry = kernel_master_table;
    for (i = 0x00000000; i < MMU_MAX_PROCESS_SPACE; i += MMU_SECTION_ENTRY_SIZE)
    {
        *currentTableEntry = i | MMU_SECTION_ENTRY_KERNEL_INITIAL;
        currentTableEntry++;
    }
    *currentTableEntry = 0xFFF00000 | MMU_SECTION_ENTRY_KERNEL_INITIAL; // don't miss the last one

    //
    // activate the mmu using the master table of the kernel
    mmu_set_kernel_table(kernel_master_table);
    mmu_set_process_table(kernel_master_table);
    // TODO: Change to 0x55555557 if everything works
    __mmu_set_domain_access(0xFFFFFFFF);
    __mmu_enable();
}

//
// MMU updating
//

void mmu_set_kernel_table(tablePointer table)
{
    uint32_t tableAddress = (uint32_t) table & MMU_ALIGN_ADDRESS;
    mmu_current_master_table = tableAddress;
    __mmu_flush_tlb();
    __mmu_set_kernel_table(tableAddress);
    __mmu_set_process_table(tableAddress);
}

void mmu_set_process_table(tablePointer table)
{
    uint32_t tableAddress = (uint32_t) table & MMU_ALIGN_ADDRESS;
    mmu_current_master_table = tableAddress;
    __mmu_flush_tlb();
    __mmu_set_process_table(tableAddress);
}

void mmu_switch_to_kernel()
{
    mmu_set_kernel_table(kernel_master_table);
}

void mmu_switch_to_process(Process* process)
{
    // if we alread have a master table, we use it
    if (process->masterTable == NULL)
    {
        printf("FATAL: no master table for process\n");
        return;
    }

    mmu_set_process_table(process->masterTable);
}

//
// MMU handler
//

Boolean mmu_handle_data_abort()
{
    mmu_accessed_address = 0;
    mmu_fault_state = 0;
    __mmu_load_dabt_data();

    mmu_switch_to_kernel();
    printf("data abort!\n");

    Process* currentProcess = SchedulerCurrentProcess();

    if (currentProcess == NULL)
    {
        printf("No process running, we keep going");
        return FALSE;
    }
    else
    {
        if (mmu_is_legal(mmu_accessed_address, mmu_fault_state))
        {
            printf("create mapped page\n");
            mmu_create_page_mapping(currentProcess->masterTable, mmu_accessed_address, 0);
            return TRUE;
        }
        else
        {
            printf("illegal accessed address (0x%x) at position (0x%x) bye bye pid %i\n", mmu_accessed_address,
                    0x4711, currentProcess->id);
            // FIXME scheduler_kill_current(global_scheduler);
            return FALSE;
        }
    }
}

Boolean mmu_handle_prefetch_abort()
{
    mmu_switch_to_kernel();
    // bad process! bye bye
    printf("prefetch abort!\n");
  //FIXME  scheduler_kill_current(global_scheduler);
    return FALSE; // false triggers scheduler run
}

//
// Master Table Handling
//

tablePointer mmu_create_master_table(void)
{
    // find some free pages in our memory to place the table
    tablePointer tableAddress = (tablePointer) MemFindFree(MMU_MASTER_TABLE_PAGE_COUNT, TRUE, TRUE);
    memset(tableAddress, 0, MMU_MASTER_TABLE_SIZE);
    return tableAddress;
}

void mmu_create_page_mapping(tablePointer masterTable, uint32_t virtualAddress, uint8_t domain)
{
    // we search for a new page
    void* physicalAddress = MemFindFree(1, TRUE, TRUE);
    // reset the page
    memset(physicalAddress, 0, MMU_MASTER_TABLE_PAGE_SIZE);
    // and map it to the given master table
    mmu_create_address_mapping(masterTable, virtualAddress, (uint32_t) physicalAddress, domain);
}

void mmu_create_direct_mapping_range(tablePointer masterTable, uint32_t physicalStartAddress,
        uint32_t physicalEndAddress, uint8_t domain)
{
    uint32_t i;
    uint32_t masterEntryCount;
    uint32_t masterEntryIndex;
    tablePointer currentTableEntry;
    // mark page as reserved
    masterEntryCount = (physicalEndAddress - physicalStartAddress) / MMU_SECTION_ENTRY_SIZE;
    // if there is still a little space, reserve one more
    if (((physicalEndAddress - physicalStartAddress) % MMU_SECTION_ENTRY_SIZE) > 0)
    {
        masterEntryCount++;
    }

    // map the addresses directly (as sections)
    masterEntryIndex = MMU_VIRTUAL_TO_MASTER_TABLE_ENTRY(physicalStartAddress);
    currentTableEntry = masterTable + masterEntryIndex;
    for (i = 0; i < masterEntryCount; i++)
    {
        *currentTableEntry = (masterEntryIndex << 20) | MMU_SECTION_ENTRY_KERNEL_INITIAL;

        masterEntryIndex++;
        currentTableEntry++;
    }
}

void mmu_create_address_mapping_range(tablePointer masterTable, uint32_t virtualStartAddress,
        uint32_t physicalStartAddress, uint32_t physicalEndAddress, uint8_t domain)
{
    // create mappings in l2 table steps
    uint32_t i;
    uint32_t l2EntryCount;
    // mark page as reserved
    l2EntryCount = (physicalEndAddress - physicalStartAddress) / MMU_L2_PAGE_SIZE;
    // if there is still a little space, reserve one more
    if (((physicalEndAddress - physicalStartAddress) % MMU_L2_PAGE_SIZE) > 0)
    {
        l2EntryCount++;
    }

    // map the addresses directly (as sections)
    for(i = 0; i < l2EntryCount; i++)
    {
        mmu_create_address_mapping(masterTable,
                virtualStartAddress + (i * MMU_L2_PAGE_SIZE),
                physicalStartAddress + (i* MMU_L2_PAGE_SIZE),
                domain
        );

    }
}

void mmu_create_address_mapping(tablePointer masterTable, uint32_t virtualAddress, uint32_t physicalAddress,
        uint8_t domain)
{
    // For an actual mapping we need to get the L2 table
    tablePointer l2Table;
    uint32_t l2EntryNumber;
    uint32_t l2EntryValue;
    uint32_t access;

    l2Table = mmu_get_or_create_l2_table(masterTable, MMU_VIRTUAL_TO_MASTER_TABLE_ENTRY(virtualAddress), domain);
    if (l2Table != NULL)
    {
        // now we build the l2 table entry
        l2EntryValue = MMU_L2_TABLE_ENTRY_TO_PAGE(physicalAddress) | MMU_L2_INITIAL_VALUE;
        access = HalMmuGetDomainAccess(domain);

        l2EntryValue |= (0x03 << 4); // Set all Access Bits
        l2EntryValue |= (0x01 << 9);

        // and write it to the l2 table
        l2EntryNumber = MMU_VIRTUAL_TO_L2_TABLE_ENTRY(virtualAddress);
        *(l2Table + l2EntryNumber) = l2EntryValue;
    }
    else
    {
        printf("FATAL: we ran out of memory, L2 Table could not be created\n");
    }
}

//
// L2 Table Handling
//

tablePointer mmu_get_or_create_l2_table(tablePointer masterTable, uint32_t masterTableEntry,
        uint8_t domain)
{
    tablePointer l2Table;
    uint32_t masterTableEntryValue;

    if (masterTableEntry < MMU_MASTER_TABLE_PAGE_SIZE)
    {
        // we lookup the entry in the master table
        masterTableEntryValue = *(masterTable + masterTableEntry);

        // is there a valid pointer in the master table?
        if (masterTableEntryValue != 0)
        {
            l2Table = (tablePointer) MMU_MASTER_TABLE_ENTRY_TO_L2_ADDRESS(masterTableEntryValue);
        }
        else
        {
            // no L2 table found? - we create one
            l2Table = (tablePointer) MemFindFree(1, TRUE, TRUE);
            memset(l2Table, 0, MMU_L2_TABLE_SIZE);

            // compute table entry value
            masterTableEntryValue = ((uint32_t) l2Table) | MMU_MASTER_TABLE_TO_L2_INITIAL_VALUE | (domain << 5);

            // set the value in the masterPageTable
            *(masterTable + masterTableEntry) = masterTableEntryValue;
        }
    }

    return l2Table;
}

//
// MMU utils / Process Stuff
//

void mmu_init_process(Process* process)
{
    // map the kernel pages in all memories
    MemoryType t;
    MemMemory* m;
    uint32_t i;
    for (t = 0; t < memory_count; t++)
    {
        m = MemGet(t);

        mmu_create_direct_mapping_range(process->masterTable, m->globalStartAddress, m->userStartAddress, 0);
    }

    //
    // map memory mapped ios
    tablePointer currentTableEntry;
    for (i = 0; i < device_memory_count; i++)
    {
        mmu_create_direct_mapping_range(process->masterTable, device_memories[i].startAddress,
                device_memories[i].startAddress + device_memories[i].size, 0);
    }

    //
    // map interrupt handlers
    mmu_create_direct_mapping_range(process->masterTable, intvecs_start, intvecs_start + intvecs_size, 0);
}

Boolean mmu_is_legal(uint32_t accessedAddress, uint32_t faultState)
{
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

    if (writeAccess)
    {
        switch (status)
        {
            case MMU_TRANSLATION_FAULT_SECTION:
            case MMU_TRANSLATION_FAULT_PAGE:
                // access to stack or heap?
                return (accessedAddress >= PROCESS_STACK_START
                        && accessedAddress < (PROCESS_STACK_START + PROCESS_STACK_SIZE))
                        || (accessedAddress >= PROCESS_SYSMEM_START
                                && accessedAddress < (PROCESS_SYSMEM_START + PROCESS_SYSMEM_SIZE));
            case MMU_DEBUG_EVENT:
                return TRUE;
            default:
                return FALSE;

        }
    }

    return FALSE;
}

Boolean mmu_is_process_page(uint32_t address)
{
    // check if address lies in a kernel space region of any memory
    MemoryType i;
    MemMemory* m;
    for (i = 0; i < memory_count; i++)
    {
        m = MemGet(i);

        // check if in kernel range
        if (address >= m->globalStartAddress && address < m->userStartAddress)
        {
            return FALSE;
        }
    }

    // check if it's not in interrupt vectors
    if (address >= intvecs_start && address < (intvecs_start + intvecs_size))
    {
        return FALSE;
    }

    return TRUE;
}

void mmu_delete_process_memory(Process* proc)
{
    tablePointer masterTable;
    uint32_t masterTableEntry;

    tablePointer l2Table;
    uint32_t l2TableEntry;

    uint32_t pageAddress;

    uint32_t pageNumber;
    MemoryType t;

    // cleanup all mastertable entries
    masterTable = proc->masterTable;
    for (masterTableEntry = 0; masterTableEntry < MMU_MASTER_TABLE_PAGE_SIZE; masterTableEntry++)
    {
        // cleanup the l2 table for the masterTableEntry if there is a table
        l2Table = (tablePointer) MMU_MASTER_TABLE_ENTRY_TO_L2_ADDRESS((*(masterTable + masterTableEntry)));
        if (l2Table != NULL)
        {
            // cleanup pages for all l2 entries
            for (l2TableEntry = 0; l2TableEntry < MMU_L2_PAGE_SIZE; l2TableEntry++)
            {
                // cleanup page for l2Entry
                pageAddress = MMU_L2_TABLE_ENTRY_TO_PAGE( (*(l2Table+ l2TableEntry)) );
                if (pageAddress != 0 && mmu_is_process_page(pageAddress))
                {
                    pageNumber = MemGetPageNumber(&t, pageAddress);
                    MemFreePages(t, pageNumber, 1);
                }

                // clean l2Entry
                *(l2Table + l2TableEntry) = 0;
            }

            // free the pages for the l2 Table
            pageNumber = MemGetPageNumber(&t, (uint32_t) l2Table);
            MemFreePages(t, pageNumber, 1);
        }

        // free the pages for the masterTableEntry
        pageNumber = MemGetPageNumber(&t, (uint32_t) masterTable);
        MemFreePages(t, pageNumber, MMU_MASTER_TABLE_PAGE_COUNT);
    }
}

// a manual implementation of the stuff the MMU does (for validation)
uint32_t mmu_virtual_to_physical(tablePointer masterTable, uint32_t virtualAddress)
{
    // lookup master table entry
    uint32_t masterTableEntry = MMU_VIRTUAL_TO_MASTER_TABLE_ENTRY(virtualAddress);
    uint32_t masterTableEntryValue;

    tablePointer l2Table;
    uint32_t l2TableEntry;

    uint32_t pageAddress;

    if (masterTableEntry < MMU_MASTER_TABLE_PAGE_SIZE)
    {
        // load L2 Table address
        masterTableEntryValue = (*(masterTable + masterTableEntry));

        // is this a page entry?
        if ((masterTableEntryValue & 0x01) == 0x01)
        {
            l2Table = (tablePointer) MMU_MASTER_TABLE_ENTRY_TO_L2_ADDRESS(masterTableEntryValue);
            if (l2Table != NULL)
            {
                // lookup L2 Entry
                l2TableEntry = MMU_VIRTUAL_TO_L2_TABLE_ENTRY(virtualAddress);
                if (l2TableEntry < MMU_L2_PAGE_SIZE)
                {
                    // load page address
                    pageAddress = MMU_L2_TABLE_ENTRY_TO_PAGE( *(l2Table + l2TableEntry) );

                    return pageAddress;
                }
            }
        }
        // is this a section entry?
        else if ((masterTableEntryValue & 0x02) == 0x02)
        {
            // upper 12 bits of entry (entry index) | lower 20 bits of virtual address
            return (masterTableEntryValue & 0xFFF00000) | (virtualAddress & 0xFFFFF);
        }
    }

    return 0;
}
