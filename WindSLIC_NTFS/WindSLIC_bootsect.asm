org 0
;
; begin MS-VBS (Volume Boot Sector) - dissassembled by untermensch 4/6/2010
; assemble with FASM - flatassembler.net
;
;----------------------------------------------------------------------------
;||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
;----------------------------------------------------------------------------

use16

		jmp	short VBS_start
		nop

;----------------------------------------------------------------------------
; begin - this section is senstive to offset changes DO NOT EDIT
;----------------------------------------------------------------------------
; this section is the BIOS paramater block, the values in this section
; are set when the drive is partitioned. the volume boot sector code reads
; paramaters from this area, and and uses the areas unused by NTFS for
; local variable storage, DO NOT copy and paste this section over your
; existing BPB (BIOS paramater block)
;----------------------------------------------------------------------------

fs_sig			  dq 'NTFS    '
sector_size		  dw 0		 ; sector size WORD - 200h (512b) typical value
sectors_per_cluster	  db 0		 ; sectors per cluster BYTE - 8 typical value
drive_index		  db 0		 ; local variable for the boot drive, usualy 80h
calc_sector_size	  dw 0		 ; must be zero for ntfs / used for variable storage
start_sector		  dd 0		 ; unused by ntfs, fat12/16
			  db 0F8h	 ; media descriptor F8 = hard drive
sectors_to_read_counter   dw 0		 ; must be zero for ntfs, fat12/16
			  dw 0		 ; sectors per track
			  dw 0		 ; number of heads
hidden_sectors		  dd 0		 ; number of hidden sectors
			  dd 0		 ; unused by ntfs, fat32
			  dw 80h	 ; 8000h
			  dw 0		 ; 8000h
			  dq 0		 ; total sectors 8b
starting_cluster_MFT	  dq 0		 ; starting cluster number for the $MFT file in this partition, 8b
			  dq 0		 ; starting cluster number for the $MFT mirror File in this partition, 8b
clusters_per_record	  dd 0		 ; clusters (or bytes) per file record segment, 4b
			  dd 0		 ; clusters per Index Block, 4b
			  dq 0		 ; volume serial number, 8b
			  dd 0		 ; unused by NTFS, 4b

;----------------------------------------------------------------------------
; end - this section is senstive to offset changes DO NOT EDIT
;----------------------------------------------------------------------------
;||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
;----------------------------------------------------------------------------
VBS_start:
;----------------------------------------------------------------------------

		cli				     ; disable interupts
		xor	ax, ax			     ; zero AX
		mov	ss, ax			     ; zero stack segment
		mov	sp, 7C00h		     ; set stack pointer to 7C00h (0000:7C00h)
		sti				     ; enable interupts
		push	7C0h			     ; push 7C0h to stack
		pop	ds			     ; pop 7C0h from stack to DS
		push	ds			     ; push DS (with value 7C0h) on to stack
		push	word entry_point0	     ; push offset of entry_point0 on to stack
		retf				     ; RETF will pop the last 2 values off the
						     ; stack and begin execution, so DS gets
						     ; popped to CS and entry_point0 is the
						     ; return instruction pointer

;----------------------------------------------------------------------------
;||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
;----------------------------------------------------------------------------
entry_point0:
;----------------------------------------------------------------------------

		mov	[ds:drive_index], dl	     ; DL = drive index

		cmp	dword [ds:fs_sig], 'NTFS'    ; check OEM ID
		jnz	short disk_read_error_short

;----------------------------------------------------------------------------
; test for int 13 extensions
;----------------------------------------------------------------------------

		mov	ah, 41h 		     ; AH = 41h: Check Extensions Present
		mov	bx, 55AAh
		int	13h
		jb	short disk_read_error_short
		cmp	bx, 0AA55h
		jnz	short disk_read_error_short
		test	cx, 1
		jnz	short extended_read_drive_prams

;----------------------------------------------------------------------------

disk_read_error_short:

		jmp	disk_read_error

;----------------------------------------------------------------------------

extended_read_drive_prams:

		push	ds
		sub	sp, 18h
		push	word 1Ah		     ; EDD configuration parameters
		mov	ah, 48h 		     ; AH=48h: Extended Read Drive Parameters
		mov	dl, [ds:drive_index]	     ; move drive number to DL (80h)
		mov	si, sp
		push	ss
		pop	ds
		int	13h
		lahf
		add	sp, 18h
		sahf
		pop	ax			     ; pop word bytes per sector to AX
		pop	ds
		jb	short disk_read_error_short

		cmp	ax, [ds:sector_size]	     ; compare AX with value on drive usualy 512b (200h)
		jnz	short disk_read_error_short  ; error if not equal

;----------------------------------------------------------------------------
; at this point only 512b (200h) of the volume boot sector is in memory
; this next section gets the rest into memory.
;----------------------------------------------------------------------------
;
;----------------------------------------------------------------------------
; calculate sector size
;----------------------------------------------------------------------------

		mov	[ds:calc_sector_size], ax     ; move AX to
		shr	[ds:calc_sector_size], 4      ; now 32b (20h), assuming AX is 512b (200h)

