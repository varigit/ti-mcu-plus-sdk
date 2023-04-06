//
// DM Stub firmware
//
// Copyright (C) 2021 Texas Instruments Incorporated - http://www.ti.com/
//
// This software is licensed under the standard terms and conditions in the
// Texas Instruments Incorporated Technology and Software Publicly
// Available Software License Agreement, a copy of which is included in
// the software download.
//

	.global _stub_start
	.global undef_handler
	.global swi_handler
	.global prefetch_abt_handler
	.global data_abt_handler
	.global reserved_handler
	.global irq_handler
	.global fiq_handler
	.global stub_entry
	.global _end_stack

	 
	 

       .text
       .arm
        
        

_vectors:

_reset:			ldr pc, _reset_handler
_undef:			ldr pc, _undef_handler
_swi:			ldr pc, _swi_handler
_prefetch_abt:		ldr pc, _prefetch_abt_handler
_data_abt:		ldr pc, _data_abt_handler
_reserved:		ldr pc, _reserved_handler
_irq:			ldr pc, _irq_handler
_fiq:			ldr pc, _fiq_handler

_reset_handler:
	.word _stub_start
_undef_handler:
	.word undef_handler
_swi_handler:
	.word swi_handler
_prefetch_abt_handler:
	.word prefetch_abt_handler
_data_abt_handler:
	.word data_abt_handler
_reserved_handler:
	.word reserved_handler
_irq_handler:
	.word irq_handler_asm
_fiq_handler:
	.word fiq_handler
_end_vectors:

_reset_vector_address:
	.word 0x00000000

_stub_start:
	// save r4-r11 registers
	stmdb   sp!,{r4-r11}
	// Save current stack pointer
	adr r1, _sys_stack_pointer
	str sp, [r1]
	adr r1, _sys_lr_register
	str lr, [r1]

	// Copy vector table
	adr     r0, _reset
	ldr     r1, _reset_vector_address
	MOV     r2, #0x10
wordcopy:
	ldr     r3, [r0], #4
	str     r3, [r1], #4
	SUBS    r2, r2, #1
	BNE     wordcopy

	// switch to irq mode, set irq stack
	mrs	r0, cpsr
	mvn	r1, #0x1f
	and	r2, r1, r0
	orr	r2, r2, #0x12
	msr	cpsr_cf, r2
	adr r1, _sys_irq_stack_pointer // save the current irq stack pointer
	str sp, [r1]
	ldr	r13, _irq_stack_addr
	mov 	r14,#0

	//switch to fiq mode and save stack pointer
	mrs	r0, cpsr
	bic r0, r0, #0x1f
	orr r0, r0, #0x11
	msr	cpsr_cf, r0
	adr r1, _sys_fiq_stack_pointer
	str sp, [r1]

	//switch to abort mode and save stack pointer
	mrs	r0, cpsr
	bic r0, r0, #0x1f
	orr r0, r0, #0x17
	msr	cpsr_cf, r0
	adr r1, _sys_abort_stack_pointer
	str sp, [r1]

	//switch to undefined mode and save stack pointer
	mrs	r0, cpsr
	bic r0, r0, #0x1f
	orr r0, r0, #0x1b
	msr	cpsr_cf, r0
	adr r1, _sys_undefined_stack_pointer
	str sp, [r1]

	//switch to supervisor mode and save stack pointer
	mrs	r0, cpsr
	bic r0, r0, #0x1f
	orr r0, r0, #0x13
	msr	cpsr_cf, r0
	adr r1, _sys_supervisor_stack_pointer
	str sp, [r1]


	// switch to SYS mode
	mrs	r0, cpsr
	mvn	r1, #0x1f
	and	r2, r1, r0
	orr	r2, r2, #0x1f
	msr	cpsr_cf, r2

	// initialize the stack
	ldr sp, _stack_addr
	// Space for the jump
	nop
	nop

	// At this point we should be CPU0
	// jump to proper reset handler
	bl stub_entry

	//save the return value to r3
	mov r3, r0
	// switching back to DDR
	// disable interrupt
	cpsid i
	// switch to irq mode, set irq stack
	mrs	r0, cpsr
	mvn	r1, #0x1f
	and	r2, r1, r0
	orr	r2, r2, #0x12
	msr	cpsr_cf, r2
	ldr	r13, _sys_irq_stack_pointer
	mov 	r14,#0

	//switch to fiq mode and restore stack pointer
	mrs	r0, cpsr
	bic r0, r0, #0x1f
	orr r0, r0, #0x11
	msr	cpsr_cf, r0
	ldr	r13, _sys_fiq_stack_pointer

	//switch to abort mode and restore stack pointer
	mrs	r0, cpsr
	bic r0, r0, #0x1f
	orr r0, r0, #0x17
	msr	cpsr_cf, r0
	ldr	r13, _sys_abort_stack_pointer

	//switch to undefined ort mode and restore stack pointer
	mrs	r0, cpsr
	bic r0, r0, #0x1f
	orr r0, r0, #0x1b
	msr	cpsr_cf, r0
	ldr	r13, _sys_undefined_stack_pointer

	//switch to supervisor mode and restore stack pointer
	mrs	r0, cpsr
	bic r0, r0, #0x1f
	orr r0, r0, #0x13
	msr	cpsr_cf, r0
	ldr	r13, _sys_supervisor_stack_pointer


	// switch to SYS mode
	mrs	r0, cpsr
	mvn	r1, #0x1f
	and	r2, r1, r0
	orr	r2, r2, #0x1f
	msr	cpsr_cf, r2

	//restore the return value from r3
	mov r0, r3
	// reload the stack
	ldr sp, _sys_stack_pointer
	ldmia	sp!,{r4-r11}
	// Space for the jump
	nop
	nop
	ldr pc, _sys_lr_register

irq_handler_asm:
	stmdb	sp!,{r0-r3,r12,lr}
	bl irq_handler
	ldmia	sp!,{r0-r3,r12,lr}
	subs	pc, lr, #4

_stack_addr:
	.word _end_stack
_irq_stack_addr:
	.word _end_stack - 0x200
_sys_stack_pointer:
	.word 0x00000000
_sys_lr_register:
	.word 0x00000000
_sys_irq_stack_pointer:
	.word 0x00000000
_sys_fiq_stack_pointer:
	.word 0x00000000
_sys_abort_stack_pointer:
	.word 0x00000000
_sys_undefined_stack_pointer:
	.word 0x00000000
_sys_supervisor_stack_pointer:
	.word 0x00000000
