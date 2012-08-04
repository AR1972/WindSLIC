
;====================================================================================================
move_handler:
;====================================================================================================
;move int15h handler to low memory
;----------------------------------------------------------------------------------------------------

	push	edi
	push	edx
	push	esi
	push	ecx
	push	eax

	xor	edx,edx
	mov	edx,cs
	shl	edx,4
	add	edx,handler_int15h
	mov	esi,edx

	add	edx,hi15h.end
	mov	ecx,edx
	sub	ecx,esi

	push	esi
	push	ebx
	xor	ebx,ebx
	call	URM_scan_ebda_for_zeroes_area
	pop	ebx
	pop	esi
	push	edi
	rep	movs byte [edi],byte [esi]
	pop	edi

;--------------------------------------------------------------------------------------------
; convert edi to segment:offset
;--------------------------------------------------------------------------------------------

	mov	eax,edi
	shr	eax,16
	shl	eax,16
	sub	edi,eax
	shr	eax,4
	push	word ax
	push	word di
	pop	dword edi
	set_ivt_entry_from_reg	ENTRY_HOLD_RSDP_ADDR,edi

	pop	eax
	pop	ecx
	pop	esi
	pop	edx
	pop	edi
	retn

;----------------------------------------------------------------------------------------------------
;||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
;====================================================================================================
cleanup_reserved:
;====================================================================================================
; clean junk out of our reserved memory
; EDI = start address
; ECX = lenght to write
;----------------------------------------------------------------------------------------------------

	pushad
	mov eax, 0FFh
	rep stos byte [edi]
	popad
	retn

;----------------------------------------------------------------------------------------------------
;||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
;====================================================================================================
rsdp_writable:
;====================================================================================================

       push	edi
       push	esi
       push	ecx

       xor     ecx,ecx
       mov     edi,[esi]
       cmp     edi,20445352h
       jne     rsdp_not_writable
       mov     al,1
       call    SetReadWriteMask
       mov     byte [esi],byte 00h
       cmp     byte [esi],byte 00h
       je      rsdp_is_writable

rsdp_not_writable:

       clc
       jmp     rdsp_writable_done

rsdp_is_writable:

       mov    [esi],edi
       stc

rdsp_writable_done:

	pop	ecx
	pop	esi
	pop	edi
	retn

;----------------------------------------------------------------------------------------------------
;||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
;====================================================================================================
URM_get_ebda_end_into_edx:
;====================================================================================================

	push	eax
	push	ebx
	push	ecx

	mov	ebx,esi
	mov	eax,[ebx]
	xor	edx,edx
	mov	ecx,400h
	mul	ecx
	mov	edx,eax
	add	edx,esi

	pop	ecx
	pop	ebx
	pop	eax

	retn

;----------------------------------------------------------------------------------------------------
;||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
;====================================================================================================
URM_update_rsdt_tables:
;====================================================================================================
; ESI = RSD PTR address
; compares RSD PTR OEMID with OEMID of ACPI tables, updates with SLIC OEMID+OEMTableID when required
; and updates checksum
;----------------------------------------------------------------------------------------------------

	push esi
	xor  eax,eax
	mov  edi,esi		   ; move ESI to EDI
	add  edi,RSDP_ofs_OEMID    ; add the OEMID offset to EDI
	mov  edi,[edi]		   ; EDI now has 4 bytes of the OEMID

	mov	esi,[esi+RSDP_ofs_RSDTAddress]	; change esi to point to rsdt address
	lea	ebx,[esi+RSDT_ofs_EntryTable]	; Set EBX to offset of first table 'entry'
	mov	ecx,[esi+RSDT_ofs_Length]	; Get length of rsdt into ECX

	sub	ecx,24h 			; Subtract table header size from total length to check
	shr	ecx,2				; Shift ECX right 2 places to simulate "divide by 4"

URM_update_rsdt_tables_loop:

	mov	edx,[ebx]
	cmp	dword [edx+ACPIGenericTable_ofs_OEMID],edi
	jne	URM_update_rsdt_tables_next
	mov	esi,[ebx]
	push	ebx
	push	ecx
	mov	ebx,1
	call	URM_copy_slic_oemname_to_esi
	call	URM_fix_acpi_table_chksum_at_esi
	pop	ecx
	pop	ebx
	mov	eax,1