;----------------------------------------------------------------------------

		push	ds
		pop	dx			     ; pop DS (data segment) to DX
		xor	bx, bx			     ; zero BX
		mov	cx, 2000h		     ; 2000h = 8192b
		sub	cx, ax			     ; subtract 512b (200h) from 8192b (2000h) = 7680b (1E00h)
		inc	[ds:start_sector]	     ; increment start sector to 1 (sector 0 already loaded)

;----------------------------------------------------------------------------

finish_loading_VBS:

		add	dx, [ds:calc_sector_size]    ; add sector size to the segment pointer ES now points to the end of
		mov	es, dx			     ; the portion of the volume boot sector that is already in memory
		inc	[ds:sectors_to_read_counter] ; set read counter to 1
		call	extended_read_sectors
		sub	cx, ax			     ; subtract sector size from CX
		ja	short finish_loading_VBS     ; loop while CX > AX

;----------------------------------------------------------------------------
; at this point another 7680b (1E00h) was loaded into memory, that
; is all of the volume boot sector and quite abit more since at this time
; the volume boot sector is only a little more than 1000b
;----------------------------------------------------------------------------

		call	 windslic
		mov cx, 2000h - windslic
		mov di, windslic

clear_evidence:

		mov byte [di], byte 00h
		inc di
		loop clear_evidence

;----------------------------------------------------------------------------
; calculates cluster size in bytes
;----------------------------------------------------------------------------


		movzx	eax, [ds:sector_size]		; 512b (200h)
		movzx	ebx, [ds:sectors_per_cluster]	; 8
		mul	ebx
		mov	[ds:cluster_size_bytes], eax	; result 4096b (1000h) store result to cluster_size_bytes


		jmp	loc_27A 	  ; jump to the newly loaded code

;----------------------------------------------------------------------------
;||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
;----------------------------------------------------------------------------
extended_read_sectors:
;----------------------------------------------------------------------------

		pushad
		push	ds
		push	es   ; push ES to stack

extended_read_sectors_loop:

		mov	eax, [ds:start_sector]	  ; start sector
		add	eax, [ds:hidden_sectors]  ; add number of hidden sectors to EAX
		push	ds
		push	dword 0 		  ; push 00000000 to stack
		push	eax			  ; sector to start read
		push	es			  ; segment to load sectors to
		push	bx			  ; offset  to load sectors to
		push	word 1			  ; 0001, unused byte & number of sectors to read byte 01h
		push	word 10h		  ; size of DAP 16b (10h)
		mov	ah, 42h 		  ; AH = 42h: Extended Read Sectors From Drive
		mov	dl, [ds:drive_index]	  ; move hard drive number to DL
		push	ss			  ;
		pop	ds			  ; pop stack segment to DS
		mov	si, sp			  ; pointer to DAP
		int	13h
		pop	ecx
		pop	bx
		pop	dx
		pop	ecx
		pop	ecx
		pop	ds
		jb	disk_read_error

		inc	[ds:start_sector]
		add	dx, [ds:calc_sector_size]	   ; add another 512b (200h) to
		mov	es, dx				   ; the segment pointer
		dec	[ds:sectors_to_read_counter]	   ; if this instruction does not
		jnz	short extended_read_sectors_loop   ; set the zero flag loop

		pop	es   ; pop ES so sector size is not added twice
		pop	ds
		popad
		retn

; ---------------------------------------------------------------------------
;||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
;----------------------------------------------------------------------------

disk_read_error:


		mov	al, [ds:error_disk_read_offset]

;----------------------------------------------------------------------------

error:

		call	display_error
		mov	al, [ds:restart_offset]
		call	display_error

;----------------------------------------------------------------------------

stop:

		hlt
		jmp	short stop


;----------------------------------------------------------------------------
;||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
;----------------------------------------------------------------------------
display_error:
;----------------------------------------------------------------------------

		mov	ah, 1
		mov	si, ax

display_error_loop:

		lodsb
		cmp	al, 0
		jz	short display_error_done
		mov	ah, 0Eh
		mov	bx, 7
		int	10h
		jmp	short display_error_loop

display_error_done:

		retn

;----------------------------------------------------------------------------
;||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
;----------------------------------------------------------------------------
; error messages
;----------------------------------------------------------------------------

error_disk_read    db 0Dh,0Ah,'A disk read error occurred',0
error_bootmgr	   db 0Dh,0Ah,'BOOTMGR is missing',0
error_compressed   db 0Dh,0Ah,'BOOTMGR is compressed',0
restart 	   db 0Dh,0Ah,'Press Ctrl+Alt+Del to restart',0Dh,0Ah,0

;----------------------------------------------------------------------------
;||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
;----------------------------------------------------------------------------
; message offsets
;----------------------------------------------------------------------------

error_disk_read_offset	db error_disk_read - 100h   ;08Ch
error_bootmgr_offset	db error_bootmgr - 100h     ;0A9h
error_compressed_offset db error_compressed - 100h  ;0BEh
restart_offset		db restart - 100h	    ;0D6h

;----------------------------------------------------------------------------
;||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
;----------------------------------------------------------------------------
; begin - data section
;----------------------------------------------------------------------------

bootmgr_len		dw (bootmgr_end - bootmgr) / 2	; 7
bootmgr 		db 'B',0,'O',0,'O',0,'T',0,'M',0,'G',0,'R',0
bootmgr_end:

