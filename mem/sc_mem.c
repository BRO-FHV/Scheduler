/*
 * Scheduler: sc_mem.c
 * Part of BRO Project, 2014 <<https://github.com/BRO-FHV>>
 *
 * Created on: 24.05.2014
 * Description:
 * TODO
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sc_mem.h"

Region regions[MEM_REGION_COUNT];

extern volatile uint32_t ext_ddr_user_start;

// internal function definitions
void _memInitOne(Region* region);
void* _memGetPageAddress(uint32_t type, uint32_t pageNumber);

void MemInit() {
	// Boot ROM
	regions[0].direct = TRUE;
	regions[0].start = 0x40000000;
	regions[0].end = 0x4002BFFF;
	_memInitOne(&regions[0]);

	// Memory Mapped IO
	regions[1].direct = TRUE;
	regions[1].start = 0x40300000;
	regions[1].end = 0x7FFFFFFF;
	_memInitOne(&regions[1]);

	// TODO get from linker script
	// SDRAM: Kernel
	regions[2].direct = TRUE;
	regions[2].start = 0x80000000;
	regions[2].end = 0x80FFFFFF;
	_memInitOne(&regions[2]);

	// SDRAM: Process Pages
	regions[3].direct = FALSE;
	regions[3].start = 0x81000000;
	regions[3].end = 0xBFFFFFFF;
	_memInitOne(&regions[3]);
}

/**
 * \brief Method init the memory of a defined region
 * \param  Region
 *
 * \return None
 *
 */
void _memInitOne(Region* region) {
	// calculate length of region
	region->length = region->end - region->start;
	// calculate page count direct use 1MB pages / indirect use 64kB pages
	region->totalPageCount = region->length / MEM_PAGE_SIZE;
	region->alocatedPageCount = 0;
	// init page lookup
	region->lookup = malloc(sizeof(PageTableLookup) * region->totalPageCount);
	memset(region->lookup, 0, sizeof(PageTableLookup) * region->totalPageCount);
}

Region* MemGet(uint32_t type) {
	return &regions[type];
}

void MemReservePages(uint32_t type, uint32_t firstPage, uint32_t pageCount) {
	uint32_t i;
	for (i = 0; i < pageCount; i++) {
		MemReservePage(type, firstPage + i);
	}
}

void MemReservePage(uint32_t type, uint32_t pageNumber) {
	Region* region = MemGet(type);
	if (pageNumber >= region->totalPageCount) {
		return;
	}
	region->lookup[pageNumber].reserved = TRUE;
	region->alocatedPageCount++;
}

void* MemFindFree(uint32_t pageCount, Boolean align, Boolean reserve) {
	void* address;
	uint32_t i;
	for (i = 0; i < MEM_REGION_COUNT; i++) {
		address = MemFindFreeIn(i, pageCount, align, reserve);
		if (address != NULL) {
			return address;
		}
	}
	return NULL;
}

void* MemFindFreeIn(uint32_t type, uint32_t pageCount, Boolean align,
		Boolean reserve) {
	// find pageCount unoccupied pages in a row
	void* result = NULL;
	Region* region = MemGet(type);
	uint32_t pagesFound = 0;
	uint32_t i;

	for (i = 0; i < region->totalPageCount; i++) {
		if (!region->lookup[i].reserved
				&& ((!align) || (pagesFound > 0) || ((i % pageCount) == 0))) {
			pagesFound++;
			if (pagesFound == pageCount) {
				result = _memGetPageAddress(type, (i - pageCount) + 1);
				if (reserve) {
					MemReservePages(type, (i - pageCount) + 1, pageCount);
				}
				break;
			}
		} else {
			pagesFound = 0;
		}
	}
	return result;
}

/**
 * \brief Method to get a mem address out of a Page
 * \param  Region
 *
 * \return None
 *
 */
void* _memGetPageAddress(uint32_t type, uint32_t pageNumber) {
	Region* region = MemGet(type);
	if (pageNumber >= region->totalPageCount) {
		return NULL;
	}
	return (void*) (region->start + (pageNumber * MEM_PAGE_SIZE));
}

