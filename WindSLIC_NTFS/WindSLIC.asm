;-----------------------------------------------------------------------
; WindSLIC - bootsect
;-----------------------------------------------------------------------
;///////////////////////////////////////////////////////////////////////
;=======================================================================

; Note: Step-by-Step Instructions can be found in: readme-ws.txt 
; Note: A brief changelog/"what's new" can be found in: ws_chlog.txt 

;-----------------------------------------------------------------------
; -v-  Constants
;-----------------------------------------------------------------------
include 'ws_const.inc'
include 'config.ini'
;-----------------------------------------------------------------------
; -^-  Constants
;-----------------------------------------------------------------------
;
;=======================================================================
; -^- Stuff for users to configure before compiling this ROM image
;=======================================================================

;///////////////////////////////////////////////////////////////////////
; -v- Stuff for advanced users/developers to configure if needed
;///////////////////////////////////////////////////////////////////////
;
;-----------------------------------------------------------------------
; -v-  Stack and Data Area Configuration
;-----------------------------------------------------------------------
;
; Note: Segment 8000 has been chosen since (it is believed that) nothing
;       is using this segment during this phase in the boot sequence.
;
; Note: Originally, 0x9000 was chosen, but the high end of that segment
;       tends to house the EBDA, so earlier versions probably stepped onr
;       the EBDA with their stack segment.
;
; Set the Resvd Stack Segment to Use
;if ($$ = 0x100) ; if $$ is 0x100, this is a DOS COM file
;RESVD_STACK_SEG = "cs"          ; if so, create a temporary stack at end of CS
;
;RESVD_STACK_OFS = 0xFF00       ; create a temporary stack ptr at offset 0xff00 
;RESVD_STACK_OFS = 0xFFFE        ; create a temporary stack ptr at offset 0xff00
;
RESVD_DATA_SEG = 0x8000 	; create a temporary data segment at 0x8000
;RESVD_DATA_OFS = 0x0000         ; create a temporary data offset  at 0x0000
;
;-----------------------------------------------------------------------
; -^-  Stack and Data Area Configuration
;-----------------------------------------------------------------------
;
;-----------------------------------------------------------------------
; -v-  Interrupt Vector Table Stolen Vectors Configuration
;-----------------------------------------------------------------------
WDSC_MARKER = 'x'
WDSC_IDENTS = 'z'
;-----------------------------------------------------------------------
ENTRY_FIRST_IVT_ENTRY	=	0x90	; For IVT, Set this to 0x90 (First DOS-NonUsed IVT Entry is 0x90)
;-----------------------------------------------------------------------
;
ENTRY_HOLD_WDSC_MARKER	=	ENTRY_FIRST_IVT_ENTRY + 0	; use IVT Entry +0 to hold 'WDSC' marker
ENTRY_HOLD_WDSC_IDENTS	=	ENTRY_FIRST_IVT_ENTRY + 1	; use IVT Entry +1 to hold 'WDSC' version/ident
;
ENTRY_HOLD_RSDP_ADDR	=	ENTRY_FIRST_IVT_ENTRY + 2	; use IVT entry +2 to hold 32-bit linear RSDP addr  
;
ENTRY_HOLD_IH_ENABLE	=	ENTRY_FIRST_IVT_ENTRY + 3	; use IVT entry +3 to hold e820 override enable
ENTRY_HOLD_OLD_I15H	=	ENTRY_FIRST_IVT_ENTRY + 4	; use IVT entry +4 to hold old Int15 Handler
;
ENTRY_HOLD_E820_CCODE	=	ENTRY_FIRST_IVT_ENTRY + 5	; use IVT entry +5 to hold our e820 contcode
ENTRY_HOLD_E820_RSVTN	=	ENTRY_FIRST_IVT_ENTRY + 6	; use IVT entry +6 to hold the reserved base addr
;
;-----------------------------------------------------------------------
; -^-  Interrupt Vector Table Stolen Vectors Configuration
;-----------------------------------------------------------------------
;
;-----------------------------------------------------------------------
; -v-  ACPI Reservation Parameters
;-----------------------------------------------------------------------
; [!] You shouldn't have to change these unless you manually edit the
; assembly language procedures below the user-configurable parameters
; section (this section).
;-----------------------------------------------------------------------
; These parameters control how much space the e820 hook reserves 
; out of the available memory beyond 1MB to hold our 'new' ACPI
; tables
;-----------------------------------------------------------------------
; Set the "TYPE" of reservation to make the reserved ACPI portion
;
ACPI_RESERVE_TYPE = 3		; 3 is "ACPI data", 4 is "ACPI NVS"
;
;-----------------------------------------------------------------------
; -v- EBDA Offsets for our copied ACPI RSDP Table
;-----------------------------------------------------------------------
;rsdp must be on an offset divisible by 16d(0x10)
;(e)bda (r)sdp (t)able (r)eservation ==> ertr_
ertr_offset_max    = 0x400	; max possible EBDA is 0x400 (1024d)