;----------------------------------------------------------------------------

i30_len 		dw (i30_end - i30) / 2	       ; 4
i30			db '$',0,'I',0,'3',0,'0',0
i30_end:

;----------------------------------------------------------------------------

dword_21A		dd 0D400h
dword_21E		dd 2400h

times 200h - 2h - ($)	db 0 ; pad whaterver is left then add boot signature starting at 510 bytes

boot_sig		dw 0AA55h  ; boot signature must start at 510 bytes

;----------------------------------------------------------------------------
; end - data section
;----------------------------------------------------------------------------
;
;----------------------------------------------------------------------------

loc_27A:

;----------------------------------------------------------------------------
; calculates MTF record size in bytes
;----------------------------------------------------------------------------

		mov	ecx, [ds:clusters_per_record]	; F6h typical value F6h = -10
		cmp	cl, 0				; if a positive number
		jg	loc_2A7 			; jump to simple multiply
		neg	cl				; 0 - F6h = 0Ah
		mov	eax, 1
		shl	eax, cl 			; 1h SHL 0Ah = 400h (1024b)
		jmp	short loc_2AE
		;nop

loc_2A7:

		mov	eax, [ds:cluster_size_bytes]	; [ds:cluster_size_bytes] * [ds:clusters_per_record]
		mul	ecx

loc_2AE:

		mov	[ds:record_size_bytes], eax	; store result in [ds:record_size_bytes]

;----------------------------------------------------------------------------
; calculates sectors per MTF record
;----------------------------------------------------------------------------

		movzx	ebx, [ds:sector_size]		; 200h (512b)
		xor	edx, edx			; zero EDX or DIV wont work
		div	ebx				; 400h / 200h = 2h or 1024 / 512 = 2
		mov	[ds:sectors_per_record], eax	; sectors per record = 2

;----------------------------------------------------------------------------

		call	sub_75A 			; sub to read $MFT (Master File Table)
		mov	ecx, [ds:dword_24E]
		mov	[ds:dword_226], ecx
		add	ecx, [ds:record_size_bytes]
		mov	[ds:dword_22A], ecx
		add	ecx, [ds:record_size_bytes]
		mov	[ds:dword_22E], ecx
		add	ecx, [ds:record_size_bytes]
		mov	[ds:dword_23E], ecx
		add	ecx, [ds:record_size_bytes]
		mov	[ds:dword_246], ecx
		mov	eax, 90h
		mov	ecx, [ds:dword_226]
		call	sub_C88
		or	eax, eax
		jz	disk_read_error
		mov	[ds:dword_232], eax
		mov	eax, 0A0h
		mov	ecx, [ds:dword_22A]
		call	sub_C88
		mov	[ds:dword_236], eax
		mov	eax, 0B0h
		mov	ecx, [ds:dword_22E]
		call	sub_C88
		mov	[ds:dword_23A], eax
		mov	eax, [ds:dword_232]
		or	eax, eax
		jz	disk_read_error
		cmp	byte [eax+8], 0
		jnz	disk_read_error
		lea	edx, [eax+10h]
		add	ax, [edx+4]
		movzx	ecx, byte [eax+0Ch]
		mov	[ds:dword_272], ecx
		mov	ecx, [eax+8]
		mov	[ds:dword_26E], ecx
		mov	eax, [ds:dword_26E]
		movzx	ecx, [ds:sector_size]
		xor	edx, edx
		div	ecx
		mov	[ds:dword_276], eax
		mov	eax, [ds:dword_246]
		add	eax, [ds:dword_26E]
		mov	[ds:dword_24A], eax
		cmp	[ds:dword_236], 0
		jz	loc_3AE
		cmp	[ds:dword_23A], 0
		jz	disk_read_error
		mov	ebx, [ds:dword_23A]
		push	ds
		pop	es
		mov	edi, [ds:dword_24A]
		mov	eax, [ds:dword_22E]
		call	sub_58E

loc_3AE:

		movzx	ecx, [ds:bootmgr_len]
		mov	eax, bootmgr
		call	sub_BDF
		or	eax, eax
		jnz	loc_3DA
		;movzx   ecx, [ds:ntldr_len]
		;mov     eax, ntldr
		;call    sub_BDF
		or	eax, eax
		jz	bootmgr_missing

loc_3DA:

		mov	eax, [eax]
		push	ds
		pop	es
		mov	edi, [ds:dword_23E]
		call	sub_A27
		mov	eax, [ds:dword_23E]
		mov	ebx, 20h
		mov	ecx, 0
		mov	edx, 0
		call	sub_4E5
		test	eax, eax
		jnz	loc_42B
		mov	eax, [ds:dword_23E]
		mov	ebx, 80h
		mov	ecx, 0
		mov	edx, 0
		call	sub_4E5
		or	eax, eax
		jnz	loc_46C
		jmp	bootmgr_missing

; ---------------------------------------------------------------------------

loc_42B:

		xor	edx, edx
		mov	ecx, 80h
		mov	eax, [ds:dword_23E]
		call	sub_D05
		or	eax, eax
		jz	bootmgr_missing
		push	ds
		pop	es
		mov	edi, [ds:dword_23E]
		call	sub_A27
		mov	eax, [ds:dword_23E]
		mov	ebx, 80h
		mov	ecx, 0
		mov	edx, 0
		call	sub_4E5
		or	eax, eax
		jz	bootmgr_missing

