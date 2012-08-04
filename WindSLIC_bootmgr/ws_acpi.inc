
;-----------------------------------------------------------------------
URM_scan_ebda_for_zeroes_area:
;-----------------------------------------------------------------------
;input   - esi points to RSDP pointer table
;purpose - attempts to locate an area in the EBDA filled with zeroes
;returns - CF set on error, CF clear on success, EDI set to target on success
;-----------------------------------------------------------------------
;startp  "URM_scan_ebda_for_zeroes_area:"
;-----------------------------------------------------------------------

	multipush	ebx,ecx,edx,esi
	;rsdp must be on an offset divisible by 16d(0x10)

	cmp  ebx,0
	je  Usefza.deep_scan

Usefza.ebda_scan:

	; get the EBDA if it exists
	call	URM_get_ebda_address_into_esi
	jc	Usefza.no_ebda_found
	mov	ebx,esi
	add	ebx,EBDA_SCAN_OFFSET
	jmp	Usefza.start

	;-v- At this point -v-
	;ESI - contains the EBDA address
	;-^- At this point -^-

Usefza.deep_scan:

	mov	 ebx, 800h

Usefza.start:

	call	 URM_get_ebda_end_into_edx
	mov	 eax,ecx

	;-v- At this point -v-
	;ESI - contains the EBDA address
	;EBX - contains the address of the start of the area to check
	;EDX - contains the address of the end of the area to check
	;-^- At this point -^-

Usefza.begin_outer_loop:

	mov	ecx, eax			; Set ECX to the count for the inner loop
	mov	esi, ebx			; Set ESI to the current EBX chunk addr to check

	;-v- At this point -v-
	;ESI - contains the current EBX address to check
	;EBX - contains the current address for a 16-byte chunk to check (incremented each outer loop)
	;ECX - contains the table length # of bytes to check (36-bytes as of this writing)
	;EDX - contains the address of the end of the area to check
	;-^- At this point -^-

Usefza.begin_inner_loop:

	cmp	byte [esi],00h			; Compare [ESI] to 0x00
	jnz	Usefza.nonzerodetected		; If not zero, jump to nonzero_detected
	mov	byte [esi],0FFh 		; test if writable
	cmp	byte [esi],0FFh 		; check to see if esi is now FFh
	jne	Usefza.notwritable		; if cant write to it jump to notwritable
	mov	byte [esi],00h			; area is writable so undo our write
	inc	esi				; Increment esi to check next byte
	loop	Usefza.begin_inner_loop

Usefza.zeroregiondetected:
	; If we get here, EBX should have the start location for a zero-region
	mov	edi,ebx 			; Load EDI with the zero region in EBX
	jmp	Usefza.finish_ok

Usefza.notwritable:
Usefza.nonzerodetected:

	cmp	ebx,edx 			; Check if we are past the end
	ja	Usefza.no_zero_regions_found	; If > , then give up
	add	ebx,0x10			; If not, add 16d (0x10) to EBX 
	jmp	Usefza.begin_outer_loop 	; and restart the outer loop


Usefza.no_ebda_found:
Usefza.no_zero_regions_found:
Usefza.finish_notok:

	stc					; Set carry flag to indicate Error on retn                                                ; EDI will contain an arbitrary non-usable address!
	jmp	Usefza.done

Usefza.finish_ok:

	clc					; Clear Carry Flag to indicate Success                                                ; EDI will contain the zeroes-region address to use
	jmp	Usefza.done

Usefza.done:

	multipop	ebx,ecx,edx,esi
	retn
;-----------------------------------------------------------------------
;;endofp  "URM_scan_ebda_for_zeroes_area:"
;-----------------------------------------------------------------------

;-----------------------------------------------------------------------
URM_copy_rsdp_into_ebda: 
;-----------------------------------------------------------------------
;input - esi points to RSDP pointer table
;purpose - given the inputs, copies the bytes of RSDP to [edi]
;returns - CF set on error, CF clear on success
;-----------------------------------------------------------------------
;startp  "URM_copy_rsdp_into_ebda:"
;-----------------------------------------------------------------------

