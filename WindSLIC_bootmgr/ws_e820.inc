;----------------------------------------------------------------------------------------------------
; -v- Int15H,AX=E820H Reservation Procedures
;----------------------------------------------------------------------------------------------------

;-----------------------------------------------------------------------
get_highest_free_e820_line:
;-----------------------------------------------------------------------
;startp  "get_highest_free_e820_line:"
;-----------------------------------------------------------------------
; this function uses int15h,ax=e820h to find the highest available
; "free" memory region.
;-----------------------------------------------------------------------
; calls : - get_e820_line
; calls : - convert_e820_range_len_to_address
; calls : - check_for_new_e820_high_water_mark
;-----------------------------------------------------------------------
	pushad 
	multipush	ds
	push	word RESVD_DATA_SEG
	pop	ds

	stc					; Set CF initially, to assume failure. 
 
	mov	ebx,0x00000000			; Set Continuation Code to 0x00000000 for first get. 
ghe8.line_loop: 
	call	get_e820_line			; get the line data using int15h proc 
	jc	ghe8.failed			; if CF set, jump to failure report 
	or	ecx,ecx 			; Check to make sure we have returned bytes 
	jz	ghe8.failed_no_bytes_td 	; assume if we have no bytes, we are done. 
	 
	call	check_for_new_e820_high_wtr_mrk ; Check this e820 memrange for free/high
	or	ebx,ebx 			; compare e820 BX return val to zero 
	jnz	ghe8.line_loop			; if non-zero, another line exists, go again 
	jmp	ghe8.okend			; otherwise, go to ghe8.okend
	 
 
ghe8.failed_no_bytes_td:
ghe8.failed:
	stc 
	jmp ghe8.done 
ghe8.okend:
	clc
ghe8.done: 
	multipop	ds
	popad 
	retn
;-----------------------------------------------------------------------
;endofp  "get_highest_free_e820_line:"
;-----------------------------------------------------------------------

;-----------------------------------------------------------------------
; -v- get_e820_line:
;-----------------------------------------------------------------------
;       where : EBX is Continuation Code
;            :   -on first-run, must be 0x00000000.
;            :   -on subsequent runs, must be the continuation value
;            :   -returned by the last invocation of int21h,ax=e820h
;-----------------------------------------------------------------------
;       sets : ES:DI to a Buffer Pointer: e820_line_buffer
;            :   -pointer to a buffer of [ECX] bytes size
;            : ECX is the Buffer Size: E820Struct_len_Buffer
;            :   - the buffer should hold  at least 20d(14h) bytes
;            :     with more modern BIOSes returning 24d(18h) bytes 
;            :     so 24d(18h) is used in this program.
;            : EAX to Function Code (e820h)
;            : EDX to the Signature [will be provided by procedure]
;            :   - the Signature value that must be used with this
;            :     call is 'SMAP'. [Note: due to l.e., 'PAMS']
;-----------------------------------------------------------------------
;     returns : CF - Clear if OK, Set on Error
;            : EAX - Signature ('SMAP') [Note: due to l.e. 'PAMS']
;            : EBX - Continuation Code
;            :   -on non-last runs, should be a continuation value
;            :   -that needs to be passed to the next invocation
;            :   -of int15h/ax=e820h to get the next memory range.
;            :   -on last-run, should be 0x00000000.
;            : ECX is the Buffer Size Used
;            :   - the interrupt should return at least 20d(14h) bytes
;            :     with more modern BIOSes returning 24d(18h) bytes 
;            : ES:DI is the Buffer Pointer 
;            :   -ES and DI should be unchanged during this procedure
;-----------------------------------------------------------------------
;Purpose: Get an e820 memory block line using int21h,ax=e820h
;-----------------------------------------------------------------------
get_e820_line:
;-----------------------------------------------------------------------
;startp  "get_e820_line:"
;-----------------------------------------------------------------------
	multipush	ds,es

	push	word RESVD_DATA_SEG		; Set DS to our RESVD_DATA_SEG
	pop	ds
	push	ds				; Set ES=DS via push DS 
	pop	es				; and pop ES 

	mov	ecx,E820Struct_len_Buffer	; Set ECX to the size of the E820 Buffer
	mov	edx,'PAMS'			; 'SMAP'(or, in little-endian, 'PAMS')
						; is the Signature that int21h,ax=e820h expects.

	mov	eax,0x0000e820			; e820h is the Function code for this int21h function
	mov	di,E820Struct_ofs_Start 	; Set DI to our line buffer structure 

	mov	[ds:E820Struct_ofs_ContCodeForThis],ebx  ; Store the Continuation Code into the buffer

	int	15h

	jc	ge8l.failed_noget

	cmp	eax,edx 			; Check that the signature returned matches what we sent
	jne	ge8l.failed_no_sigmatch

	mov	[ds:E820Struct_ofs_ContCodeForNext],ebx 	; Store the Next Cont Code into the buffer

	call	convert_e820_range_len_to_address	; figure out the end-addr 

	jmp	ge8l.okend