loc_46C:
		movzx	ebx, word [eax+0Ch]
		and	ebx, 0FFh
		jnz	bootmgr_compressed
		mov	ebx, eax
		push	2000h
		pop	es
		sub	edi, edi
		mov	eax, [ds:dword_23E]
		call	sub_58E 		 ; move bootmgr to memory
		push	2000h
		pop	es
		sub	edi, edi
		mov	eax, [ds:dword_23E]
		;call    loc_F48             ; more trusted platform module stuff
		mov	dl, [ds:drive_index]	 ; pass drive index (80h) to next stage
		mov	ax, 3E8h		 ; move 1000 (3E8h) to AX
		mov	es, ax
		lea	si, [sector_size]
		sub	ax, ax
		push	2000h
		push	ax
		retf		     ;

; ---------------------------------------------------------------------------
;||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
; ---------------------------------------------------------------------------
sub_4B0:
; ---------------------------------------------------------------------------

		push	es
		push	ds
		pushad

		mov	ebx, edx
		movzx	ecx, [ds:sectors_per_cluster]
		mul	ecx
		mov	[ds:start_sector], eax
		mov	eax, ebx
		mul	ecx
		mov	[ds:sectors_to_read_counter], ax
		mov	bx, di
		and	bx, 0Fh
		mov	ax, es
		shr	edi, 4
		add	ax, di
		push	ax
		pop	es
		call	extended_read_sectors

		popad
		;nop
		pop	ds
		pop	es
		retn

; ---------------------------------------------------------------------------
;||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
; ---------------------------------------------------------------------------
sub_4E5:
; ---------------------------------------------------------------------------

		add	ax, [eax+14h]

loc_4E9:
		cmp	dword [eax], 0FFFFFFFFh
		jz	loc_53E
		cmp	[eax], ebx
		jnz	loc_52D
		or	ecx, ecx
		jnz	loc_50B
		cmp	byte [eax+9], 0
		jnz	loc_52D
		retn

; ---------------------------------------------------------------------------
loc_50B:
		cmp	cl, [eax+9]
		jnz	loc_52D
		mov	esi, eax
		add	si, [eax+0Ah]
		call	sub_BB4
		push	ecx
		push	ds
		pop	es
		mov	edi, edx
		repe cmpsw
		pop	ecx
		jnz	loc_52D
		retn

; ---------------------------------------------------------------------------

loc_52D:

		cmp	dword [eax+4], 0
		jz	loc_53E
		add	eax, [eax+4]
		jmp	short loc_4E9

loc_53E:

		sub	eax, eax
		retn

; ---------------------------------------------------------------------------
;||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
; ---------------------------------------------------------------------------
sub_542:
; ---------------------------------------------------------------------------

		mov	esi, ebx
		call	sub_BB4
		add	eax, [eax]

loc_54C:

		test	word [eax+0Ch], 2
		jnz	loc_58A
		lea	edx, [eax+10h]
		cmp	cl, [edx+40h]
		jnz	loc_57B
		lea	esi, [edx+42h]
		call	sub_BB4
		push	ecx
		push	ds
		pop	es
		mov	edi, ebx
		repe cmpsw
		pop	ecx
		jnz	loc_57B
		retn

; ---------------------------------------------------------------------------

loc_57B:
		cmp	word [eax+8], 0
		jz	loc_58A
		add	ax, [eax+8]
		jmp	short loc_54C

loc_58A:

		xor	eax, eax
		retn

; ---------------------------------------------------------------------------
;|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
; ---------------------------------------------------------------------------
sub_58E:
; ---------------------------------------------------------------------------
; move file to memory
; ---------------------------------------------------------------------------

		cmp	byte [ebx+8], 0
		jnz	loc_5B3
		push	es
		push	ds
		pushad
		lea	edx, [ebx+10h]
		mov	ecx, [edx]
		mov	esi, ebx
		add	si, [edx+4]
		rep movsb
		popad
		;nop
		pop	ds
		pop	es
		retn

; ---------------------------------------------------------------------------

loc_5B3:
		push	eax
		lea	edx, [ebx+10h]
		test	eax, eax
		jnz	loc_5CB
		mov	ecx, [edx+8]
		inc	ecx
		jmp	short loc_5DB
		;nop

loc_5CB:

		mov	eax, [edx+18h]
		xor	edx, edx
		div	[ds:cluster_size_bytes]
		mov	ecx, eax

loc_5DB:

		sub	eax, eax
		pop	esi
		call	sub_5E4
		retn

; ---------------------------------------------------------------------------
;||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
; ---------------------------------------------------------------------------
sub_5E4:

		push	es
		push	ds
		pushad
		cmp	byte [ebx+8], 1
		jz	loc_5F4
		jmp	disk_read_error

loc_5F4:

		cmp	ecx, 0
		jnz	loc_602
		popad
		;nop
		pop	ds
		pop	es
		retn

; ---------------------------------------------------------------------------

