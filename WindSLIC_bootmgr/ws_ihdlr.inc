; ws_ihdlr.inc - interrupt handler include file
;-----------------------------------------------------------------------
;-----------------------------------------------------------------------
; -v-  Int15H-related Functions and Macros
;-----------------------------------------------------------------------

;-----------------------------------------------------------------------
; -v- replace_handler__int15h:
;-----------------------------------------------------------------------
;startp "replace_handler__int15h:"
;-----------------------------------------------------------------------
replace_handler__int15h:

	cli						; Disable Interrupts while replacing the Interrupt Handler
	get_ivt_entry_to_reg	eax,15h 		; Get old int15h handler into EAX
	set_ivt_entry_from_reg	ENTRY_HOLD_OLD_I15H,eax ; and save it back into spot for int90h
	cmp_ivt_entry_against	ENTRY_HOLD_OLD_I15H,eax ; Compare to verify successful write

							; If something is blocking our write, don't hook the interrupt.
	sti						; Re-Enable Interrupts after replacing the Interrupt Handler
	jne	replace_handler__int15h.save_failed

replace_handler__int15h.save_ok:			; If here, Save was ok, so hook the Interrupt
	set_ivt_entry_from_segoff 15h,cs,handler_int15h ; Do the actual hooking here.
	clc						; Clear the Carry Flag to Indicate Success
	jmp	replace_handler__int15h.done

replace_handler__int15h.save_failed:			; If here, then save failed,
							; So print err msg and set CF.
							; Then return to the caller.
	;disp_str        err_fail_i15h_handler_save      ; "Error: Unable to save old int15h handler."
	stc						; Set the Carry Flag to Indicate Failure
	;jmp     replace_handler__int15h.done
replace_handler__int15h.done:
	retn
;-----------------------------------------------------------------------
;endofp "replace_handler__int15h:"
;-----------------------------------------------------------------------
; -^- replace_handler__int15h:
;-----------------------------------------------------------------------

;-----------------------------------------------------------------------
; int15h,ax=e820h
; we need to sniff the cont_code
; if the cont_code is the one we're overriding,
;       return (our free code, with cont_code set to "rsv1")
; if the cont_code is rsv1,
;       return (our resvd code, with cont_code set to whatever the next cont_code should be)

; so, for our two extra memory lines:
; free:
;       cont_code = [overridden cont_code]
;       next_code = "RSV1"
; resvd:
;       cont_code = "RSV1"
;       next_code = [next contcode past overridden]
;-----------------------------------------------------------------------

;-----------------------------------------------------------------------
; -v- handler_int15h:
;-----------------------------------------------------------------------
;startp "handler_int15h"
;-----------------------------------------------------------------------
handler_int15h:

	;Replacement Int15H Handler

	;---------------------------------------
	; -v- Frame Definition for This Routine
	;---------------------------------------
	; EBP   +       0x08    Caller FLAGS
	; EBP   +       0x06    Caller CS
	; EBP   +       0x04    Caller IP
	;=EBP   +       0x00    Saved EBP
	; EBP   -       0x04    Saved EFLAGS
	;---------------------------------------
	; -^- Frame Definition for This Routine
	;---------------------------------------
	
	cli						;Step 1. Disable interrupts until we're ready for them.

	setup_frame_pointer				;Step 2. Setup the frame pointer (pushes ebp, sets ebp to esp)

	pushfd						;Step 3, Save Entry Flags to ebp-4
							;NOTE: this will only be cleaned up teardown_frame_pointer

	;!!!!!!!!!!!!
	;!!! Apparently bp works but ebp doesnt!
	;!!!!!!!!!!!!
	
	;TMP JUMP TO DEF HALD
	;jmp hi15h.dump_to_default_hdlr                 ;Otherwise, just use the default handler

	cmp ax,0xE820					; Step 4. Compare AX against "0xE820" call
	jne hi15h.dump_to_default_hdlr			; If no match, just use the default_handler


	cmp_ivt_entry_against ENTRY_HOLD_IH_ENABLE,0x01 ; Step 5. Check if our overide is enabled
	jne hi15h.dump_to_default_hdlr			; If no match, just use the default_handler


	cmp_ivt_entry_against ENTRY_HOLD_E820_CCODE,0	; Step 6. Compare our stored E820 Cont-Code against 0
	je hi15h.dump_to_default_hdlr			; Zero indicates problem, so use default handler


	cmp_ivt_entry_against ENTRY_HOLD_E820_CCODE,ebx ; Step 7. Compare against our Target E820 Cont-Code
	je hi15h.handle_our_free			; If Matched, jmp to handle_our_free


	cmp ebx, 'RSVD' 				; Step 8. Otherwise, Compare against our Reserved E820 Cont-Code
	je hi15h.handle_our_rsvd			; If Matched, jmp to handle_our_rsvd

	jmp hi15h.dump_to_default_hdlr			; Otherwise, just use the default handler

