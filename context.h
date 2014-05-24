/*
 * Driver: contex.h
 * Part of BRO Project, 2014 <<https://github.com/BRO-FHV>>
 *
 * Created on: May 07, 2014
 * Description:
 * TODO
 */

#ifndef CONTEXT_H_
#define CONTEXT_H_

#define PROCESS_CODE_START 0x00020000
#define PROCESS_STACK_START 0x81200000
#define PROCESS_STACK_SIZE 128000

#define PROCESS_SYSMEM_START 0x81202000
#define PROCESS_SYSMEM_SIZE 0x40000000

#define PROCESS_CONTEXT_SIZE 17
#define DECLARE_PROCESS_CONTEXT(name) volatile void* name[PROCESS_CONTEXT_SIZE];

#endif /* CONTEXT_H_ */