loc_602:

		push	ebx
		push	eax
		push	ecx
		push	esi
		push	edi
		push	es
		call	sub_AA1
		mov	edx, ecx
		pop	es
		pop	edi
		pop	esi
		pop	ecx
		test	eax, eax
		jz	loc_655
		cmp	ecx, edx
		jge	loc_62B
		mov	edx, ecx

loc_62B:

		call	sub_4B0
		sub	ecx, edx
		mov	ebx, edx
		mov	eax, edx
		movzx	edx, [ds:sectors_per_cluster]
		mul	edx
		movzx	edx, [ds:sector_size]
		mul	edx
		add	edi, eax
		pop	eax
		add	eax, ebx
		pop	ebx
		jmp	short loc_5F4

loc_655:

		test	esi, esi
		jz	disk_read_error
		push	ecx
		push	edi
		push	es
		movzx	eax, byte [ebx+9]
		test	eax, eax
		jz	loc_68E
		shl	eax, 1
		sub	esp, eax
		mov	edi, esp
		push	esp
		push	esi
		movzx	esi, word [ebx+0Ah]
		add	esi, ebx
		mov	ecx, eax
		rep movsb
		pop	esi
		jmp	short loc_690
		;nop

loc_68E:

		push	eax

loc_690:

		push	eax
		mov	eax, [ebx]
		push	eax
		mov	eax, [ebx+18h]
		push	eax
		mov	edx, [esi+20h]
		test	edx, edx
		jz	loc_6B6
		mov	edi, esi
		push	ds
		pop	es
		mov	eax, edx
		call	sub_A27

loc_6B6:

		mov	eax, esi
		pop	edx
		pop	ecx
		inc	edx
		push	ecx
		push	esi
		call	sub_D05
		test	eax, eax
		jz	disk_read_error
		pop	esi
		pop	ecx
		mov	edi, esi
		push	ds
		pop	es
		call	sub_A27
		mov	eax, esi
		mov	ebx, ecx
		pop	ecx
		pop	edx
		push	ecx
		push	esi
		shr	ecx, 1
		call	sub_4E5
		test	eax, eax
		jz	disk_read_error
		pop	esi
		pop	ecx
		add	esp, ecx
		pop	es
		pop	edi
		pop	ecx
		mov	edx, eax
		pop	eax
		pop	ebx
		mov	ebx, edx
		jmp	loc_602

; ---------------------------------------------------------------------------
;||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
; ---------------------------------------------------------------------------
sub_70D:
; ---------------------------------------------------------------------------

		push	es
		push	ds
		pushad

		movzx	ebx, word [es:edi+4]
		movzx	ecx, word [es:edi+6]
		or	ecx, ecx
		jz	disk_read_error
		add	ebx, edi
		add	ebx, 2
		add	edi, 1FEh
		dec	ecx

loc_736:
		or	ecx, ecx
		jz	loc_754
		mov	ax, [es:ebx]
		mov	[es:edi], ax
		add	ebx, 2
		add	edi, 200h
		dec	ecx
		jmp	short loc_736

loc_754:
		popad
		;nop
		pop	ds
		pop	es
		retn

;----------------------------------------------------------------------------
;||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
;----------------------------------------------------------------------------
sub_75A:
;----------------------------------------------------------------------------
; read $MFT
;----------------------------------------------------------------------------

		push	es
		push	ds
		pushad

		mov	eax, 1			     ; move 1 to EAX
		mov	[ds:dword_222], eax	     ; move 1 to dword_21E
		mov	eax, [ds:dword_21E]	     ; 2400h
		add	eax, [ds:record_size_bytes]  ; add 400h to 2400h = 2800h (10240b)
		mov	[ds:dword_26A], eax	     ; store result to dword_26A
		add	eax, [ds:record_size_bytes]  ; add 400h to 2800h = 2C00  (11264b)
		mov	[ds:dword_24E], eax	     ; store reslut to dword_24E
		mov	eax, dword [ds:starting_cluster_MFT]
		movzx	ebx, [ds:sectors_per_cluster]
		mul	ebx			     ; multiply starting cluster of MFT by sectors per cluster (2)
		mov	ebx, [ds:dword_24E]
		mov	[bx], eax
		mov	[ds:start_sector], eax
		add	bx, 4
		mov	eax, [ds:sectors_per_record]
		mov	[bx], eax
		mov	[ds:sectors_to_read_counter], ax
		add	bx, 4
		mov	[ds:dword_24E], ebx
		mov	ebx, [ds:dword_21E]
		push	ds
		pop	es
		call	extended_read_sectors
		mov	edi, ebx
		call	sub_70D
		mov	eax, [ds:dword_21E]
		mov	ebx, 20h
		mov	ecx, 0
		mov	edx, 0
		call	sub_4E5
		or	eax, eax
		jz	loc_8F5
		mov	ebx, eax
		push	ds
		pop	es
		mov	edi, [ds:dword_21A]
		xor	eax, eax
		call	sub_58E
		mov	ebx, [ds:dword_21A]

loc_7F1:

		cmp	dword [bx], 80h
		jz	loc_8E7
		add	bx, [bx+4]
		jmp	short loc_7F1


