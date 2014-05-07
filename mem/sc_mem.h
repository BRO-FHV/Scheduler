/*
 * Driver: sc_mem.h
 * Part of BRO Project, 2014 <<https://github.com/BRO-FHV>>
 *
 * Created on: May 07, 2014
 * Description:
 * Physical Memory
 */

#ifndef MEM_GENERIC_H_
#define MEM_GENERIC_H_

#include <basic.h>
#include <context/mem.h>

MemMemory* MemGet(MemoryType t);
void MemInit(void);

void MemReservePage(MemoryType type, uint32_t pageNumber);
void MemReservePages(MemoryType type, uint32_t firstPageNumber, uint32_t pageCount);

void MemFreePage(MemoryType type, uint32_t pageNumber);
void MemFreePages(MemoryType type, uint32_t firstPageNumber, uint32_t pageCount);

void* MemGetPageAddress(MemoryType type, uint32_t pageNumber);
uint32_t MemGetPageNumber(MemoryType* type, uint32_t address);

void* MemFindFree(uint32_t pageCount, Boolean align, Boolean reserve);
void* MemFindFreeIn(MemoryType type, uint32_t pageCount, Boolean align, Boolean reserve);

#endif /* MEM_H_ */