hi15h.dump_to_default_hdlr:				

	call int_handler_tools.recover_saved_eflags	; Call a support routine to Get our Saved Eflags from the Stack

	int	ENTRY_HOLD_OLD_I15H			; Call the Old Int-15H
	cli						; Disable Interrupts until we are leaving the Handler

	call int_handler_tools.set_caller_flags 	; Call a support routine to set the Caller's Flags on the Stack

	teardown_frame_pointer
	iret

hi15h.handle_our_free:
	call int_handler_tools.recover_saved_eflags	; Call a support routine to Get our Saved Eflags from the Stack

	; Do the old handler to get the line values...
	int ENTRY_HOLD_OLD_I15H 			; Call the int handler that int15h old was saved to.
	cli						; Disable Interrupts until we are leaving the Handler
	jc	hi15h.finish

	pushfd			; save the flags momentarily

	; Subtract ACPI_RESERVE_SPACE_* from the free region
	sub	dword [es:di+E820Struct_ofs_LengthLow],ACPI_RESERVE_SPACE_LOW		; (?) sub the ACPI_RESERVE_SPACE_LOW
	sbb	dword [es:di+E820Struct_ofs_LengthHigh],ACPI_RESERVE_SPACE_HIGH 	; (?) sbb the ACPI_RESERVE_SPACE_HIGH
	
	mov	ebx,'RSVD'				; Set the 'cont-code' we give back to be our 'reserved' cont-code

	popfd						; Restore the flags from momentary save

	jmp hi15h.finish				; Since the old handler has been used, jump to cleanup routine

hi15h.handle_our_rsvd:

	pushfd						; Save Flags
	get_ivt_entry_to_reg	ebx,ENTRY_HOLD_E820_CCODE	; Get our E820 Continuation Code from the IVT entry where we saved it
	popfd						; Restore Flags

	call int_handler_tools.recover_saved_eflags	; Call a support routine to Get our Saved Eflags from the Stack

	int	ENTRY_HOLD_OLD_I15H			; Call the Int Handler that int15h old was saved to.
	cli						; Disable Interrupts until we are leaving the Handler
	jc	hi15h.finish
	
	pushfd						; Save the flags momentarily

	; -v- subtract ACPI_RESERVE_SPACE_* from the free region
	sub	dword [es:di+E820Struct_ofs_LengthLow],ACPI_RESERVE_SPACE_LOW	; (?) move the returned lengthlow into struct
	sbb	dword [es:di+E820Struct_ofs_LengthHigh],ACPI_RESERVE_SPACE_HIGH ; (?) move the returned lengthhigh into struct

	; -v- Convert the base addr to the end of the free region
	multipush	eax,ebx
	mov	ebx,[es:di+E820Struct_ofs_BaseAddrLow]	; (?) move the baseaddrlow  into ebx
	mov	eax,[es:di+E820Struct_ofs_BaseAddrHigh] ; (?) move the baseaddrhigh into eax
	add	ebx,[es:di+E820Struct_ofs_LengthLow]	; (?) add the new lengthlow to the baseaddrlow
	adc	eax,[es:di+E820Struct_ofs_LengthHigh]	; (?) adc the new lengthhigh to the baseaddrhigh
	mov	[es:di+E820Struct_ofs_BaseAddrLow],ebx	; and store the new baseaddrlow back to the buffer
	mov	[es:di+E820Struct_ofs_BaseAddrHigh],eax ; and store the new baseaddrhigh back to the buffer
	multipop	eax,ebx

	; Set the resvd length to ACPI_RESERVE_SPACE_LOW/_HIGH
	mov dword [es:di+E820Struct_ofs_LengthLow],ACPI_RESERVE_SPACE_LOW ; (?) move the returned lengthlow into [es:di+8]
	mov dword [es:di+E820Struct_ofs_LengthHigh],ACPI_RESERVE_SPACE_HIGH ; (?) move the returned lengthhigh into [es:di+12]

	; set the type of the memline to ACPI_RESERVE_TYPE
	mov dword [es:di+E820Struct_ofs_Type],ACPI_RESERVE_TYPE ; (?) adjust the type to be ACPI_RESERVE_TYPE

	popfd						; Restore the flags from momentary save

	jmp	hi15h.finish				; Since the old handler has been used, jump to cleanup routine