URM_update_rsdt_tables_next:

	add    ebx,RSDT_len_EntryTableEntry    ; Otherwise, add 4 to EBX to point to next entry
	loop   URM_update_rsdt_tables_loop     ; and start the loop again

URM_update_rsdt_tables_done:

	pop	esi
	retn

;----------------------------------------------------------------------------------------------------
;||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
;====================================================================================================
URM_update_xsdt_tables:
;====================================================================================================
; ESI = RSD PTR address
; compares RSD PTR OEMID with OEMID of ACPI tables, updates with SLIC OEMID+OEMTableID when required
; and updates chechsum
;----------------------------------------------------------------------------------------------------

	push esi

	cmp byte [esi+RSDP_ofs_Revision],0x2		; check if Revision is 2
	jb URM_update_xsdt_tables_done			; if revision is 2 or higher, get length from table

	mov  edi,esi		   ; move ESI to EDI
	add  edi,RSDP_ofs_OEMID    ; add the OEMID offset to EDI
	mov  edi,[edi]		   ; EDI now has 4 bytes of the OEMID

	mov	esi,[esi+RSDP_ofs_XSDTAddress]	; change esi to point to xsdt address
	lea	ebx,[esi+XSDT_ofs_EntryTable]	; Set EBX to offset of first table 'entry'
	mov	ecx,[esi+XSDT_ofs_Length]	; Get length of rsdt into ECX

	sub	ecx,24h 			; Subtract table header size from total length to check
	shr	ecx,3				; Shift ECX right 2 places to simulate "divide by 4"

URM_update_xsdt_tables_loop:

	mov	edx,[ebx]
	cmp	dword [edx+ACPIGenericTable_ofs_OEMID],edi
	jne	URM_update_xsdt_tables_next
	mov	esi,[ebx]
	push	ebx
	push	ecx
	mov	ebx,1
	call	URM_copy_slic_oemname_to_esi
	call	URM_fix_acpi_table_chksum_at_esi
	pop	ecx
	pop	ebx

URM_update_xsdt_tables_next:

	add	ebx,XSDT_len_EntryTableEntry	; Otherwise, add 8 to EBX to point to next entry
	loop	URM_update_xsdt_tables_loop	; and start the loop again

URM_update_xsdt_tables_done:

	pop	esi
	retn

;----------------------------------------------------------------------------------------------------
;||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
;----------------------------------------------------------------------------------------------------
move_table_rsdt:
;----------------------------------------------------------------------------------------------------
; if we are here then there is not enough space to add a SLIC pointer
; so we will attempt to move the next table clear the old table from memory
; and update the RSDT with the table's new location.
;----------------------------------------------------------------------------------------------------
; ESI has RSDT address
; EDI has RSDP pointer
; EBX has the start of the next table
;----------------------------------------------------------------------------------------------------

	push esi
	push edi

	cmp	byte  [edi+RSDP_ofs_Revision], 02h  ; if an XSDT exists then patch the XSDT with a SLIC pointer
	jge	move_table_done_has_xsdt

	mov	ecx,[ebx+4]			    ; move table length to ECX

	get_ivt_entry_to_reg eax,ENTRY_HOLD_E820_RSVTN	;get resvd base into EAX
	add	eax,rsdt_table_resvd_offset
	mov	edi,eax

	push	esi
	mov	esi,ebx

	push	edi				; save the address we move the table to
	push	ecx				; save the length of the table

	rep	movs byte [edi],byte [esi]	; and move the table

	pop	ecx				; restore the table lenght
	mov	edi,ebx 			; move the moved table old address to edi
	call	cleanup_reserved		; clear the old table

	pop	edi				; restore the table destination address
	pop	esi				; restore the rsdt address

	lea	eax,[esi+RSDT_ofs_EntryTable]	; Set EAX to offset of first table 'entry'
	mov	ecx,[esi+RSDT_ofs_Length]	; Get length of rsdt into ECX
	sub	ecx,24h 			; Subtract table header size from total length to check
	shr	ecx,2				; Shift ECX right 2 places to simulate "divide by 4"

