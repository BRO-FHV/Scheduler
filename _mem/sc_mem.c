/*
 * Driver: sc_mem.c
 * Part of BRO Project, 2014 <<https://github.com/BRO-FHV>>
 *
 * Created on: May 07, 2014
 * Description:
 * Physical Memory
 */

#include "sc_mem.h"
#include "../mmu/sc_mmu.h"
#include <context/mmu.h>
#include <stdlib.h>
#include <string.h>
#include <basic.h>

uint32_t intvecs_start;
uint32_t intvecs_size;

uint8_t memory_count;
MemMemory* memories;

uint8_t device_memory_count;
MemDevicememory* device_memories;

// Two booleans for each entry
MemMemory* MemGet(MemoryType t)
{
    return &memories[t];
}

void MemInit(void)
{
    MemoryType i;
    HalMemInit();

    for(i = 0; i < memory_count; i++)
    {
        memories[i].pageTableTotalCount = memories[i].globalSize  / MMU_MASTER_TABLE_PAGE_SIZE;
        memories[i].pageTableAllocatedCount = 0;
        memories[i].pageTableLookup = malloc(sizeof(MemPagetableLookup) * memories[i].pageTableTotalCount);
        memset(memories[i].pageTableLookup, 0, sizeof(MemPagetableLookup) * memories[i].pageTableTotalCount);
    }
}

void MemReservePages(MemoryType type, uint32_t firstPageNumber, uint32_t pageCount)
{
    int i;
    for (i = 0; i < pageCount; i++)
    {
        MemReservePage(type, firstPageNumber + i);
    }
}

void MemReservePage(MemoryType type, uint32_t pageNumber)
{
    MemMemory* m = MemGet(type);
    if (pageNumber >= m->pageTableTotalCount)
    {
        return;
    }
    m->pageTableLookup[pageNumber].reserved = TRUE;
    m->pageTableAllocatedCount++;
    return;
}

void MemFreePage(MemoryType type, uint32_t pageNumber)
{
    MemMemory* m = MemGet(type);
    if (pageNumber >= m->pageTableTotalCount)
    {
        return;
    }
    m->pageTableLookup[pageNumber].reserved = FALSE;
    m->pageTableAllocatedCount--;
    // reset the memory of the page
    memset(MemGetPageAddress(type, pageNumber), 0, MMU_MASTER_TABLE_PAGE_SIZE);
}

void MemFreePages(MemoryType type, uint32_t firstPageNumber, uint32_t pageCount)
{
    int i;
    for (i = 0; i < pageCount; i++)
    {
        MemFreePage(type, firstPageNumber + i);
    }
}

void* MemGetPageAddress(MemoryType type, uint32_t pageNumber)
{
    MemMemory* m = MemGet(type);
    if (pageNumber >= m->pageTableTotalCount)
    {
        return NULL;
    }
    return (void*) (m->globalStartAddress + (pageNumber * MMU_MASTER_TABLE_PAGE_SIZE));
}

uint32_t MemGetPageNumber(MemoryType* type, uint32_t address)
{
    MemoryType i;
    MemMemory* m;
    for(i = 0; i < memory_count; i++)
    {
        m = MemGet(i);
        if(address >= m->globalStartAddress && address < (m->globalStartAddress + m->globalSize))
        {
            *type = i;
            return (address - m->globalStartAddress) / MMU_MASTER_TABLE_PAGE_SIZE;
        }
    }
    return 0;
}

void* MemFindFreeIn(MemoryType type, uint32_t pageCount, Boolean align, Boolean reserve)
{
    //
    // find pageCount unoccupied pages in a row
    void* result = NULL;
    MemMemory* m = MemGet(type);
    uint32_t pagesFound = 0;
    uint32_t i;

    for (i = 0; i < m->pageTableTotalCount; i++)
    {
        if (!m->pageTableLookup[i].reserved && ((!align) || (pagesFound > 0) || ((i % pageCount) == 0)))
        {
            pagesFound++;
            if (pagesFound == pageCount)
            {
                result = MemGetPageAddress(type, (i - pageCount) + 1);
                if (reserve)
                {
                    MemReservePages(type, (i - pageCount) + 1, pageCount);
                }
                break;
            }
        }
        else
        {
            pagesFound = 0;
        }
    }
    return result;
}

void* MemFindFree(uint32_t pageCount, Boolean align, Boolean reserve)
{
    void* address;
    MemoryType i;
    for(i = 0; i < memory_count; i++)
    {
        address = MemFindFreeIn(i, pageCount, align, reserve);
        if(address != NULL)
        {
            return address;
        }
    }
    return NULL;
}