hi15h.finish:
	;call int_handler_tools.set_caller_flags         ; Call a support routine to set the Caller's Flags on the Stack
	push	ax					; Save EAX
	sti						; Re-Enable Interrupts
	pushf						; Save Flags as returned by Old Int-Handler
	pop	ax					; And put the Flags into EAX
	mov	[ss:bp+0x08],ax 			; Overwrite the caller's Flags
	pop	ax
	teardown_frame_pointer				; Undo Step 1's Setup-the-Frame-Pointer
	iret

;-----------------------------------------------------------------------
;endofp  "handler_int15h"
;-----------------------------------------------------------------------
; -^- handler_int15h:
;-----------------------------------------------------------------------

;-----------------------------------------------------------------------
; -v- int_handler_tools.recover_saved_eflags:
;-----------------------------------------------------------------------
;startp  "int_handler_tools.recover_saved_eflags"
;-----------------------------------------------------------------------
int_handler_tools.recover_saved_eflags:
	;-v- recover saved eflags
	push eax					; Save EAX Contents
	mov eax,[ss:bp-0x04]				; Get EFLAGS into EAX from [ss:ebp-0x04]
	push eax					; Push EAX (holding the Saved Eflags) onto the stack
	popfd						; and then load EFlags by Popping off the Stack
	pop eax 					; Restore EAX Contents
	;-^- recover saved eflags
	retn
;-----------------------------------------------------------------------
;endofp  "int_handler_tools.recover_saved_eflags"
;-----------------------------------------------------------------------
; -^- int_handler_tools.recover_saved_eflags:
;-----------------------------------------------------------------------

;-----------------------------------------------------------------------
; -v- int_handler_tools.set_caller_flags:
;-----------------------------------------------------------------------
;startp  "int_handler_tools.set_caller_flags"
;-----------------------------------------------------------------------
int_handler_tools.set_caller_flags:
	;-v- set flags for return-to-caller
	push	ax					; Save EAX
	sti						; Re-Enable Interrupts
	pushf						; Save Flags as returned by Old Int-Handler
	pop	ax					; And put the Flags into EAX
	mov	[ss:bp+0x08],ax 			; Overwrite the caller's Flags
	pop	ax					; And Restore EAX
	;-^- set flags for return-to-caller
	retn
;-----------------------------------------------------------------------
;endofp  "int_handler_tools.set_caller_flags"
;-----------------------------------------------------------------------
; -^- int_handler_tools.set_caller_flags:
;-----------------------------------------------------------------------
hi15h.end:
;-----------------------------------------------------------------------
; -^-  Int15H-related Functions and Macros
;-----------------------------------------------------------------------
;#eof - ws_ihdlr.inc