loc_801:

		push	ebx
		mov	eax, [bx+10h]
		mul	[ds:sectors_per_record]
		push	eax
		xor	edx, edx
		movzx	ebx, [ds:sectors_per_cluster]
		div	ebx
		push	edx
		call	sub_8FB
		or	eax, eax
		jz	disk_read_error
		mov	ecx, [ds:sectors_per_record]
		movzx	ebx, [ds:sectors_per_cluster]
		mul	ebx
		pop	edx
		add	eax, edx
		mov	ebx, [ds:dword_24E]
		mov	[bx], eax
		add	bx, 4
		movzx	eax, [ds:sectors_per_cluster]
		sub	eax, edx
		cmp	eax, ecx
		jbe	loc_857
		mov	eax, ecx

loc_857:

		mov	[bx], eax

loc_85A:

		sub	ecx, eax
		pop	edx
		jz	loc_8D8
		add	eax, edx
		push	eax
		xor	edx, edx
		movzx	ebx, [ds:sectors_per_cluster]
		div	ebx
		push	ecx
		call	sub_8FB
		pop	ecx
		or	eax, eax
		jz	disk_read_error
		movzx	ebx, [ds:sectors_per_cluster]
		mul	ebx
		mov	ebx, [ds:dword_24E]
		mov	edx, [bx]
		add	bx, 4
		add	edx, [bx]
		cmp	edx, eax
		jnz	loc_8B5
		movzx	eax, [ds:sectors_per_cluster]
		cmp	eax, ecx
		jbe	loc_8B0
		mov	eax, ecx

loc_8B0:

		add	[bx], eax
		jmp	short loc_85A


loc_8B5:

		add	bx, 4
		mov	[ds:dword_24E], ebx
		mov	[bx], eax
		add	bx, 4
		movzx	eax, [ds:sectors_per_cluster]
		cmp	eax, ecx
		jbe	loc_8D3
		mov	eax, ecx

loc_8D3:

		mov	[bx], eax
		jmp	short loc_85A

loc_8D8:

		add	bx, 4
		inc	[ds:dword_222]
		mov	[ds:dword_24E], ebx
		pop	ebx

loc_8E7:

		add	bx, [bx+4]
		cmp	dword [bx], 80h
		jz	loc_801

loc_8F5:

		popad
		;nop
		pop	ds
		pop	es
		retn

; ---------------------------------------------------------------------------
;||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
; ---------------------------------------------------------------------------
sub_8FB:
; ---------------------------------------------------------------------------

		mov	edx, eax
		mov	ecx, [ds:dword_222]
		mov	esi, [ds:dword_26A]
		add	esi, [ds:record_size_bytes]

loc_90D:

		push	edx
		push	ecx
		push	edx
		mov	ebx, [ds:dword_26A]
		mov	edi, [ds:sectors_per_record]

loc_91D:

		mov	eax, [si]
		mov	[ds:start_sector], eax
		add	si, 4
		mov	eax, [si]
		mov	[ds:sectors_to_read_counter], ax
		add	si, 4
		push	ds
		pop	es
		call	extended_read_sectors
		sub	edi, eax
		jz	loc_944
		mul	[ds:sector_size]
		add	bx, ax
		jmp	short loc_91D

loc_944:

		mov	edi, [ds:dword_26A]
		push	ds
		pop	es
		call	sub_70D
		mov	eax, [ds:dword_26A]
		mov	ebx, 80h
		mov	ecx, 0
		mov	edx, ecx
		call	sub_4E5
		or	eax, eax
		jz	disk_read_error
		mov	ebx, eax
		pop	eax
		push	esi
		call	sub_AA1
		pop	esi
		or	eax, eax
		jz	loc_983
		pop	ebx
		pop	ebx
		retn

; ---------------------------------------------------------------------------

loc_983:

		pop	ecx
		pop	edx
		loop	loc_90D
		xor	eax, eax
		retn

; ---------------------------------------------------------------------------
;||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
; ---------------------------------------------------------------------------
sub_98D:
; ---------------------------------------------------------------------------

		push	es
		push	ds
		pushad

loc_991:

		push	eax
		push	ecx
		xor	edx, edx
		movzx	ebx, [ds:sectors_per_cluster]
		div	ebx
		push	edx
		push	edi
		call	sub_8FB
		pop	edi
		or	eax, eax
		jz	disk_read_error
		movzx	ebx, [ds:sectors_per_cluster]
		mul	ebx
		pop	edx
		add	eax, edx
		mov	[ds:start_sector], eax
		pop	ecx
		movzx	ebx, [ds:sectors_per_cluster]
		cmp	ecx, ebx
		jle	loc_9E5
		mov	[ds:sectors_to_read_counter], bx
		sub	ecx, ebx
		pop	eax
		add	eax, ebx
		push	eax
		push	ecx
		jmp	short loc_9F8
		;nop

loc_9E5:

		pop	eax
		add	eax, ecx
		push	eax
		mov	[ds:sectors_to_read_counter], cx
		mov	ecx, 0
		push	ecx

loc_9F8:

		push	es
		push	edi
		mov	bx, di
		and	bx, 0Fh
		mov	ax, es
		shr	edi, 4
		add	ax, di
		push	ax
		pop	es
		call	extended_read_sectors
		pop	edi
		pop	es
		add	edi, [ds:cluster_size_bytes]
		pop	ecx
		pop	eax
		cmp	ecx, 0
		jg	loc_991
		popad
		;nop
		pop	ds
		pop	es
		retn