; the RSDP has changed from ACPI 1.0 to 2.0+
; in the ACPI 1.0b spec:
;       - length is: 20d(14h) bytes long
;       - revision byte: offset 15d(fh) is 0
;       - checksum byte: offset 8d(8h)
;       - no length byte!
; in the ACPI 4.0  spec:
;       - has length-dword and *two* checksums
;       - length is: 36d(24h) bytes long
;       - revision byte: offset 15d(fh) is 2
;       - length dword: offset 20d(14h) ; little-endian?
;       - checksum byte[of first 20d(14h) bytes: offset 8d(8h)
;       - extd checksum byte: offset 32d(20h)
;       - extended-checksum byte: offset 32d(20h)

;ertr_offset = 0x200    ; start at offset 0x200 (byte 512)
;ertr_table_len    = 0x24       ; expect 36d(0x24) bytes

	;Note: string instructions assume src:ds:si -> dst:es:di

	multipush ebx,ecx,edx,edi,esi,ebp

	mov edx,esi			;set edx to esi so we can save the RSDP pointer



	cmp byte [esi+RSDP_ofs_Revision],0x2		; check if Revision is 2
	jae Ucrie.has_lengthdword			; if revision is 2 or higher, get length from table

Ucrie.has_no_lengthdword:

	mov ecx,24h					      ; otherwise, length is assumed to be 36d(24h) just in case

	jmp Ucrie.length_decided

Ucrie.has_lengthdword:


	mov ecx,[esi+RSDP_ofs_Length]		; get length dword into ECX
	jmp Ucrie.length_decided

Ucrie.length_decided:



	cmp ecx,ertr_table_len	; we only reserved 0x24-bytes so fail if RSDP is larger
	ja Ucrie.rsdp_wont_fit
	
	;edx now has RSDP pointer, and ecx now has the RSDP table length

;-v-
Ucrie.check_if_rsdp_already_in_ebda:



	push esi				; save the RSDP pointer as it was
	call URM_locate_rsdp_table_in_ebda	; Double check if the RSDP is already IN the EBDA
	pop  esi				; and restore the RSDP pointer as it was
	jc Ucrie.determine_new_rsdp_loc 	; if CF set, then jump to new-location-finder

Ucrie.rsdp_already_in_ebda:			; Otherwise, maintain current RSDP


	lea edi,[esi+ecx]			; Set up EDI to report the End-of-RSDP Address
						; since we report it later on in BS_DETAIL mode

	jnc Ucrie.rsdp_go_on
;-^-
Ucrie.determine_new_rsdp_loc:


	push ebx
	mov  ebx,1
	call URM_scan_ebda_for_zeroes_area	; Call the EBDA scanner to find a valid region to use
	pop  ebx
	jc Ucrie.failed_to_get_ebda_addr	;  if CF set, jump to fail
						;  otherwise, EDI should contain retd addr for EBDA zeroes-region

	cmp byte [esi+RSDP_ofs_Revision],0x2		; check if Revision is 2
	jae Ucrie.ready_to_copy_rsdp			; if revision is < 2 or higher change length to 14h
	mov ecx,14h

Ucrie.ready_to_copy_rsdp:



	rep movs byte [edi],byte [esi]	; and move the table

	mov esi,edx			; set esi back to RSDP location
	jmp Ucrie.rsdp_go_on

Ucrie.failed_to_get_ebda_addr:

	stc
	jmp Ucrie.rsdp_done

Ucrie.rsdp_wont_fit:

	stc
	jmp Ucrie.rsdp_done

Ucrie.rsdp_go_on:

	clc
	jmp Ucrie.rsdp_done

Ucrie.rsdp_done:

	multipop  ebx,ecx,edx,edi,esi,ebp
	retn
;-----------------------------------------------------------------------
;;endofp  "URM_copy_rsdp_into_ebda:"
;-----------------------------------------------------------------------

;-----------------------------------------------------------------------
URM_copy_rsdt_into_rsvd_space: 
;-----------------------------------------------------------------------
;input - esi points to RSDP pointer table
;purpose - given the inputs, copies the bytes of RSDT to [edi]
;        - and patches the RSDP to point to it.
	;Note: string instructions assume src:ds:si -> dst:es:di
;-----------------------------------------------------------------------
;startp  "URM_copy_rsdt_into_rsvd_space:"
;-----------------------------------------------------------------------

	multipush	ebx,ecx,edx,edi,esi,ebp



	mov	edx,esi 		;set edx to esi so we can save the RSDP pointer

	mov	esi,[esi+RSDP_ofs_RSDTAddress]		;change esi to point to rsdt address
	mov	ecx,[esi+RSDT_ofs_Length]		;get length of rsdt into ecx
	cmp	ecx,rsdt_table_resvd_len		; check if RSDT is larger than we reserved for
	ja	Ucrirs.rsdt_wont_fit			; and jmp to fail if it is larger

	;esi now has RSDT pointer, and ecx now has the RSDT table length

	push	eax

	get_ivt_entry_to_reg eax,ENTRY_HOLD_E820_RSVTN	;get resvd base into EAX
	add	eax,rsdt_table_resvd_offset
	mov	edi,eax 	; set destination edi to the new location 
	mov	ebx,eax

	pop	eax



	rep	movs byte [edi],byte [esi]	; and move the table
	mov	ecx, 4
	xor	eax, eax
	rep	stos byte [edi]
	mov	esi,edx 			; set esi back to RSDP location
	mov	[esi+RSDP_ofs_RSDTAddress],ebx	; and set the location within the RSDP
	cmp	ebx,[esi+RSDP_ofs_RSDTAddress]	; compare the just-set location
	jne	Ucrirs.rsdt_pointer_update_fail ; if it isn't the same, jump to failure
	;call    URM_chksumfix_rsdp_input_esi    ; fix the rsdp checksum
	jmp	Ucrirs.rsdt_go_on

Ucrirs.rsdt_wont_fit:
	stc
	jmp	Ucrirs.rsdt_done
Ucrirs.rsdt_pointer_update_fail:
	stc
	jmp	Ucrirs.rsdt_done
Ucrirs.rsdt_go_on:
	clc
	jmp	Ucrirs.rsdt_done
Ucrirs.rsdt_done:
	multipop	ebx,ecx,edx,edi,esi,ebp
	retn
;----------------------------------------------------------------------
;;endofp  "URM_copy_rsdt_into_rsvd_space:"
;-----------------------------------------------------------------------

;-----------------------------------------------------------------------
URM_copy_xsdt_into_rsvd_space: 
;-----------------------------------------------------------------------
;input - esi points to RSDP pointer table
;purpose - given the inputs, copies the bytes of XSDT to [edi]
;        - and patches the RSDP to point to it.
	;Note: string instructions assume src:ds:si -> dst:es:di
;-----------------------------------------------------------------------
;startp  "URM_copy_xsdt_into_rsvd_space:"
;-----------------------------------------------------------------------


	multipush	ebx,ecx,edx,edi,esi,ebp

	mov	edx,esi 				; Set EDX to ESI so we can save the RSDP pointer

	cmp byte [esi+RSDP_ofs_Revision],0x02		; Compare RSDP revision
	jb Ucxirs.old_rsdp_table

	cmp dword [esi+RSDP_ofs_XSDTAddress],0x00000000 	;compare XSDT Pointer
	je Ucxirs.null_xsdt_pointer

	mov esi,[esi+RSDP_ofs_XSDTAddress]	;change esi to point to xsdt address [RSDP+0x18]

	cmp dword [esi],XSDT_val_Signature	; compare against 'XSDT' (which is 84688388h)
	jne Ucxirs.no_xsdt_ident		; and if no match, fail


	mov	ecx,[esi+XSDT_ofs_Length]		; Get length of xsdt into ecx
	cmp	ecx,xsdt_table_resvd_len		; Check if XSDT is larger than we reserved for
	ja	Ucxirs.xsdt_wont_fit			; and jmp to fail if it is larger

	;ESI now has RSDT pointer, and ECX now has the RSDT table length

	push	eax

	get_ivt_entry_to_reg	eax,ENTRY_HOLD_E820_RSVTN	;get resvd base into EAX
	add	eax,xsdt_table_resvd_offset
	mov	edi,eax 	; set destination edi to the new location 
	mov	ebx,eax

	pop	eax

	rep	movs byte [edi],byte [esi]	; and move the table
	mov	ecx, 8
	xor	eax, eax
	rep	stos byte [edi]
	mov	esi,edx 			; set esi back to RSDP location
	mov	[esi+RSDP_ofs_XSDTAddress],ebx	; and set the location within the RSDP
	cmp	ebx,[esi+RSDP_ofs_XSDTAddress]	; compare the just-set location
	jne	Ucxirs.xsdt_pointer_update_fail ; if it isn't the same, jump to failure
	;call    URM_chksumfix_rsdp_input_esi    ; fix the rsdp checksum
	jmp	Ucxirs.xsdt_go_on

Ucxirs.no_xsdt_ident:

	jmp	Ucxirs.finish_fail

Ucxirs.xsdt_wont_fit:

	jmp	Ucxirs.finish_fail

Ucxirs.xsdt_pointer_update_fail:

	jmp	Ucxirs.finish_fail

Ucxirs.old_rsdp_table:

	jmp	Ucxirs.finish_fail

Ucxirs.null_xsdt_pointer:

	jmp	Ucxirs.finish_fail

Ucxirs.finish_fail:
	stc
	jmp	Ucxirs.xsdt_done

Ucxirs.xsdt_go_on:

	clc
	jmp	Ucxirs.xsdt_done

Ucxirs.xsdt_done:

	multipop	ebx,ecx,edx,edi,esi,ebp
	retn
;-----------------------------------------------------------------------
;;endofp  "URM_copy_xsdt_into_rsvd_space:"
;-----------------------------------------------------------------------

;-----------------------------------------------------------------------
URM_chksumfix_rsdp_input_esi:
;-----------------------------------------------------------------------
;startp  "URM_chksumfix_rsdp_input_esi:"
;-----------------------------------------------------------------------
; the RSDP has changed from ACPI 1.0 to 2.0+
; in the ACPI 1.0b spec:
;       - length is: 20d(14h) bytes long
;       - revision byte: offset 15d(fh) is 0
;       - checksum byte: offset 8d(8h)
;       - no length byte!
; in the ACPI 4.0  spec:
;       - length is: 36d(24h) bytes long
;       - revision byte: offset 15d(fh) is 2
;       - has length-dword and *two* checksums
;       - checksum byte[of first 20d(14h) bytes: offset 8d(8h)
;       - extd checksum byte: offset 32d(20h)
;       - extended-checksum byte: offset 32d(20h)

	pushfd
	multipush	eax,ecx
	xor	eax,eax
	mov	[esi+RSDP_ofs_Checksum],ah	;Clear the Checksum before recalcing it
	mov	ecx,14h 		       ;RSDP(v1.0) table is 20 (0x14) bytes long
	push	esi
Ucsfrie.loop_checksum_rsdp:
	mov	al,[esi]
	inc	esi
	add	ah,al
	loop	Ucsfrie.loop_checksum_rsdp
	neg	ah
	pop	esi

	mov	[esi+RSDP_ofs_Checksum],ah	; Store the Checksum

Ucsfrie.extd_checksum_v20rsdp:
	xor	ecx,ecx
	mov	cl,[esi+RSDP_ofs_Revision]	; Get the revision byte
	cmp	cl,2				; If revbyte < 2
	jl	Ucsfrie.checksumming_complete	; then jump to checksumming-complete
	mov	dword ecx,[esi+RSDP_ofs_Length] ; If revbyte was 2, then get 'lengthdword' (0x14)

	; Clear the extd checksum bytes so they don't get calced.
	xor	eax,eax
	mov	[esi+RSDP_ofs_ExtdChecksum],ah	;Clear the Extd Checksum before recalcing it

	push	esi
Ucsfrie.loop_extdchecksum_rsdp:
	mov	al,[esi]
	inc	esi
	add	ah,al
	loop	Ucsfrie.loop_extdchecksum_rsdp
	neg	ah
	pop	esi
	mov	[esi+RSDP_ofs_ExtdChecksum],ah	;Store the Extd Checksum

Ucsfrie.checksumming_complete:
	multipop	eax,ecx
	popfd
	retn
;-----------------------------------------------------------------------
;;endofp  "URM_chksumfix_rsdp_input_esi:"
;-----------------------------------------------------------------------

;-----------------------------------------------------------------------
URM_copy_slic_table_to_resvd_memory:
;-----------------------------------------------------------------------
;   returns : EDI - pointer to new slic address
;-----------------------------------------------------------------------
;startp  "URM_copy_slic_table_to_resvd_memory:"
;-----------------------------------------------------------------------
	multipush	ecx,edx,esi

	; Load ESI with source SLIC address
	xor	edx,edx
	mov	edx,cs					; Put CS into EDX
	shl	edx,4					; Shift EDX left 4 places (mult-by-16)
	add	edx,SLIC.addr_includedat		; Add SLIC offset to EDX to get linear src address
	mov	esi,edx 				; Set source esi to the old location

	call decrypt_slic


	mov ecx,[cs:SLIC.addr_includedat+SLIC_ofs_Length]	; Get the length of the SLIC into ECX


	; Load EDI with destination SLIC address
	get_ivt_entry_to_reg	edx,ENTRY_HOLD_E820_RSVTN	; Get resvd base into EDX


	add	edx,slic_table_resvd_offset		; Then add the offset for the appropriate table

	mov	edi,edx 				; Set destination edi to the new location

	multipush	esi,edi
	rep	movs byte [edi],[esi]
	multipop	esi,edi
	multipop	ecx,edx,esi
	retn
;-----------------------------------------------------------------------
;;endofp  "URM_copy_slic_table_to_resvd_memory:"
;-----------------------------------------------------------------------

;-----------------------------------------------------------------------
; -v- URM_patch_rsdt_for_slic:
;-----------------------------------------------------------------------
; at entry : ESI is RSDP pointer
; at entry : EDI is SLIC pointer
;-----------------------------------------------------------------------
URM_patch_rsdt_for_slic:
;-----------------------------------------------------------------------
;startp  "URM_patch_rsdt_for_slic:"
;-----------------------------------------------------------------------
	multipush	eax,ebx,ecx,edx,esi

	xor	eax,eax 			; Set EAX,EBX,ECX,EDX to zero
	xor	ebx,ebx
	xor	ecx,ecx 		
	xor	edx,edx

	push	edi				; save SLIC pointer to stack
	mov	edi,esi 			; move RSDP to EDI, will need this if table move is required

	mov	esi,[esi+RSDP_ofs_RSDTAddress]	; Change ESI to point to rsdt address [RSDP+0x10]
	lea	ebx,[esi+RSDT_ofs_EntryTable]	; Set EBX to offset of first table 'entry'
	mov	ecx,[esi+RSDT_ofs_Length]	; Get length of rsdt into ECX

	sub	ecx,24h 			; Subtract table header size from total length to check
	shr	ecx,2				; Shift ECX right 2 places to simulate "divide by 4"

Uprfs.patch_rsdt_entry_loop:

	mov	edx,[ebx]
	cmp	dword [edx],SLIC_val_Signature	; Compare table entry vs 'SLIC' (slic label)
	je	Uprfs.patch_rsdt_slic_found	; If SLIC found, jmp to that handler
	add	ebx,RSDT_len_EntryTableEntry	; Otherwise, add 4 to EBX to point to next entry
	loop	Uprfs.patch_rsdt_entry_loop	; and start the loop again
	jmp	Uprfs.patch_rsdt_append 	; If the loop gives up, assume append will be necessary.

Uprfs.patch_rsdt_append:			; Append 4 to the length of RSDT

	mov	eax,[esi+RSDT_ofs_Length]	; by getting current length in EAX
	add	eax,RSDT_len_EntryTableEntry	; and adding 4
	mov	[esi+RSDT_ofs_Length],eax	; then storing back to [ESI+0x04]
	jmp	Uprfs.patch_rsdt_ready		; Finally, jump to the actual patch process

Uprfs.patch_rsdt_slic_found:

	cmp	 byte [edi+15], 02h
	jge	 post_table_move_rsdt
	call	 clear_old_slic
	jmp	 post_table_move_rsdt

Uprfs.patch_rsdt_ready:

	cmp	dword [ebx],0 ; test if there are 4 00h behind RSDT
	jne	Uprfs.patch_rsdt_for_slic_no_space

post_table_move_rsdt:

	pop	edi	    ; pop SLIC pointer to EDI

	; If we get here, EBX is the offset that needs to be patched
	; and EDI is the value it should be patched to.

	mov	[ebx],edi

post_table_move_rsdt2:

	push	ebx
	mov	ebx,1
	call	URM_copy_slic_oemname_to_esi
	pop	ebx
	call	URM_fix_acpi_table_chksum_at_esi
	jmp	Uprfs.patch_rsdt_for_slic_end

Uprfs.patch_rsdt_for_slic_no_space:

	call move_table_rsdt
	jc  post_table_move_rsdt
	pop edi 		   ; pop SLIC pointer to EDI
	jmp post_table_move_rsdt2

Uprfs.patch_rsdt_for_slic_end:

	multipop	eax,ebx,ecx,edx,esi
	retn
;-----------------------------------------------------------------------
;;endofp  "URM_patch_rsdt_for_slic:"
;-----------------------------------------------------------------------
; -^- URM_patch_rsdt_for_slic:
;-----------------------------------------------------------------------

;-----------------------------------------------------------------------
; -v- URM_patch_xsdt_for_slic:
;-----------------------------------------------------------------------
URM_patch_xsdt_for_slic:
;-----------------------------------------------------------------------
; at entry : ESI is RSDP pointer
; at entry : EDI is SLIC pointer
;-----------------------------------------------------------------------
;startp  "URM_patch_xsdt_for_slic:"
;-----------------------------------------------------------------------
	multipush	eax,ebx,ecx,edx,esi


	cmp	byte [esi+RSDP_ofs_Revision],0x02		; Compare RSDP revision
	jb	Upxfs.pxfs_old_rsdp_table

	cmp	dword [esi+RSDP_ofs_XSDTAddress],0x00000000	; Compare XSDT Pointer
	je	Upxfs.pxfs_null_xsdt_pointer

	xor	eax,eax 					; Set eax,ebx,ecx,edx to zero
	xor	ebx,ebx
	xor	ecx,ecx 		
	xor	edx,edx

	mov	esi,[esi+RSDP_ofs_XSDTAddress]		; Change esi to point to xsdt address [RSDP+0x18]
	lea	ebx,[esi+XSDT_ofs_EntryTable]		; Set EBX to offset of first table 'entry'
	mov	ecx,[esi+XSDT_ofs_Length]		; Get length of xsdt into ECX

	sub	ecx,24h 				; Subtract table header size 36d/0x24 from total length to check
	shr	ecx,3					; Shift ECX right 3 places to simulate "divide by 8"

Upxfs.patch_xsdt_entry_loop:

	mov	edx,[ebx]
	cmp	dword [edx],SLIC_val_Signature		; Compare table entry vs 'SLIC' (slic label)
	je	Upxfs.patch_xsdt_slic_found		; If SLIC found, jmp to that handler
	add	ebx,XSDT_len_EntryTableEntry		; Otherwise, add 8 to EBX to point to next entry
	loop	Upxfs.patch_xsdt_entry_loop		; and start the loop again
	jmp	Upxfs.patch_xsdt_append 		; If the loop gives up, assume append will be necessary.

Upxfs.patch_xsdt_append:					; Append 8 to the length of XSDT

	mov	eax,[esi+XSDT_ofs_Length]			; by getting current length in EAX
	add	eax,XSDT_len_EntryTableEntry			; and adding 8
	mov	[esi+XSDT_ofs_Length],eax			; then storing back to [ESI+0x04]
	jmp	Upxfs.patch_xsdt_ready				; Finally, jmp to the actual patch process

Upxfs.patch_xsdt_slic_found:

	call	 clear_old_slic
	jmp	 post_table_move_xsdt

Upxfs.patch_xsdt_ready:

	cmp	dword [ebx], 0
	jne	Upxfs.patch_xsdt_for_slic_no_space
	cmp	dword [ebx+4], 0
	jne	Upxfs.patch_xsdt_for_slic_no_space

post_table_move_xsdt:

	; If we get here, ebx is the offset that needs to be patched
	; and EDI is the value it should be patched to.

	mov	[ebx],edi
	push	ebx
	mov	ebx,1
	call	URM_copy_slic_oemname_to_esi
	pop	ebx
	call	URM_fix_acpi_table_chksum_at_esi
	jmp	Upxfs.patch_xsdt_for_slic_end

Upxfs.pxfs_old_rsdp_table:

	stc
	jmp	Upxfs.patch_xsdt_for_slic_end

Upxfs.pxfs_null_xsdt_pointer:

	stc
	jmp	Upxfs.patch_xsdt_for_slic_end

Upxfs.patch_xsdt_for_slic_no_space:

	call move_table_xsdt
	jmp post_table_move_xsdt

Upxfs.patch_xsdt_for_slic_end:

	multipop	eax,ebx,ecx,edx,esi
	retn

;-----------------------------------------------------------------------
;;endofp  "URM_patch_xsdt_for_slic:"
;-----------------------------------------------------------------------
; -^- URM_patch_xsdt_for_slic:
;-----------------------------------------------------------------------

;-----------------------------------------------------------------------
URM_copy_slic_oemname_to_esi:
;-----------------------------------------------------------------------
;startp  "URM_copy_slic_oemname_to_esi:"
;-----------------------------------------------------------------------
	multipush	eax,esi,edi

	mov	edi,esi
	xor	ecx,ecx
	mov	esi,SLIC.addr_includedat+SLIC_ofs_OEMID ; Mov ESI SLIC+0Ah
	cmp	ebx,0
	je	update_rsdp

update_table:

	add	edi,SLIC_ofs_OEMID			; Add EDI, 0Ah
	mov	cx,SLIC_len_OEMID+SLIC_len_OEMTableID	; Mov CS, 0Eh
	jmp	start_update

update_rsdp:

	add	edi,RSDP_ofs_OEMID
	mov	cx, RSDP_len_OEMID

start_update:

	rep	movs byte [edi],byte [cs:esi]

	multipop	eax,esi,edi
	retn
;-----------------------------------------------------------------------
;;endofp  "URM_copy_slic_oemname_to_esi:"
;-----------------------------------------------------------------------

;-----------------------------------------------------------------------
URM_fix_acpi_table_chksum_at_esi:
;-----------------------------------------------------------------------
;startp  "URM_fix_acpi_table_chksum_at_esi:"
;-----------------------------------------------------------------------
	push	eax
	xor	ax,ax
	mov	[esi+ACPIGenericTable_ofs_Checksum],ah	; Clear the Checksum
	mov	ecx,[esi+ACPIGenericTable_ofs_Length]	; Get Table Length
	push	esi
loop_checksum:
	mov	al,[esi]
	inc	esi
	add	ah,al
	loop	loop_checksum
	neg	ah
	pop	esi
	mov	[esi+ACPIGenericTable_ofs_Checksum],ah
	pop	eax
	retn
;-----------------------------------------------------------------------
;;endofp  "URM_fix_acpi_table_chksum_at_esi:"
;-----------------------------------------------------------------------

;-----------------------------------------------------------------------
URM_locate_rsdp_table:
;-----------------------------------------------------------------------
; Purpose       : Locates the "RSD PTR " RSDP Table in either
;               : EBDA or BIOS Memory Segments (0xE000-0xFFFE)
;-----------------------------------------------------------------------
;   at exit     : CF set if unable to locate the RSDP table
;               : -or-
;               : CF clear if able to locate the RSDP table
;               : and ESI is the linear address of the RSDP table
;-----------------------------------------------------------------------
;startp  "URM_locate_rsdp_table:"
;-----------------------------------------------------------------------
Ulrt.search_ebda:
	call URM_locate_rsdp_table_in_ebda
	jnc Ulrt.save_addr
Ulrt.search_bios_seg:
	call URM_locate_rsdp_table_in_bios_seg
	jnc Ulrt.save_addr
	jmp Ulrt.failure
Ulrt.save_addr:
	jmp Ulrt.done
Ulrt.failure:
Ulrt.done:
	retn
;-----------------------------------------------------------------------
;;endofp  "URM_locate_rsdp_table:"
;-----------------------------------------------------------------------

;-----------------------------------------------------------------------
URM_locate_rsdp_table_in_ebda:
;-----------------------------------------------------------------------
; Purpose       : Locates the "RSD PTR " RSDP Table in EBDA
;-----------------------------------------------------------------------
;   at exit     : CF set if unable to locate the RSDP table
;               : -or-
;               : CF clear if able to locate the RSDP table
;               : and ESI is the linear address of the RSDP table
;-----------------------------------------------------------------------
;startp  "URM_locate_rsdp_table_in_ebda:"
;-----------------------------------------------------------------------
	multipush	eax,ebx,ecx,edx

	; Get the EBDA if it exists
	call	URM_get_ebda_address_into_esi
	jc	Ulrtie.no_ebda_found

	; Set up to check 1K of EBDA
	mov	ebx,esi 			; Move ESI ebda address into EBX
	add	ebx,0x400			; Add 1K (0x400) to the address
Ulrtie.check_for_rsd_str:
	cmp	dword [esi],20445352h		; Search "RSD "
	jz	short Ulrtie.check_for_ptr_str	; If = , jmp to   cmp  "RSD " + 4 = "PTR "
Ulrtie.check_next_addr_block:
	add	esi,10h 			; Add 16-bytes to linear address in ESI
	cmp	esi,ebx 			; Check if we are past the first 1K
	jnae	Ulrtie.check_for_rsd_str	; If < , jmp  to search "RSD "
	stc					; Set the carry flag
	jmp	Ulrtie.no_rsdp_found
Ulrtie.check_for_ptr_str:
	cmp	dword [esi+4h],20525450h	; Cmp  "RSD " + 4 = "PTR "
	jz	short Ulrtie.matched_whole_str	; If = , jmp
	jmp	Ulrtie.check_next_addr_block
Ulrtie.matched_whole_str:
	clc					; clear the carry flag
	jmp	Ulrtie.done
Ulrtie.no_ebda_found:
Ulrtie.no_rsdp_found:
	stc
	jmp	Ulrtie.done
Ulrtie.done:
	multipop	eax,ebx,ecx,edx
	retn
;-----------------------------------------------------------------------
;startp  "URM_locate_rsdp_table_in_ebda:"
;-----------------------------------------------------------------------

;-----------------------------------------------------------------------
URM_locate_rsdp_table_in_bios_seg:
;-----------------------------------------------------------------------
; Purpose       : Locates the "RSD PTR " RSDP Table in upper memory
;               : Segments (from e000h-fffeh)
;-----------------------------------------------------------------------
;   at exit     : CF set if unable to locate the RSDP table
;               : -or-
;               : CF clear if able to locate the RSDP table
;               : and ESI is the linear address of the RSDP table
;-----------------------------------------------------------------------
;startp  "URM_locate_rsdp_table_in_bios_seg:"
;-----------------------------------------------------------------------
	multipush	eax,ebx,ecx,edx

	; Check the E000 - FFFE segment range
	mov	ebx,0e0000h			; Scan Start Address (0xE000:0x0000)
	mov	edx,0fffe0h			; Scan End   Address (0xFFFE:0x0000)
	clc
	mov	esi,edx 			; Start at the top, work downwards
Ulrtibs.check_for_rsd_str:
	cmp	dword [esi],20445352h		; Compare the start of the block to "RSD "
	jz	Ulrtibs.check_for_ptr_str	; If matched, jmp to "PTR " comparison
Ulrtibs.check_next_addr_block:
	sub	esi,10h
	cmp	esi,ebx
	jae	Ulrtibs.check_for_rsd_str	; If < , jmp  to search "RSD "
	stc					; Set the carry flag
	jmp	Ulrtibs.no_rsdp_found
Ulrtibs.check_for_ptr_str:
	cmp	dword [esi+4h],20525450h	; Cmp  "RSD " + 4 = "PTR "
	jz	Ulrtibs.matched_whole_str	; If = , jmp
	jmp	Ulrtibs.check_next_addr_block
Ulrtibs.matched_whole_str:
	clc					; Clear the carry flag
	jmp	Ulrtibs.done
Ulrtibs.no_rsdp_found:
	jmp	Ulrtibs.done
Ulrtibs.done:
	multipop	eax,ebx,ecx,edx
	retn
;-----------------------------------------------------------------------
;endofp  "URM_locate_rsdp_table_in_bios_seg:"
;-----------------------------------------------------------------------

;-----------------------------------------------------------------------
URM_get_ebda_address_into_esi:
;-----------------------------------------------------------------------
;startp  "URM_get_ebda_address_into_esi:"
;-----------------------------------------------------------------------
	multipush	eax

	xor	eax, eax
	mov	esi, 0x40e			; 0x400 is the linear address of
						; the start of the BIOS data area 
						; and 0x0e is the offset of the 
						; segment of the EBDA location
						; So, 0x40e is the linear address.
	mov	ax,[esi]			; Mov into AX the location of EBDA
	or	ax, ax				; Check if location is null/zero
	jz	Ugea.nonefound
	shl	eax,4				; Turn EAX segment into linear address
	mov	esi, eax
	clc
	jmp	Ugea.done

Ugea.nonefound:

	stc

Ugea.done:

	multipop	eax
	retn
;-----------------------------------------------------------------------
;endofp  "URM_get_ebda_address_into_esi:"
;-----------------------------------------------------------------------