;Start checking at EBDA offset:
;       0x010 - to get the most chances to succeed
;       0x200 - to be safest about possible BIOS collisions with EBDA usage
ertr_check_start   = 0x010	; start check at offset 0x200 (byte 512d) 

ertr_table_len	   =  0x24	; reserve 36d (0x24) bytes
;so, given the max possible and the amount to reserve, calculate the end to check

;Since the table must be placed on a 16-byte boundary, figure out
;the highest offset we have to check by starting at our declared
;maximum (1K (which is 0x400)), and subtract the needed table-length
;rounded to the nearest higher 16-byte interval (aka a 24-byte table
;needs to be rounded up to 32-bytes so that it falls on the 16-byte
;boundary)
if (ertr_table_len mod 0x10)
	ertr_check_end = (ertr_offset_max - (((ertr_table_len / 16) * 16) + 16))
else
	ertr_check_end = (ertr_offset_max - (ertr_table_len))
end if
;-----------------------------------------------------------------------
; -^- EBDA Offsets for our copied ACPI RSDP Table
;-----------------------------------------------------------------------
;
;-----------------------------------------------------------------------
; -v- Reserved Area Offsets for our new ACPI Tables
;-----------------------------------------------------------------------
; [!] You shouldn't have to change these unless you determine that a
; motherboard for some reason has a huge RSDT table (after ACPI 4.0?)
; or the slic size increases.
;-----------------------------------------------------------------------
;
;rsdt offset starts at 0
rsdt_table_resvd_offset = 0x0000	; start at offset 0x0000
rsdt_table_resvd_len	= 0x0100	; reserve at least 1024(0x400) byte
;
;xsdt offset based on new rsdt offset plus its length
xsdt_table_resvd_offset = (rsdt_table_resvd_offset + rsdt_table_resvd_len)
xsdt_table_resvd_len	= 0x0100	; reserve at least 1024(0x400) byte
;
;calculate slic offset based on new xsdt offset plus length.
slic_table_resvd_offset = (xsdt_table_resvd_offset + xsdt_table_resvd_len)
slic_table_resvd_len	= 0x200 	; reserve at least 374d(0x176) bytes
;
;-----------------------------------------------------------------------
; -^- Reserved Area Offsets for our new ACPI Tables
;-----------------------------------------------------------------------
; Reserve this amount of space for ACPI tables
;   based on the offsets/lengths above
;   limited to 4GB max (an absurd number, but the reason is that
;   the code only verifies the lower 32-bits of the e820 memory
;   ranges for a big enough chunk of free memory.
;
ACPI_RESERVE_SPACE = (rsdt_table_resvd_len + xsdt_table_resvd_len + slic_table_resvd_len)
;
;-----------------------------------------------------------------------
; -^-  ACPI Reservation Parameters
;-----------------------------------------------------------------------
;
;///////////////////////////////////////////////////////////////////////
; -^- Stuff for advanced users/developers to configure if needed
;///////////////////////////////////////////////////////////////////////
;///////////////////////////////////////////////////////////////////////
; -v- Look! Actual code begins below! -v-
;///////////////////////////////////////////////////////////////////////
;-----------------------------------------------------------------------
use16  ; generate 16-bit code by default
;-----------------------------------------------------------------------
;
;-----------------------------------------------------------------------
; -v-  Macros 
;-----------------------------------------------------------------------
include 'ws_macro.inc'			; ws_macro.inc - macro file, main
include 'ws_ivtmc.inc'			; ws_ivtmc.inc - macro file, ivt
;-----------------------------------------------------------------------
; -^-  Macros 
;-----------------------------------------------------------------------
;-----------------------------------------------------------------------
; ACPI Reservation Parameters (Calculated from values above)
;-----------------------------------------------------------------------
;The high and low dwords are calculated based on the ACPI_RESERVE_SPACE above.
ACPI_RESERVE_SPACE_HIGH = ((ACPI_RESERVE_SPACE - (ACPI_RESERVE_SPACE mod 0xffffffff)) / 0xffffffff)
ACPI_RESERVE_SPACE_LOW = (ACPI_RESERVE_SPACE and 0xffffffff)
;
start:

mov ah,01h			 ; recovery
int 16h
cmp ah, 3Fh			 ; test for F5 key
jnz INIT
retn
;
;-----------------------------------------------------------------------
; -v-  GDTR Table Stuff
;-----------------------------------------------------------------------
include 'ws_gdtrs.inc'
;-----------------------------------------------------------------------
; -^-  GDTR Table Stuff
;-----------------------------------------------------------------------
;
;-----------------------------------------------------------------------
INIT:				
;-----------------------------------------------------------------------
;startp  "INIT:"
;-----------------------------------------------------------------------
;
	pushad

	cmp_ivt_entry_against ENTRY_HOLD_WDSC_MARKER,WDSC_MARKER    ; check if another WDSC is already loaded
	jne INIT.first_instance

	cmp_ivt_entry_against ENTRY_HOLD_WDSC_IDENTS,WDSC_IDENTS   ; check if another WDSC is already loaded
	jae INIT.failure_Duplicate_Detected

INIT.first_instance:

	set_ivt_entry_from_literal ENTRY_HOLD_WDSC_MARKER, WDSC_MARKER	    ; Set a marker to prevent double-execution
	set_ivt_entry_from_literal ENTRY_HOLD_WDSC_IDENTS, WDSC_IDENTS	    ; Set a marker to prevent double-execution

	jmp INIT.success	; if we got this far, assume INIT succeeded.

INIT.failure_Duplicate_Detected:
	popad
	retn
INIT.failure_cleanup:
INIT.failure_cleanup_quick:
INIT.success:
INIT.done:
	popad
;
;-----------------------------------------------------------------------
;endofp  "INIT:"
;-----------------------------------------------------------------------
;
BEV:				
;-----------------------------------------------------------------------
;startp  "BEV:"
;-----------------------------------------------------------------------
;
	pushfd
	multipush	ds,es,fs,gs,ss
	pushad
	;Setup a New Stack to keep old one intact
	;setup_new_stack RESVD_STACK_SEG,RESVD_STACK_OFS

	call	get_highest_free_e820_line			; Get the highest free e820 line to
								; from which to reserve memory
;---------------------------------------
;-v- BEV_core_only:
; NOTE: during this part, unreal mode will be active. 
;     : Do not load DS or ES, as they will lose unreal capability.
;     : Instead, use FS and GS for segment selectors when needed.
;---------------------------------------
;
BEV.core_only:

	pushad
	multipush	ds,es
	xor	ecx,ecx
	mov	al,1
	call	setRealMode		; Enter 'Big-Real'/'Unreal' Mode (Enable a20)
	push	ecx
	call	main
	pop	ecx
	xor	al,al
	call	setRealMode		; Return to 'Regular' Real Mode (Disable a20)
	multipop	ds,es
	popad
;
;---------------------------------------
;-^- BEV.core_only:
;---------------------------------------

	; Then, if the override is enabled,
	;  replace the int15h handler with our own.
	;
	; This will allow us to actually make the reservations
	;  we calculated above.

	cmp_ivt_entry_against	ENTRY_HOLD_IH_ENABLE,0x01	; If override is not enabled, 
	jne	BEV.skip_replace_ih_call			; Skip over the handler hooking step.


	call	replace_handler__int15h 		; Replace the int-hdlr for i15h
	jmp	BEV.finish

BEV.skip_replace_ih_call:

	jmp BEV.finish

BEV.finish:

	;teardown_new_stack      ; teardown the new stack created for the BEV

BEV.skip_init:

	popad
	multipop  ds,es,fs,gs,ss
	popfd

BEV.done:

	sti			; enable interrupts (just in case)
	retn
;
;-----------------------------------------------------------------------
;endofp  "BEV:"
;-----------------------------------------------------------------------
;
;----------------------------------------------------------------------------------------------------
; -v- "Main" Procedures and A20-Line Support Functions
;----------------------------------------------------------------------------------------------------
;
;-----------------------------------------------------------------------
main:
;-----------------------------------------------------------------------
;startp  "main:"
;-----------------------------------------------------------------------
;
	multipush	eax,edi
	call URM_get_ebda_address_into_esi
	call URM_get_ebda_end_into_edx
	call	URM_locate_rsdp_table
	jc	main.surrender_1			; If < , jmp to main.surrender_1
	
;------------------------------------------------------------------------------
;check SLIC version
;------------------------------------------------------------------------------

	push esi								;| save RSDP address to stack
	mov esi, [esi+RSDP_ofs_RSDTAddress]		;| move RSDT address to ESI
	mov ecx, [esi+RSDT_ofs_Length]			;| move RSDT length to ECX 
	sub ecx, 24h							;| subtract RSDT header
	shr	ecx, 2h								;| divide by 4
	add esi, 24h							;| move ESI past RSDT header
findSLICloop:								;|
	mov edx, [esi]							;| move address of first entry to EDX
	cmp dword [edx], 'SLIC'					;| compair to SLIC
	jz	findSLICfound						;| 
	add esi,4h								;| add 4 to RSDT address
	loop findSLICloop						;| loop
	jmp findSLICdone						;| loop has quit SLIC not found 
findSLICfound:								;| SLIC found check version
	mov al,[edx+8h]							;| move SLIC version to AL
	cmp al, 1								;| compair SLIC version to 01
	jnz findSLICdone						;| if not version 01 continue
	set_ivt_entry_from_literal	ENTRY_HOLD_IH_ENABLE,0x00 ;| if version 01 DO NOT install replacement int handler
	pop esi									;| restore RSDP address to ESI from stack
	jmp main.done							;| SLIC version 01 found quit
findSLICdone:								;|
	pop esi									;| restore RSDP address to ESI from stack
	
;------------------------------------------------------------------------------
;end check SLIC version
;------------------------------------------------------------------------------

	cmp	byte [cs:ForceFallBack], 0
	je	main.noforcefallback
	mov	byte [cs:main.forcefallback], 0EBh
main.noforcefallback:
	cmp_ivt_entry_against ENTRY_HOLD_IH_ENABLE,0x01 ; If override is not enabled,
main.forcefallback:
	jne	main.prepare_fallback_spaces		; use fallback non-moved RSDP,?SDT,and SLIC

main.prepare_resvd_spaces:

	; Write 00h to reserved memory area
	push	edi
	push	ecx
	get_ivt_entry_to_reg edi,ENTRY_HOLD_E820_RSVTN
	mov	ecx,ACPI_RESERVE_SPACE
	call	cleanup_reserved
	pop	ecx
	pop	edi

	; test RSDP area is writable, try to make it writable
	call	 rsdp_writable
	jc	 main.skip_move_rsdp

	; Move RSDP (to EBDA), RSDT (to HMEM), and XSDT (to HMEM)
	multipush	edi,esi
	call	URM_copy_rsdp_into_ebda
	multipop	edi,esi

	call	URM_locate_rsdp_table			; Extra call to re-determine if RSDP copied to EBDA
	jc	main.surrender_1			; If < , jmp to main.surrender_1

main.skip_move_rsdp:

	; move int15_handler to EBDA
	call move_handler

	;Move the RSDT table
	call	URM_copy_rsdt_into_rsvd_space

	;Move the XSDT table
	call	URM_copy_xsdt_into_rsvd_space

	; Copy the SLIC into Resvd Memory
	call	URM_copy_slic_table_to_resvd_memory
	;call    URM_copy_handeler_to_resvd_memory

	jmp	main.post_slic_move

main.prepare_fallback_spaces:

	call	move_handler
	call	URM_copy_slic_table_to_resvd_memory
	push	edi
	jmp	main.skip_patch_oemid
	
main.post_slic_move:
	; Idea: if 'fallback', then we're leaving SLIC in CS:SLIC
	; and pointing RSDT/XSDT to the appropriate location.
	; This is done by scanning the tables, and changing
	; the SLIC pointer, or appending the SLIC pointer if
	; it didn't already exist.
	push   edi

	cmp    [cs:AddOEMID], 0
	je     main.skip_patch_oemid
	call   URM_update_rsdt_tables
	cmp    eax,0
	je     main.skip_patch_oemid		   ; if no matches in RSDT dont patch XSDT or RSDP
	call   URM_update_xsdt_tables
	xor    ebx,ebx
	call   URM_copy_slic_oemname_to_esi	   ; add oemname to RSDP


main.skip_patch_oemid:

	pop    edi
	call   URM_patch_rsdt_for_slic
	call   URM_patch_xsdt_for_slic
	call   URM_chksumfix_rsdp_input_esi
	xor	al, al
	call	SetReadWriteMask
	jmp    main.done

main.surrender_1:

main.done:

	multipop	eax,edi
	retn
;
;-----------------------------------------------------------------------
;endofp  "main:"
;-----------------------------------------------------------------------
;
;-----------------------------------------------------------------------
enableA20:
;-----------------------------------------------------------------------
;startp  "enableA20:"
;-----------------------------------------------------------------------
	in	al,92h
	test	al,02h
	jne	enableA20.open
	or	al,02h
	out	92h,al
	retn
enableA20.open:
	mov	cl,01h
	retn
;-----------------------------------------------------------------------
;endofp  "enableA20:"
;-----------------------------------------------------------------------
;
;-----------------------------------------------------------------------
disableA20:
;-----------------------------------------------------------------------
;startp  "disableA20:"
;-----------------------------------------------------------------------
	in	al,92h
	cmp	cl,01h
	je	disableA20.exit
	and	al,0fdh
	out	92h,al
disableA20.exit:
	retn
;-----------------------------------------------------------------------
;endofp  "disableA20:"
;-----------------------------------------------------------------------
;
;-----------------------------------------------------------------------
setRealMode:
;-----------------------------------------------------------------------
;startp  "setRealMode:"
;-----------------------------------------------------------------------
	push	eax
	cli
	or	al,al
	jz	short setRealMode.offA20	;
	call	enableA20
	jmp	setRealMode.enable4gb		;
setRealMode.offA20:
	call	disableA20
setRealMode.enable4gb:
	push	ecx

	;-----------------------------------------------------------------------
	; The following method using the stack because some of the AWARD
	; motherboard 8800:AE00 such area is read-only,
	; I must go back to the stack
	;-----------------------------------------------------------------------

	xor	eax,eax
	xor	ecx,ecx 			;calculation of cs: GDTR.GDT1 the physical address for LGDT
	mov	ax,cs				;
	shl	eax,4h				;
	mov	cx,GDTR.GDT1			;here to use the stack operation,
						;you can not be rewritten without
						;considering the value of pseudo-problem GDTR
	add	eax,ecx
	push	eax
	mov	ax,GDTR.GDT1_END-GDTR.GDT1-1h
	push	ax
	xor	edi,edi
	mov	di,sp				;here to use the stack to preserve the value of pseudo-GDTR
	add	sp,6h				;to restore the original stack pointer! 6h = the length of the pseudo-GDTR
	lgdt	[ss:di] 			;f the PC repeatedly restart, the majority of cases, because GDTR not set well.
	mov	eax,cr0 			
	or	al,0x01
	mov	cr0,eax 			;enter protected mode
	jmp	setRealMode.pm_flush_queue
setRealMode.pm_flush_queue:
	pop	ecx
	pop	eax
	push	eax
	push	ecx
	or	al,al
	jz	short setRealMode.to64k
	mov	ax,GDTR.DATA4G_INDEX
	jmp	short setRealMode.enable4gbOK
setRealMode.to64k:
	mov	ax,GDTR.DATA64K_INDEX
setRealMode.enable4gbOK:
	mov	dl,al
	mov	es,ax
	mov	ds,ax
	mov	eax,cr0
	and	al,0xFE
	mov	cr0,eax
	jmp	setRealMode.rm_flush_queue			;clear CPU instruction queue
setRealMode.rm_flush_queue:
	xor	ax,ax
	mov	dh,al
	mov	ds,ax
	mov	es,ax
	sti					;resume interrupts
	pop	ecx
	pop	eax
	retn
;
;-----------------------------------------------------------------------
;endofp  "setRealMode:"
;-----------------------------------------------------------------------
;
;----------------------------------------------------------------------------------------------------
; -^- "Main" Procedures and A20-Line Support Functions
;----------------------------------------------------------------------------------------------------
;
;-----------------------------------------------------------------------
; -v-  Relocated functions and addons
;-----------------------------------------------------------------------
include 'ws_e820.inc'
include 'OtherFunctions.inc'
include 'ws_acpi.inc'
include 'ws_ihdlr.inc'
;-----------------------------------------------------------------------
; -^-  Relocated functions and addons
;-----------------------------------------------------------------------
;
include 'datasection.inc'
;
;///////////////////////////////////////////////////////////////////////
; -^- Look! Actual code ends above! -^-
;///////////////////////////////////////////////////////////////////////
times 2000h - ($ - $$)	db 0  ; pad to 8192b