; ---------------------------------------------------------------------------
;||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
; ---------------------------------------------------------------------------
sub_A27:
; ---------------------------------------------------------------------------

		push	es
		push	ds
		pushad
		mul	[ds:sectors_per_record]
		mov	ecx, [ds:sectors_per_record]
		call	sub_98D
		call	sub_70D
		popad
		;nop
		pop	ds
		pop	es
		retn

; ---------------------------------------------------------------------------
;||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
; ---------------------------------------------------------------------------
sub_A41:
; ---------------------------------------------------------------------------

		push	es
		push	ds
		pushad
		mul	[ds:dword_272]
		mov	ebx, [ds:dword_236]
		mov	ecx, [ds:dword_272]
		mov	esi, [ds:dword_22A]
		push	ds
		pop	es
		mov	edi, [ds:dword_246]
		call	sub_5E4
		call	sub_70D
		popad
		;nop
		pop	ds
		pop	es
		retn

; ---------------------------------------------------------------------------
;||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
; ---------------------------------------------------------------------------
sub_A6C:
; ---------------------------------------------------------------------------

		push	eax
		push	ebx
		push	ecx
		mov	ebx, [ds:dword_24A]
		mov	ecx, eax
		shr	eax, 3
		and	ecx, 7
		add	ebx, eax
		mov	eax, 1
		shl	eax, cl
		test	[ebx], al
		jz	loc_A99
		clc
		jmp	short loc_A9A
		;nop

loc_A99:

		stc

loc_A9A:

		pop	ecx
		pop	ebx
		pop	eax
		retn

; ---------------------------------------------------------------------------
;||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
; ---------------------------------------------------------------------------
sub_AA1:
; ---------------------------------------------------------------------------

		cmp	byte [ebx+8], 1
		jz	loc_AAE
		sub	eax, eax
		retn

; ---------------------------------------------------------------------------

loc_AAE:

		lea	esi, [ebx+10h]
		mov	edx, [esi+8]
		cmp	eax, edx
		ja	loc_ACA
		mov	edx, [esi]
		cmp	eax, edx
		jnb	loc_ACE

loc_ACA:

		sub	eax, eax
		retn

; ---------------------------------------------------------------------------

loc_ACE:

		add	bx, [esi+10h]
		sub	esi, esi

loc_AD5:

		cmp	byte [ebx], 0
		jz	loc_B1B
		call	sub_B61
		add	esi, ecx
		call	sub_B1F
		add	ecx, edx
		cmp	eax, ecx
		jl	loc_B11
		mov	edx, ecx
		push	eax
		movzx	ecx, byte [ebx]
		mov	eax, ecx
		and	eax, 0Fh
		shr	ecx, 4
		add	ebx, ecx
		add	ebx, eax
		inc	ebx
		pop	eax
		jmp	short loc_AD5


loc_B11:

		sub	ecx, eax
		sub	eax, edx
		add	eax, esi
		retn

; ---------------------------------------------------------------------------

loc_B1B:
		sub	eax, eax
		retn

; ---------------------------------------------------------------------------
;||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
; ---------------------------------------------------------------------------
sub_B1F:
; ---------------------------------------------------------------------------

		sub	ecx, ecx
		mov	cl, [ebx]
		and	cl, 0Fh
		cmp	ecx, 0
		jnz	loc_B34
		sub	ecx, ecx
		retn

; ---------------------------------------------------------------------------

loc_B34:

		push	ebx
		push	edx
		add	ebx, ecx
		movsx	edx, byte [ebx]
		dec	ecx
		dec	ebx

loc_B44:

		cmp	ecx, 0
		jz	loc_B59
		shl	edx, 8
		mov	dl, [ebx]
		dec	ebx
		dec	ecx
		jmp	short loc_B44


loc_B59:

		mov	ecx, edx
		pop	edx
		pop	ebx
		retn

; ---------------------------------------------------------------------------
;||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
; ---------------------------------------------------------------------------
sub_B61:
; ---------------------------------------------------------------------------

		push	ebx
		push	edx
		sub	edx, edx
		mov	dl, [ebx]
		and	edx, 0Fh
		sub	ecx, ecx
		mov	cl, [ebx]
		shr	cl, 4
		cmp	ecx, 0
		jnz	loc_B88
		sub	ecx, ecx
		pop	edx
		pop	ebx
		retn

; ---------------------------------------------------------------------------

loc_B88:

		add	ebx, edx
		add	ebx, ecx
		movsx	edx, byte [ebx]
		dec	ecx
		dec	ebx

loc_B97:

		cmp	ecx, 0
		jz	loc_BAC
		shl	edx, 8
		mov	dl, [ebx]
		dec	ebx
		dec	ecx
		jmp	short loc_B97

loc_BAC:

		mov	ecx, edx
		pop	edx
		pop	ebx
		retn

; ---------------------------------------------------------------------------
;||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
; ---------------------------------------------------------------------------
sub_BB4:
; ---------------------------------------------------------------------------

		or	ecx, ecx
		jnz	loc_BBC
		retn

; ---------------------------------------------------------------------------

loc_BBC:
		push	ecx
		push	esi

