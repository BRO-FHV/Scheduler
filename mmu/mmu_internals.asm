	.global __mmu_enable
	.global __mmu_disable

	.global __mmu_flush_tlb
	.global __mmu_set_kernel_table
	.global __mmu_set_process_table

	.global __mmu_set_domain_access
	.global __mmu_load_dabt_data

	.global mmuAccessedAddress
	.global mmuFaultState

_mmu_accessed_address: .field mmuAccessedAddress, 32
_mmu_fault_state: .field mmuFaultState, 32

; SCTLR_TE (1<<30)
; SCTLR_AFE (1<<29)
; SCTLR_TRE (1<<28)
; SCTLR_ICACHE (1<<12)    /* I */
; SCTLR_PREDICT (1<<11)   /* Z */
; SCTLR_DCACHE (1<<2)     /* C */
; SCTLR_ALIGN (1<<1)      /* A */
; SCTLR_MMUEN (1<<0)      /* M */

__mmu_enable:
	STMFD R13!, {R0,R1} ; backup r0, r1 on stack

	; Load current mmu flags into R0
	MRC P15, #0, R0, C1, C0, #0
	; add SCTLR_ICACHE | SCTLR_DCACHE | SCTLR_PREDICT | SCTLR_MMUEN = 0x1805
	;MOV R1, #0x1805
	MOV R1, #0x01
	ORR R0, R0, R1
	; Write mmu flags back to coprocessor
	MCR P15, #0, R0, C1, C0, #0

   	LDMFD R13!, {R0,R1} ; restore r0,r1 and jump back
   	MOV PC, R14



__mmu_disable:
	STMFD R13!, {R0, R1} ; backup r0, r1 on stack

	; Load current mmu flags into R0
	MRC P15, #0, R0, C1, C0, #0
	; clearup flags ~( SCTLR_TRE | SCTLR_AFE |  SCTLR_ICACHE | SCTLR_DCACHE | SCTLR_MMUEN) = ~(0x30001005)
	; = 0xCFFFEFFA
	MOV R1, #0xCFFF
	MOVT R1, #0xEFFA
	AND R0, R0, R1
	; Write mmu flags back to coprocessor
	MCR P15, #0, R0, C1, C0, #0

  	LDMFD R13!, {R0, R1} ; restore r0,r1 and jump back
   	MOV PC, R14



__mmu_flush_tlb:
	STMFD R13!, {R0, R1} ; backup r0, r1 on stack

	MOV R0, #0
	MCR P15, #0, R0, C8, C7, #0

  	LDMFD R13!, {R0, R1} ; restore r0,r1 and jump back
   	MOV PC, R14

__mmu_set_process_table:
	MCR P15, #0, R0, C2, C0, #0 ; Set master table address
   	MOV PC, R14

__mmu_set_kernel_table:
	MCR P15, #0, R0, C2, C0, #1 ; Set master table address
   	MOV PC, R14



__mmu_set_domain_access:
	MCR P15, #0, R0, C3, C0, #0
	MOV PC, R14


__mmu_load_dabt_data:
	MRC   P15, #0, R0, C6, C0, #0
	LDR   R1, _mmu_accessed_address
	STR   R0, [R1]

	MRC p15, #0, r0, c5, c0, #0
	LDR r1, _mmu_fault_state
	STR r0, [r1]

	MOV PC, R14

