	.global __context_save
;	.global last_interrupt_source

	.global current_context
_current_context: .field current_context, 32
;_last_interrupt_source: .field last_interrupt_source, 32

__context_save:
	; Backup R14 of _context_save
	STMFD R13!, {R14}

	; Store source
	LDR R12, [R13, #8]

	;LDR R14, _last_interrupt_source;
	;STMIA R14, {R12}

	; Load context pointer into R14
	LDR R14, _current_context
	LDR R14, [R14]

	; Load R14 of calling function into context (return address)
	; (at PC position in context)
	LDR R12, [R13, #8]
	STMIA R14!, {R12}

	; Restore correct R12 in case of tramping
	LDR R12, [R13, #4]

	; Store Registers in Context
	STMIA R14, {R0-R14}^

	; Store CPSR in Context
	MRS R12, SPSR
	STR R12, [R14, #60]

	; Restore Correct R14
	LDMFD R13!, {R14}
	; Jump back to caller
	MOV PC, R14