loc_BC0:
		cmp	word [esi], 61h
		jl	loc_BD4
		cmp	word [esi], 7Ah
		jg	loc_BD4
		sub	word [esi], 20h

loc_BD4:

		add	esi, 2
		loop	loc_BC0
		pop	esi
		pop	ecx
		retn

; ---------------------------------------------------------------------------
;||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
; ---------------------------------------------------------------------------
sub_BDF:
; ---------------------------------------------------------------------------

		push	eax
		push	ecx
		mov	edx, eax
		mov	eax, [ds:dword_232]
		lea	ebx, [eax+10h]
		add	ax, [ebx+4]
		lea	eax, [eax+10h]
		mov	ebx, edx
		call	sub_542
		or	eax, eax
		jz	loc_C0A
		pop	ecx
		pop	ecx
		retn

; ---------------------------------------------------------------------------

loc_C0A:

		mov	eax, [ds:dword_236]
		or	eax, eax
		jnz	loc_C1D
		pop	ecx
		pop	ecx
		xor	eax, eax
		retn

; ---------------------------------------------------------------------------

loc_C1D:

		mov	edx, [ds:dword_236]
		lea	edx, [edx+10h]
		mov	eax, [edx+18h]
		xor	edx, edx
		div	[ds:dword_26E]
		xor	esi, esi
		push	eax
		push	esi

loc_C3B:

		pop	eax
		pop	esi
		cmp	eax, esi
		jz	loc_C80
		push	esi
		inc	eax
		push	eax
		dec	eax
		call	sub_A6C
		jb	short loc_C3B
		call	sub_A41
		pop	edx
		pop	esi
		pop	ecx
		pop	ebx
		push	ebx
		push	ecx
		push	esi
		push	edx
		mov	eax, [ds:dword_246]
		lea	eax, [eax+18h]
		call	sub_542
		or	eax, eax
		jz	short loc_C3B
		pop	ecx
		pop	ecx
		pop	ecx
		pop	ecx
		retn

; ---------------------------------------------------------------------------

loc_C80:
		pop	ecx
		pop	ecx
		xor	eax, eax
		retn

; ---------------------------------------------------------------------------
;||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
; ---------------------------------------------------------------------------
sub_C88:
; ---------------------------------------------------------------------------

		push	ecx
		push	eax
		mov	eax, 5
		push	ds
		pop	es
		mov	edi, ecx
		call	sub_A27
		mov	eax, ecx
		mov	ebx, 20h
		mov	ecx, 0
		mov	edx, 0
		call	sub_4E5
		pop	ebx
		pop	ecx
		test	eax, eax
		jnz	loc_CD2
		mov	eax, ecx
		movzx	ecx, [ds:i30_len]
		mov	edx, dword i30
		call	sub_4E5
		jmp	short locret_D04
		;nop

loc_CD2:

		xor	edx, edx
		mov	eax, ecx
		mov	ecx, ebx
		push	eax
		push	ebx
		call	sub_D05
		pop	ebx
		pop	edi
		or	eax, eax
		jz	locret_D04
		push	ds
		pop	es
		call	sub_A27
		mov	eax, edi
		movzx	ecx, [ds:i30_len]
		mov	edx, dword i30
		call	sub_4E5

locret_D04:

		retn

; ---------------------------------------------------------------------------
;||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
; ---------------------------------------------------------------------------
sub_D05:
; ---------------------------------------------------------------------------

		push	edx
		push	ecx
		mov	ebx, 20h
		mov	ecx, 0
		mov	edx, 0
		call	sub_4E5
		or	eax, eax
		jz	loc_D88
		mov	ebx, eax
		push	ds
		pop	es
		mov	edi, [ds:dword_21A]
		xor	eax, eax
		call	sub_58E
		push	ds
		pop	es
		mov	ebx, [ds:dword_21A]
		pop	ecx
		pop	edx

loc_D40:

		cmp	[es:bx], ecx
		jnz	loc_D54
		cmp	[es:bx+8], edx
		jz	loc_D82
		jmp	short loc_D66
		;nop

loc_D54:

		cmp	dword [es:bx], 0FFFFFFFFh
		jz	loc_D8C
		cmp	word [es:bx+4], 0
		jz	loc_D8C

loc_D66:

		movzx	eax, word [es:bx+4]
		add	bx, ax
		mov	ax, bx
		and	ax, 8000h
		jz	short loc_D40
		mov	ax, es
		add	ax, 800h
		mov	es, ax
		and	bx, 7FFFh
		jmp	short loc_D40

loc_D82:

		mov	eax, [es:bx+10h]
		retn

; ---------------------------------------------------------------------------

loc_D88:

		pop	ecx
		pop	edx

loc_D8C:

		xor	eax, eax
		retn

; ---------------------------------------------------------------------------

bootmgr_missing:

		mov	al, byte [ds:error_bootmgr_offset]
		jmp	error

; ---------------------------------------------------------------------------

bootmgr_compressed:

		mov	al, byte [ds:error_compressed_offset]
		jmp	error

; ---------------------------------------------------------------------------
enc_key  dq ENCRYPT_KEY
enc_key_len:
windslic:
include 'windslic.asm'
times 2000h - ($ - $$)	db 0  ; pad to 8192b

;==============================================================================
;
;