update_moved_table_rsdt:

	cmp	ebx, [eax]			; compare the old table location to entry in rsdt
	je	moved_table_patch_rsdt		; if they match patch RSDT with the moved table's location
	add	eax,RSDT_len_EntryTableEntry	; Otherwise, add 4 to EAX to point to next entry
	loop	update_moved_table_rsdt 	; loop back to the start

moved_table_patch_rsdt:

	mov [eax],edi
	stc
	jmp move_table_done_rsdt

move_table_done_has_xsdt:

	clc

move_table_done_rsdt:

	pop edi
	pop esi
	retn

;----------------------------------------------------------------------------------------------------
;||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
;----------------------------------------------------------------------------------------------------
move_table_xsdt:
;----------------------------------------------------------------------------------------------------
; if we are hear then there is not enough space to add a SLIC pointer
; so we will attempt to move the next table, clear the old table from memory
; and update the XSDT with the table's new location.
;----------------------------------------------------------------------------------------------------

	push esi
	push edi

	cmp	dword [ebx], 'RSDT'		 ; if the next table is the RSDT then we cant move it
	je	overwrite_pointer

	mov	ecx, [ebx+4]			 ; move table length to ECX

	get_ivt_entry_to_reg eax,ENTRY_HOLD_E820_RSVTN	;get resvd base into EAX
	add	eax, xsdt_table_resvd_offset
	mov	edi, eax

	push	esi
	mov	esi, ebx

	push	edi				; save the address we move the table to
	push	ecx				; save the length of the table

	rep	movs byte [edi], byte [esi]	 ; and move the table

	pop	ecx				; restore the table lenght
	mov	edi, ebx			 ; move the moved table old address to EDI
	call	cleanup_reserved		; clear the old table

	pop	edi				; restore the table destination address
	pop	esi				; restore the rsdt address

update_moved_table_xsdt:

	lea	eax, [esi+XSDT_ofs_EntryTable]	 ; Set EAX to offset of first table 'entry'
	mov	ecx, [esi+XSDT_ofs_Length]	 ; Get length of rsdt into ECX
	sub	ecx, 24h			 ; Subtract table header size from total length to check
	shr	ecx, 3				 ; Shift ECX right 2 places to simulate "divide by 4"

update_moved_table_xsdt_loop:

	cmp	ebx, [eax]			; compare the old table location to entry in rsdt
	je	moved_table_patch_xsdt		     ; if they match
	add	eax, XSDT_len_EntryTableEntry	 ; Otherwise, add 4 to EAX to point to next entry
	loop	update_moved_table_xsdt_loop

moved_table_patch_xsdt:

	mov [eax], edi
	jmp move_table_done_xsdt

;----------------------------------------------------------------------------------------------------
; if we are here then the next table is the RSDT and cant be moved so we will find an unimportant
; table and overwrite it's pointer the RSDT will still have a pointer to this table so it's OK
;----------------------------------------------------------------------------------------------------

overwrite_pointer:

	lea	ebx, [esi+XSDT_ofs_EntryTable]	 ; Set EAX to offset of first table 'entry'
	mov	ecx, [esi+XSDT_ofs_Length]	 ; Get length of rsdt into ECX
	sub	ecx, 24h			 ; Subtract table header size from total length to check
	shr	ecx, 3				 ; Shift ECX right 2 places to simulate "divide by 4"

overwrite_pointer_loop:

       mov	eax,[ebx]
       cmp	dword [eax], 'BOOT'
       je	overwrite_pointer_loop_done
       cmp	dword [eax], 'MCFG'
       je	overwrite_pointer_loop_done
       add	ebx, XSDT_len_EntryTableEntry
       loop	overwrite_pointer_loop

overwrite_pointer_loop_done:

move_table_done_xsdt:

	pop edi
	pop esi
	retn

