;
; Scheduler: dabt_handler.asm
; Part of BRO Project, 2014 <<https://github.com/BRO-FHV>>
;
; Created on: Mar 19, 2014
; Description:
; Assembler interrupt handler
;
; The ARM state register set:
; 	http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0211i/ch02s08s01.html
;
; shared header files
;
	.cdecls C,LIST,"soc_AM335x.h"

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
	.global MMUHandleDabt

;
; DABT handler function definition
;
;	+ SUB	= 	Subtract with or without Carry
;			-> 	http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0489c/CIHGJHHH.html
;
;	+ STMFD	=	The load and store multiple instructions can update the base register
;			->	http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0068b/Cacbgchh.html
;
;	+ MRS	= 	Move to ARM register from system coprocessor register
;			-> 	http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0489c/CIHGJHHH.html
;
;	+ LDR	=	The load instruction can be used to move single data item between register and memory
;			->	http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0041c/Babbfdih.html
;			->	http://www.bravegnu.org/gnu-eprog/arm-iset.html
;
;	+ STR	=	The store instruction can be used to move single data item between register and memory
;			->	http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0068b/BABDJCHA.html
;			->	http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0068b/BABDJCHA.html
;
;	+ AND	=	Bitwise AND logical operations
;			->	http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0068b/BABGIEBE.html
;
;
;	+ MOV	=	Instruction copies the value of operand2 into Rd (operand1)
;			->	http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0204j/Cihcdbca.html
;
;	+ CMP	=	Compare operation
;			->	http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0068b/CIHIDDID.html
;
;	+ STRNE	=	???
;			->	???
;
;	+ DSB	=	Data Synchronization Barrier acts as a special kind of memory barrier. No instruction in program order after this instruction executes until this instruction completes.
;			->	http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0489c/CIHGHHIE.html
;
;	+ ORR	=	Bitwise OR logical operations
;			->	http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0068b/BABGIEBE.html
;
;	+ BICNE	=	???
;			->	???
;
;	+ MOVEQ	=	Move if equal
;
;	+ LDMFD	=
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
	BL 		MMUHandleDabt

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
