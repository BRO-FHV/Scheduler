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

/**
 * \brief  Method to enable the MMU
 *
 * \param  None
 *
 * \return None
 *
 * \note  Need in asm
 */
void __mmu_enable(void);
/**
 * \brief  Method to disable the MMU
 *
 * \param  None
 *
 * \return None
 *
 * \note  Need in asm
 */
void __mmu_disable(void);

/**
 * \brief  Method to flush  the TLB register  MMU
 *
 * \param  None
 *
 * \return None
 *
 * \note  Need in asm
 */
void __mmu_flush_tlb();

/**
 * \brief  Method set the activ kernel table
 *
 * \param  address to the kernel table
 *
 * \return None
 *
 * \note  Need in asm
 */
void __mmu_set_kernel_table(uint32_t tableAddress);

/**
 * \brief  Method set the activ process table
 *
 * \param  address to the process table
 *
 * \return None
 *
 * \note  Need in asm
 */
void __mmu_set_process_table(uint32_t tableAddress);

/**
 * \brief  Method to set the domain access
 *
 * \param  level of the domainAccess
 *
 * \return None
 *
 * \note  Need in asm
 */
void __mmu_set_domain_access(uint32_t domainAccess);

/**
 * \brief  Method to load dapt data
 *
 * \param  None
 *
 * \return None
 *
 * \note  Need in asm
 */
void __mmu_load_dabt_data(void);

#endif /* MMU_INTERNALS_H_ */
