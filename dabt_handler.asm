;
; Scheduler: dabt_handler.asm
; Part of BRO Project, 2014 <<https://github.com/BRO-FHV>>
;
; Created on: 24.05.2014
; Description:
; Assembler dabt handler
;
; The ARM state register set:
; 	http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0211i/ch02s08s01.html
;
; shared header files
;
	.cdecls C,LIST,"interrupt/hw_interrupt.h"
	.cdecls C,LIST,"soc_AM335x.h"

;
; global regiter definitions
;
ADDR_THRESHOLD		.word	SOC_AINTC_REGS + INTC_THRESHOLD
ADDR_IRQ_PRIORITY	.word	SOC_AINTC_REGS + INTC_IRQ_PRIORITY
ADDR_SIR_IRQ		.word	SOC_AINTC_REGS + INTC_SIR_IRQ
ADDR_CONTROL		.word	SOC_AINTC_REGS + INTC_CONTROL

;
; bit masks
;
MASK_ACTIVE_IRQ		.set	INTC_SIR_IRQ_ACTIVEIRQ
MASK_NEW_IRQ		.set	INTC_CONTROL_NEWIRQAGR
MASK_SYS_MODE		.set	0x1F
MASK_IRQ_MODE		.set	0x12
MASK_I_BIT			.set	0x80
MASK_SWI_NUM		.set	0xFF000000

;
; define section of memory
;
	.text

;
; source file is assembled for ARM instructions
;
	.state32

;
; define global symbols (share between c and asm)
;
	.global dabt_handler
	.global MmuHandleDabt

;
; DABT handler function definition
;
dabt_handler:
	;
	; backup user context
	;	+ R13 	= stackpointer
	;	+ R14 	= link register
	;	+ SPSR	= saved program status register
	; 	+ LR	= link register
	;
	STMFD	SP, { R0 - R14 }^			; backup user context in irq stack
	SUB		SP, SP, #60					; LR correction
	STMFD	SP!, { LR }					; store LR in stack
	MRS		r1, spsr					; copy SPSR
	STMFD	SP!, {r1}					; backup SPSR in stack
	MOV 	R0, SP						; ??? pointer to SP in R0, to point to Context-struct

	;
	; start interrupt handler
	;
	BL 		MmuHandleDabt

	;
	; enable IRQ generation
	;
	MOV		r3, #MASK_NEW_IRQ			; load mask for new IRQ generation in r0
	LDR		r4, ADDR_CONTROL			; load address for interrupt control register in r1
	STR		r3, [r4, #0]				; store content of r2 in RAM address in r1 + offset 0

	;
	; TODO change comments
	;
	LDMFD	SP!, { R1 }					; restore SPSR, if changed by scheduler
	MSR		SPSR_cxsf, R1				; set stored cpsr from user to the current CPSR - will be restored later during SUBS

	LDMFD	SP!, { LR }					; restore LR, if changed by scheduler

	LDMFD	SP, { R0 - R14 }^			; restore user-registers, if changed by scheduler
	ADD		SP, SP, #60					; increment stack-pointer: 15 * 4 bytes = 60bytes

 	; TODO: when a process-switch was performed: MOVS	PC, LR should be enough, otherwise we must return to the instruction which was canceled by IRQ thus using SUBS
 	SUBS	PC, LR, #4					; return from IRQ
