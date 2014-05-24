/*
 * Scheduler: mmu_internals.h
 * Part of BRO Project, 2014 <<https://github.com/BRO-FHV>>
 *
 * Created on: 24.05.2014
 * Description:
 * TODO
 */

#ifndef MMU_INTERNALS_H_
#define MMU_INTERNALS_H_

#include <stdint.h>

void __mmu_enable(void);
void __mmu_disable(void);

void __mmu_flush_tlb();
void __mmu_set_kernel_table(uint32_t tableAddress);
void __mmu_set_process_table(uint32_t tableAddress);

void __mmu_set_domain_access(uint32_t domainAccess);
void __mmu_load_dabt_data(void);

#endif /* MMU_INTERNALS_H_ */
