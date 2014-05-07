	.global __context_log
	.global __context_load

	.global current_context
_current_context: .field current_context, 32

__context_load:
	; Load Context Pointer
	LDR R14, _current_context
	LDR R14, [R14]

	; Jump to the end of the context
	ADD R14, R14, #64

	; Restore the CPSR of the context
	LDR R12, [R14]
	MSR SPSR_cxsf, R12

	; Restore user registers
	LDMDB R14, {R0-R14}^

	; Load Program Counter
	LDR R14, [R14,#-64]

	; Jump back to process
	MOVS PC, R14
