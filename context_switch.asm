	.global context_switch
	.ref scheduler_runNextProcess

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

context_switch:
	STMFD	SP, { R0 - R14 }^			; store user-registers on stack
	SUB		SP, SP, #60					; decrement stack-pointer: 15 * 4 bytes = 60bytes

	STMFD	SP!, { LR }					; store LR in stack to either restore it after BL to allow scheduler change it (and restore it after return from BL)

	MRS		R1, SPSR					; fetch user-cpsr (SPSR) value to R1
	STMFD	SP!, { R1 }					; store SPSR in stack to allow scheduler change it

	MOV 	R0, SP						; pointer to SP in R0, to point to UserContext-struct

	; irqHandler( UserContext* ctx )
	BL		scheduler_runNextProcess	; branch AND link to irq parent handler

	; TODO: do a BEQ on R0:
	; if R0 == 1 then scheduling was performed

	LDMFD	SP!, { R1 }					; restore SPSR, if changed by scheduler
	MSR		SPSR_cxsf, R1				; set stored cpsr from user to the current CPSR - will be restored later during SUBS

	LDMFD	SP!, { LR }					; restore LR, if changed by scheduler

	LDMFD	SP, { R0 - R14 }^			; restore user-registers, if changed by scheduler
	ADD		SP, SP, #60					; increment stack-pointer: 15 * 4 bytes = 60bytes

	; NOTE: at this point SP should be at starting address again
	MOV		r0, #MASK_NEW_IRQ			; load mask for new IRQ generation in r0
	LDR		r1, ADDR_CONTROL			; load address for interrupt control register in r1
	STR		r0, [r1, #0]				; store content of r2 in RAM address in r1 + offset 0

 	; TODO: when a process-switch was performed: MOVS	PC, LR should be enough, otherwise we must return to the instruction which was canceled by IRQ thus using SUBS
 	SUBS	PC, LR, #4					; return from IRQ