;----------------------------------------------------------------------------------------------------
;||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
;----------------------------------------------------------------------------------------------------
clear_old_slic:
;----------------------------------------------------------------------------------------------------
; paranoia function - clears an existing SLIC from memory, so a memory scan wont find 2 SLIC's
;----------------------------------------------------------------------------------------------------

	push edi

	cmp	  dword [ebx], 0
	je	  clear_old_slic_done
	mov	  edi,[ebx]
	mov	  ecx,176h
	call	  cleanup_reserved

clear_old_slic_done:

	pop edi
	retn

;----------------------------------------------------------------------------------------------------
;||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
;----------------------------------------------------------------------------------------------------
decrypt_slic:
;----------------------------------------------------------------------------------------------------
; ESI has SLIC address
;----------------------------------------------------------------------------------------------------

	push	edi
	push	esi
	push	ecx

	xor	ebx,ebx 		  ; zero EBX

	mov	edi,cs
	shl	edi,4
	add	edi,enc_key		  ;

	push	edi			  ; save encryption key address to stack

	mov	edx,enc_key_len
	sub	edx,enc_key
	mov	ecx,edx

DecryptKey:

	xor	byte [edi], 0FFh
	inc	edi
	loop	DecryptKey

	pop	edi
	push	edi

	mov	ecx, 176h		  ; length to decrypt SLIC is 176h (374b)

decrypt_loop:

	cmp	edx,ebx 		  ;
	je	reset_key		  ; if loop count and key lenght = then reset to begining of key
	sub	byte [esi], byte 7Fh	  ; subtract 127, 127 was added durring encryption to help obfuscate the encryption key
	mov	al, byte [edi]		  ; move a byte of the encryption key to AL
	sub	byte [esi], al		  ; subtract key byte from SLIC byte
	inc	esi			  ; increment SLIC address
	inc	edi			  ; increment encryption key address
	inc	ebx			  ; increment loop counter
	loop decrypt_loop		  ; loop
	jmp decrypt_done

reset_key:

	xor ebx,ebx			  ; zero the loop counter
	pop edi 			  ; reset EDI with the encryption key address
	push edi			  ; save encryption key address back to stack
	jmp decrypt_loop		  ; jump back in to the loop

decrypt_done:

	pop	edi
	pop	ecx
	pop	esi
	pop	edi
	retn

;----------------------------------------------------------------------------------------------------
;||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
;----------------------------------------------------------------------------------------------------
SetReadWriteMask:
;----------------------------------------------------------------------------------------------------

	pushad
	cmp	byte [cs:ConfigByte], 1
	je	SetReadWriteMaskDone
	sti
	push	ax
	mov	ax, 0B101h
	int	1Ah
	cmp	edx, 'PCI '
	jne	Unsupported
	xor	ecx,ecx
	mov	ax, 0B10Ah
	mov	bx, 0
	mov	di, 0
	int	1Ah
	cmp	cx, 8086h
	jne	Unsupported
	shr	ecx,10h
	cmp	cx, 7190h			 ; white/blaklist device id's
	je	OldIntel
	shr	ecx, 0Ch
	cmp	cl, 1
	je	OldIntel
	cmp	cl, 2
	jg	Unsupported
NewIntel:
	mov	di, 90h
	jmp	GetConfigByte
OldIntel:
	mov	di, 59h
GetConfigByte:
	mov	ax, 0B108h
	xor	ecx, ecx
	mov	bx, 0
	int	1Ah
	pop	ax
	cmp	al, 1
	je	ReadWrite
ReadOnly:
	cmp	byte [cs:ConfigByte], 10h
	jb	SetReadWriteMaskDone
	cmp	byte [cs:ConfigByte], 30h
	je	SetReadWriteMaskDone
	jmp	SetConfigByte
ReadWrite:
	mov    byte [cs:ConfigByte], cl
	cmp	cl, 30h
	je	SetReadWriteMaskDone
	mov	cl, 30h
SetConfigByte:
	mov	ax, 0B10Bh
	int	1Ah
	jmp SetReadWriteMaskDone
Unsupported:
	pop ax
SetReadWriteMaskDone:
	popad
	retn

;----------------------------------------------------------------------------------------------------
;||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
;----------------------------------------------------------------------------------------------------