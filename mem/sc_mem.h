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


/**
 * \brief  Used to initialize the Memory for the MMU
 *
 * \param  None
 *
 * \return None
 *
 * \note  Used to initialize the Memory for the MMU
 */
void MemInit();


/**
 * \brief  Used to get a memory region by the region number
 *
 * \param  type, number of the region
 *
 * \return memory region
 *
 */
Region* MemGet(uint32_t type);

/**
 * \brief Method to Reserve Pages out of the Memory Region
 *
 * \param  type, number of the region
 * \param  firstPage, first Page
 * \param  pageCount, PageCount
 *
 * \return void
 *
 */
void MemReservePages(uint32_t type, uint32_t firstPage, uint32_t pageCount);

/**
 * \brief Method to Reserve Page out of the Memory Region
 *
 * \param  type, number of the region
 * \param  pageNumber, PageCount
 *
 * \return memory void
 *
 */
void MemReservePage(uint32_t type, uint32_t pageNumber);

/**
 * \brief Method to Find a Free Memory
 *
 * \param  pageCount
 * \param  align
 * \param  reserve
 *
 * \return void*
 *
 */
void* MemFindFree(uint32_t pageCount, Boolean align, Boolean reserve);

/**
 * \brief Method to Find a Free Memory in a defined Region
 *
 * \param type (number of Region with is used)
 * \param  pageCount
 * \param  align
 * \param  reserve
 *
 * \return void*
 *
 */
void* MemFindFreeIn(uint32_t type, uint32_t pageCount, Boolean align, Boolean reserve);

#endif /* SC_MEM_H_ */