ge8l.failed_no_sigmatch:
ge8l.failed_noget:
	stc
	jmp	ge8l.done
ge8l.okend:
	clc
ge8l.done:
	multipop	ds,es
	retn
;-----------------------------------------------------------------------
;endofp  "get_e820_line:"
;-----------------------------------------------------------------------

;-----------------------------------------------------------------------
; -v- convert_e820_range_len_to_address:
;-----------------------------------------------------------------------
; where : ES:DI is the Buffer Pointer 
;       :   -pointer to a buffer of [ECX] bytes size
;       : ECX is the Buffer Size
;       :   - the buffer should hold  at least 20d(14h) bytes
;       :     with more modern BIOSes returning 24d(18h) bytes 
;       :     so 24d(18h) is used in this program.
;-----------------------------------------------------------------------
convert_e820_range_len_to_address:
;-----------------------------------------------------------------------
;startp  "convert_e820_range_len_to_address:"
;-----------------------------------------------------------------------

	multipush	eax,ebx,ds

	push	word RESVD_DATA_SEG
	pop	ds

	mov	ebx,dword [ds:E820Struct_ofs_LengthLow]
	mov	eax,dword [ds:E820Struct_ofs_LengthHigh]
	add	ebx,dword [ds:E820Struct_ofs_BaseAddrLow]
	adc	eax,dword [ds:E820Struct_ofs_BaseAddrHigh]

	; OK, End Address Calculated. Now place these into the proper holding cells.
	mov	[ds:E820Struct_ofs_EndAddrLow],ebx
	mov	[ds:E820Struct_ofs_EndAddrHigh],eax

ce8rlta.done:
	multipop	eax,ebx,ds
	retn
;-----------------------------------------------------------------------
;endofp  "convert_e820_range_len_to_address:"
;-----------------------------------------------------------------------

;-----------------------------------------------------------------------
; -v- check_for_new_e820_high_wtr_mrk:
;-----------------------------------------------------------------------
; this function takes a buffer in ES:DI of size ECX and checks it for 
; free status.  If free, copies the buffer to the high-water struct.
;-----------------------------------------------------------------------
check_for_new_e820_high_wtr_mrk:
;-----------------------------------------------------------------------
;startp  "check_for_new_e820_high_wtr_mrk:"
;-----------------------------------------------------------------------

	pushf
	multipush	eax,ebx,ds

	;disp_str_crlf   txt_e820_hwm_chk,BS_DETAILED    ; "Checking for new e820 High-Water-Mark..."

	push	word RESVD_DATA_SEG
	pop	ds

	; Deny any range that isn't type 1 (usable)
	cmp	dword [ds:E820Struct_ofs_Type],0x00000001
	jne	cfne820hwm.skip

	; Deny any range that doesn't have at least our desired ACPI size
	cmp	dword [ds:E820Struct_ofs_LengthLow],ACPI_RESERVE_SPACE_LOW
	jb	cfne820hwm.skip

	; 64-bit workaround ... deny any range that is past 4GB
	cmp	dword [ds:E820Struct_ofs_EndAddrHigh],0x00000000
	ja	cfne820hwm.skip

	; Also deny any range that starts at 0x00000000
	;  since low memory is a minefield
	cmp	dword [ds:E820Struct_ofs_BaseAddrLow],0x00000000
	je	cfne820hwm.skip

	; Also deny any range that has a lengthlow of 0x00000000
	; Since it will have to be a 32-bit (4GB) crossing region
	cmp	dword [ds:E820Struct_ofs_LengthLow],0x00000000
	je	cfne820hwm.skip

	multipush	eax,ebx

	; -v- Store this memory range's continuation code to the resvd spot
	mov	ebx,[ds:E820Struct_ofs_ContCodeForThis]
	set_ivt_entry_from_reg	ENTRY_HOLD_E820_CCODE,ebx		;set cont code

	mov	ebx,[ds:E820Struct_ofs_EndAddrLow]				;get the line's end addr into EBX
	sub	ebx,ACPI_RESERVE_SPACE_LOW				;sub the ACPI_RESERVE_SPACE_LOW

	set_ivt_entry_from_reg	ENTRY_HOLD_E820_RSVTN,ebx		;save the resvd base into IVT
	set_ivt_entry_from_literal	ENTRY_HOLD_IH_ENABLE,0x01	; tell handler we've readied the e820 resv.


	multipop	eax,ebx

cfne820hwm.skip:

cfne820hwm.done:
	multipop	eax,ebx,ds
	popf
	retn
;-----------------------------------------------------------------------
;endofp  "check_for_new_e820_high_wtr_mrk:"
;-----------------------------------------------------------------------

;----------------------------------------------------------------------------------------------------
; -^- Int15H,AX=E820H Reservation Procedures
;----------------------------------------------------------------------------------------------------