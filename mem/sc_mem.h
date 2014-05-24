/*
 * Scheduler: sc_mem.h
 * Part of BRO Project, 2014 <<https://github.com/BRO-FHV>>
 *
 * Created on: 24.05.2014
 * Description:
 * TODO
 */

#ifndef SC_MEM_H_
#define SC_MEM_H_

#include <inttypes.h>
#include "basic.h"

#define MEM_REGION_COUNT 5
#define MEM_PAGE_SIZE 0x10000

typedef struct pagetableLookup {
	Boolean reserved;
	Boolean occupied;
} PageTableLookup;

typedef struct region {
	Boolean direct;
	uint32_t start;
	uint32_t length;
	uint32_t end;
	uint32_t totalPageCount;
	uint32_t alocatedPageCount;
	PageTableLookup* lookup;
} Region;

void MemInit();

Region* MemGet(uint32_t type);

void MemReservePages(uint32_t type, uint32_t firstPage, uint32_t pageCount);

void MemReservePage(uint32_t type, uint32_t pageNumber);

void* MemFindFree(uint32_t pageCount, Boolean align, Boolean reserve);

void* MemFindFreeIn(uint32_t type, uint32_t pageCount, Boolean align, Boolean reserve);

#endif /* SC_MEM_H_ */
