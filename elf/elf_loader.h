/*
 * Scheduler: elf_loader.h
 * Part of BRO Project, 2014 <<https://github.com/BRO-FHV>>
 *
 * Created on: 28.05.2014
 * Description: 
 * TODO
 */

#ifndef ELF_LOADER_H_
#define ELF_LOADER_H_


void loadelffile(Process* process, uint32_t length, uint8_t* data, uint32_t*  entryPoint);

#endif /* ELF_LOADER_H_ */
