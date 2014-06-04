/*
 * Driver: dr_elfloader.c
 * Part of BRO Project, 2014 <<https://github.com/BRO-FHV>>
 *
 * Created on: 28.05.2014
 * Description:
 * TODO
 */

/** ============================================================================
 *   \file  dr_elfloader.c
 *
 *   \brief This Files contain methods to load elf files
 *
 *  ============================================================================
 */

#include "dr_elfloader.h"
#include "elf_loader.h"
#include <basic.h>
#include "../scheduler.h"
#include "../mem/sc_mem.h"

Boolean elf_check(elf_header_t* header) {
	if (header->ident.magic != ELF_MAGIC) {
		return FALSE;
	}
	if (header->type != ELF_ET_EXEC) {
		return FALSE;
	}
	// TODO: Check machine type
	if (header->ident.class != ELF_CLASS_32) {
		return FALSE;
	}
	if (header->ident.data != ELF_DATA_2LSB) {
		return FALSE;
	}
	if (header->ident.version != ELF_EV_CURRENT) {
		return FALSE;
	}
	return TRUE;
}

void loadelffile(Process* process, uint32_t length, uint8_t* data, uint32_t* entryPoint) {
	void* processMemory;

	uint32_t pageCount;

	elf_header_t* header;
	elf_program_header_entry_t entry;
	uint32_t i;

	header = (elf_header_t*) data;

	if (elf_check(header)) {
		//
		// Copy and Map Pages
		*entryPoint = header->entry;
		for (i = 0; i < header->phnum; i++) {
			// copy the header to prevent alignment problems
			memcpy(&entry, (void*) (((uint32_t) header) + header->phoff	+ (header->phentsize * i)),	sizeof(elf_program_header_entry_t));
			if (entry.type == ELF_PT_LOAD)
			{
				// allocate pages needed for this program header
				pageCount = entry.filesz / MMU_MASTER_TABLE_SIZE;
				if ((entry.filesz % MMU_MASTER_TABLE_SIZE) > 0)
					pageCount++;
				processMemory = MemFindFree(pageCount, FALSE, TRUE);

				// copy the plain data of the program header
				memcpy(processMemory, (void*) ((uint32_t) header + entry.offset), entry.filesz);

				// map the virtual addressses of the new allocated program header
				MmuCreateAddressMappingRange(process->masterTable, (uint32_t) entry.vaddr, (uint32_t) processMemory, (uint32_t) processMemory + entry.filesz, 0);
			}
		}
	}
}

