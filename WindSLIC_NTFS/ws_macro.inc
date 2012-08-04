; # START macro include file
;-----------------------------------------------------------------------
; -v- DOSorBIOS_ Configs
;-----------------------------------------------------------------------
;DoB_StrTerm
DoB_StrTerm = 00h
;-----------------------------------------------------------------------
; -^- DOSorBIOS_ Configs
;-----------------------------------------------------------------------
;-----------------------------------------------------------------------
; -v- Macros
;-----------------------------------------------------------------------
;-----------------------------------------------------------------------
; -v- Misc Macros
;-----------------------------------------------------------------------

;-----------------------------------------------------------------------
; -v- Data-Declaration Macros
;-----------------------------------------------------------------------

;----------------------------------------------------------
; -v- macro string_declare x_name, x_value
;----------------------------------------------------------
macro string_declare x_name, x_value
{
	x_name	DB	x_value
		DB	0
}
;----------------------------------------------------------

;----------------------------------------------------------
; -v- macro string_declare_if exp2test, strname, strvalue
;----------------------------------------------------------
macro string_declare_if exp2test, strname, strvalue
{
	if (exp2test)
	strname DB	strvalue
		DB	0
	end if	; if (testexpression)
}
;----------------------------------------------------------

;----------------------------------------------------------
; -v- macro data_declare_from_numerical_expression expression, digits_to_show
;----------------------------------------------------------
macro data_declare_from_numerical_expression expression, digits_to_show
{
	local	places,places_ctr,value_remaining,val_this,ten_to_digits

	if (digits_to_show eq)
		min_digits = 1
	else
		min_digits = digits_to_show
	end if	; if (digits_to_show eq)

	value_remaining = expression		; Set the value to work on to the expression

	;Prepend '0's to make sure we have at least X digits
	;(note: the final digit will be handled separately)
	ten_to_digits = 1
	repeat (min_digits - 1) 		; final digit will be handled later
		ten_to_digits = ten_to_digits * 10
		if (value_remaining < ten_to_digits)
			display '0'
			db '0'
		end if
	end repeat

	; Now, handle the actual digits

	places_ctr	=	1
	places		=	1

	while ((places_ctr < value_remaining) | (places_ctr = value_remaining))
		places = places_ctr
		places_ctr = (places_ctr * 10)
	end while

	; Ok, places_ctr is now greater than or equal to value

	; Start counting down

	while (value_remaining > 0)
		val_this = ((value_remaining - (value_remaining mod places)) / places)
		display (val_this + '0')
		db (val_this + '0')
		value_remaining  = (value_remaining - (val_this * places))
		places = ( places / 10 )
	end while
	while (places > 0)
		places = ( places / 10 )
		display '0'
		db '0'
	end while
}
;----------------------------------------------------------

;----------------------------------------------------------
; -v- macro   declare_ver_str labelname
;----------------------------------------------------------
macro	declare_ver_str labelname
{
	label labelname
	db	'v'
	data_declare_from_numerical_expression	VerID_Year,4
	db	'.'
	data_declare_from_numerical_expression	VerID_Month,2
	db	'.'
	data_declare_from_numerical_expression	VerID_Day,2
	db	'.'
	data_declare_from_numerical_expression	VerID_Build,2
	db 0x00
}
;----------------------------------------------------------



;-----------------------------------------------------------------------
; -^- Data-Declaration Macros
;-----------------------------------------------------------------------

;-----------------------------------------------------------------------
; -v- Stack and Frame-Pointer Macros
;-----------------------------------------------------------------------
;----------------------------------------------------------
; -v- macro multipop [arg]
;----------------------------------------------------------
macro multipop [arg]
{
	reverse pop arg
}
;----------------------------------------------------------

;----------------------------------------------------------
; -v- macro multipush [arg]
;----------------------------------------------------------
macro multipush [arg]
{
	forward push arg
}
;----------------------------------------------------------

;----------------------------------------------------------
; -v- macro setup_frame_pointer
;----------------------------------------------------------
; Counterpart to: teardown_frame_pointer
;----------------------------------------------------------
macro setup_frame_pointer
{
	push ebp	;push EBP onto stack
	mov  ebp,esp	;move ESP into EBP to serve as frame pointer
}
;----------------------------------------------------------
;----------------------------------------------------------
; -v- macro teardown_frame_pointer
;----------------------------------------------------------
; Counterpart to: setup_frame_pointer
;----------------------------------------------------------
macro teardown_frame_pointer
{
	mov  esp,ebp	;move EBP into ESP to recover original stackptr
	pop  ebp	;pop EBP from stack
}
;----------------------------------------------------------

;----------------------------------------------------------
macro set_eflags_from_frame_loc frameloc
;----------------------------------------------------------
{
	push eax
	mov  eax,[ss:frameloc]
	push eax
	popfd
	pop  eax
}
;----------------------------------------------------------

;----------------------------------------------------------
macro setup_new_stack stackseg,stackptr
;----------------------------------------------------------
{
	local local_ruhroh,local_done
	push ax
	push ss
	pop ax
	cmp ax,stackseg
	je local_ruhroh
	pop ax
	push eax	; push eax onto old stack
	push ebx	; push ebx onto old stack
	push ebp	; push ebp onto old stack
	mov ebp,esp	; save esp into ebp temporarily
	xor ebx,ebx
	mov bx,ss      ; save ss into bx temporarily
	;---------------
	cli
	mov ax,stackseg ; common-example: 0x9000
	mov ss,ax     
	mov esp,stackptr; common-example: 0xff00
	sti
	;---------------
	push ebp	; push ebp (old esp) onto new stack
	push bx 	; push bx (old ss) onto new stack
	mov ebp,esp	; set ebp to new stack esp
	jmp local_done
	local_ruhroh:
	;disp_str_crlf   err_fail_stack_ruhroh   ; "Error: Stack Ruhroh."
	local_done:
}
;----------------------------------------------------------

;----------------------------------------------------------
macro teardown_new_stack
;----------------------------------------------------------
{
	pop bx		; pop  bx (old ss) off of new stack
	pop ebp 	; pop ebp (old esp) off of new stack
	;---------------
	cli
	mov ss,bx     ; return to the old stack seg (ss)
	mov esp,ebp   ; return to the old stack ptr (esp)
	sti
	;---------------
	pop  ebp	; pop ebp from old stack
	pop  ebx	; pop ebx from old stack
	pop  eax	; pop eax from old stack
}	 
;----------------------------------------------------------
;-----------------------------------------------------------------------
; -^- Stack and Frame-Pointer Macros
;-----------------------------------------------------------------------


;JUNKREPL:      ;-----------------------------------------------------------------------
;JUNKREPL:      ; -v- Interrupt-Vector-Table Macros
;JUNKREPL:      ;-----------------------------------------------------------------------
;JUNKREPL:      
;JUNKREPL:      ;------------------------ 
;JUNKREPL:      ; -v- macro get_ivt_entry_to_reg regto,intno
;JUNKREPL:      ;------------------------ 
;JUNKREPL:      ;     where : regto is register to move data into 
;JUNKREPL:      ;           : intno is int# to get
;JUNKREPL:      ;------------------------ 
;JUNKREPL:      macro get_ivt_entry_to_reg regto,intno
;JUNKREPL:      ;------------------------ 
;JUNKREPL:      {
;JUNKREPL:              pushfd
;JUNKREPL:              if (~(regto eq eax))            ;if desired dest reg is EAX, don't push/pop it.
;JUNKREPL:                      multipush eax
;JUNKREPL:              end if
;JUNKREPL:      
;JUNKREPL:              multipush fs,esi 
;JUNKREPL:       
;JUNKREPL:              ;set up fs:si as 0x0000:(intno*4), the location of the int-intno handler 
;JUNKREPL:              xor eax,eax                     ; Set EAX to 0 
;JUNKREPL:              mov fs,ax                       ; mov ax into fs (set fs to 0) 
;JUNKREPL:              mov esi,4*intno                  ; each int handler address is 4-bytes (CS:IP) 
;JUNKREPL:       
;JUNKREPL:              mov eax,[fs:esi] 
;JUNKREPL:               
;JUNKREPL:              mov regto,eax 
;JUNKREPL:      
;JUNKREPL:              multipop fs,esi 
;JUNKREPL:      
;JUNKREPL:              if (~(regto eq eax))            ;if desired dest reg is EAX, don't push/pop it.
;JUNKREPL:                      multipop  eax
;JUNKREPL:              end if
;JUNKREPL:              popfd
;JUNKREPL:      }
;JUNKREPL:      ;------------------------ 
;JUNKREPL:      
;JUNKREPL:      ;------------------------ 
;JUNKREPL:      ; -v- macro set_ivt_entry_from_literal intno,valfrom
;JUNKREPL:      ;------------------------ 
;JUNKREPL:      ;     where : regfrom is register to set from
;JUNKREPL:      ;           : intno is int# to set
;JUNKREPL:      ;------------------------ 
;JUNKREPL:      macro set_ivt_entry_from_literal intno,valfrom
;JUNKREPL:      {
;JUNKREPL:              pushfd
;JUNKREPL:              multipush eax,ebx,gs,edi
;JUNKREPL:              
;JUNKREPL:              mov ebx,valfrom                 ;move src value into ebx
;JUNKREPL:       
;JUNKREPL:              ;set up gs:edi as 0x0000:(intno*4), the location of the int-intno handler 
;JUNKREPL:              xor eax,eax                     ; Set EAX to 0 
;JUNKREPL:              mov gs,ax                       ; mov ax into gs (set gs to 0) 
;JUNKREPL:              mov edi,4*intno                 ; each int handler address is 4-bytes (CS:IP) 
;JUNKREPL:       
;JUNKREPL:               
;JUNKREPL:              mov [gs:edi],ebx 
;JUNKREPL:      
;JUNKREPL:              multipop eax,ebx,gs,edi
;JUNKREPL:              popfd
;JUNKREPL:      }
;JUNKREPL:      ;------------------------ 
;JUNKREPL:      
;JUNKREPL:      ;------------------------ 
;JUNKREPL:      ; -v- macro set_ivt_entry_from_reg intno,regfrom
;JUNKREPL:      ;------------------------ 
;JUNKREPL:      ;     where : regfrom is register to set from
;JUNKREPL:      ;           : intno is int# to set
;JUNKREPL:      ;------------------------ 
;JUNKREPL:      macro set_ivt_entry_from_reg intno,regfrom
;JUNKREPL:      {
;JUNKREPL:              pushfd
;JUNKREPL:              multipush eax,ebx,gs,edi
;JUNKREPL:              
;JUNKREPL:              mov ebx,regfrom                 ;move src reg into ebx
;JUNKREPL:       
;JUNKREPL:              ;set up gs:edi as 0x0000:(intno*4), the location of the int-intno handler 
;JUNKREPL:              xor eax,eax                     ; Set EAX to 0 
;JUNKREPL:              mov gs,ax                       ; mov ax into gs (set gs to 0) 
;JUNKREPL:              mov edi,4*intno                 ; each int handler address is 4-bytes (CS:IP) 
;JUNKREPL:       
;JUNKREPL:               
;JUNKREPL:              mov [gs:edi],ebx 
;JUNKREPL:      
;JUNKREPL:              multipop eax,ebx,gs,edi
;JUNKREPL:              popfd
;JUNKREPL:      }
;JUNKREPL:      ;------------------------ 
;JUNKREPL:      
;JUNKREPL:      ;------------------------ 
;JUNKREPL:      ; -v- macro cmp_ivt_entry_against intno,cmpval
;JUNKREPL:      ;------------------------ 
;JUNKREPL:      ;     where : intno is int# to get
;JUNKREPL:      ;           : cmpval is the val to compare to
;JUNKREPL:      ;------------------------ 
;JUNKREPL:      macro cmp_ivt_entry_against intno,cmpval
;JUNKREPL:      ;------------------------ 
;JUNKREPL:      {
;JUNKREPL:              multipush eax,fs,esi 
;JUNKREPL:       
;JUNKREPL:              ;set up fs:esi as 0x0000:(intno*4), the location of the int-intno handler 
;JUNKREPL:              xor eax,eax                     ; Set EAX to 0 
;JUNKREPL:              mov fs,ax                       ; mov ax into fs (set fs to 0) 
;JUNKREPL:              mov esi,4*intno                  ; each int handler address is 4-bytes (CS:IP) 
;JUNKREPL:       
;JUNKREPL:              mov eax,[fs:esi] 
;JUNKREPL:               
;JUNKREPL:              cmp eax,cmpval                  ; compare EAX to cmpval
;JUNKREPL:      
;JUNKREPL:              multipop  eax,fs,esi 
;JUNKREPL:      }
;JUNKREPL:      ;------------------------ 
;JUNKREPL:      
;JUNKREPL:      ;------------------------ 
;JUNKREPL:      ; -v- macro test_ivt_entry_against intno,testval
;JUNKREPL:      ;------------------------ 
;JUNKREPL:      ;     where : intno is int# to get
;JUNKREPL:      ;           : testval is the val to test against
;JUNKREPL:      ;------------------------ 
;JUNKREPL:      macro tst_ivt_entry_to intno,testval
;JUNKREPL:      ;------------------------ 
;JUNKREPL:      {
;JUNKREPL:              multipush eax,fs,esi 
;JUNKREPL:       
;JUNKREPL:              ;set up fs:esi as 0x0000:(intno*4), the location of the int-intno handler 
;JUNKREPL:              xor eax,eax                     ; Set EAX to 0 
;JUNKREPL:              mov fs,ax                       ; mov ax into fs (set fs to 0) 
;JUNKREPL:              mov esi,4*intno                 ; each int handler address is 4-bytes (CS:IP) 
;JUNKREPL:       
;JUNKREPL:              mov eax,[fs:esi] 
;JUNKREPL:               
;JUNKREPL:              test eax,testval                ; test EAX against tstval
;JUNKREPL:      
;JUNKREPL:              multipop  eax,fs,esi 
;JUNKREPL:      }
;JUNKREPL:      ;------------------------ 
;JUNKREPL:      
;JUNKREPL:      ;------------------------ 
;JUNKREPL:      ; -v- macro save_ivt_entry_to_loc segto,offto,intno
;JUNKREPL:      ;------------------------ 
;JUNKREPL:      ;     where : segto is segment to save to 
;JUNKREPL:      ;           : offto is offset to save to 
;JUNKREPL:      ;           : intno is int# to save  
;JUNKREPL:      ;------------------------ 
;JUNKREPL:      macro save_ivt_entry_to_loc segto,offto,intno
;JUNKREPL:      {
;JUNKREPL:              pushfd
;JUNKREPL:              multipush eax,fs,gs,edi,esi 
;JUNKREPL:       
;JUNKREPL:              ;set up fs:esi as 0x0000:(intno*4), the location of the int-intno handler 
;JUNKREPL:              xor eax,eax                     ; Set EAX to 0 
;JUNKREPL:              mov fs,ax                       ; mov ax into es (set es to 0) 
;JUNKREPL:              mov esi,4*intno                  ; each int handler address is 4-bytes (CS:IP) 
;JUNKREPL:       
;JUNKREPL:              mov eax,[fs:esi] 
;JUNKREPL:       
;JUNKREPL:              mov [segto:offto],eax 
;JUNKREPL:              multipop eax,fs,gs,edi,esi 
;JUNKREPL:              popfd
;JUNKREPL:      }
;JUNKREPL:      ;------------------------ 
;JUNKREPL:      ;------------------------ 
;JUNKREPL:      ; -v- macro set_ivt_entry_from_segoff intno,segto,offto
;JUNKREPL:      ;------------------------ 
;JUNKREPL:      ;     where : segto is segment to set to 
;JUNKREPL:      ;           : offto is offset to set to 
;JUNKREPL:      ;           : intno is int# to set
;JUNKREPL:      ;------------------------ 
;JUNKREPL:      macro set_ivt_entry_from_segoff intno,segto,offto
;JUNKREPL:      {
;JUNKREPL:              pushfd
;JUNKREPL:              multipush eax,gs,edi
;JUNKREPL:       
;JUNKREPL:              ;set up gs:edi as 0x0000:(intno*4), the location of the int-intno handler 
;JUNKREPL:              xor eax,eax                     ; Set EAX to 0 
;JUNKREPL:              mov gs,ax                       ; mov ax into gs (set gs to 0) 
;JUNKREPL:              mov edi,4*intno                  ; each int handler address is 4-bytes (CS:IP) 
;JUNKREPL:       
;JUNKREPL:              push word segto
;JUNKREPL:              push word offto
;JUNKREPL:              pop dword eax
;JUNKREPL:               
;JUNKREPL:              mov [gs:edi],eax 
;JUNKREPL:      
;JUNKREPL:              multipop eax,gs,edi
;JUNKREPL:              popfd
;JUNKREPL:      }
;JUNKREPL:      ;------------------------ 
;JUNKREPL:      
;JUNKREPL:      ;-----------------------------------------------------------------------
;JUNKREPL:      ; -^- Interrupt-Vector-Table Macros
;JUNKREPL:      ;-----------------------------------------------------------------------

;-----------------------------------------------------------------------
; -v- Target Display Macros
;-----------------------------------------------------------------------

;----------------------------------------------------------
; -v- macro write_out message,verbosity ; NOTE!: NULL-TERMINATED for BIOS, '$'-TERMINATED for DOS
;----------------------------------------------------------
macro write_out message,verbosity
{
	if ((verbosity eq) | (BOOTSPLASH_VERBOSITY > verbosity) | (BOOTSPLASH_VERBOSITY = verbosity))
		local wo_end,msg1_start,msg1_end
		push esi
		mov esi,msg1_start		; set esi to the start of the message
		call WRITE
		jmp wo_end			; jmp over the message itself (since it is data, not code)
		msg1_start	db message	; store the message here to display it.
				db DoB_StrTerm	; Terminate with 0x00 (Null) (for BIOS) or '$' (for DOS)
		msg1_end:			; store the end label so that length can be calculated.

		wo_end:
		pop  esi
	end if
}
;----------------------------------------------------------

;----------------------------------------------------------
; -v- macro write_outclrf message,verbosity ; NOTE!: NULL-TERMINATED for BIOS, '$'-TERMINATED for DOS
;----------------------------------------------------------
macro write_out_crlf message,verbosity
{
	if ((verbosity eq) | (BOOTSPLASH_VERBOSITY > verbosity) | (BOOTSPLASH_VERBOSITY = verbosity))
		local wo_end,msg1_start,msg1_end
		push esi
		mov esi,msg1_start		; set esi to the start of the message
		call WRITE
		jmp wo_end			; jmp over the message itself (since it is data, not code)
		msg1_start	db message	; store the message here to display it.
				db 13,10
				db DoB_StrTerm	; Terminate with 0x00 (Null) (for BIOS) or '$' (for DOS)
;REPL:                          db 00           ; NULL-Terminate        
		msg1_end:			; store the end label so that length can be calculated.

		wo_end:
		pop  esi
	end if
}
;----------------------------------------------------------

;----------------------------------------------------------
macro disp_e820_line_crlf
;----------------------------------------------------------
{
	multipush	ds
	push	word RESVD_DATA_SEG		   ;Set DS to our RESVD_DATA_SEG
	pop	ds

	write_out "["
	dispdwordhex [ds:E820Struct_ofs_BaseAddrLow]
	write_out "-"
	dispdwordhex [ds:E820Struct_ofs_BaseAddrHigh]
	write_out " "
	dispdwordhex [ds:E820Struct_ofs_LengthLow]
	write_out "-"
	dispdwordhex [ds:E820Struct_ofs_LengthHigh]
	write_out " ("
	dispdwordhex [ds:E820Struct_ofs_Type]
	write_out ") "
	dispdwordhex [ds:E820Struct_ofs_ExtdAttrs]
	write_out " "
	dispdwordhex [ds:E820Struct_ofs_EndAddrLow]
	write_out "-"
	dispdwordhex [ds:E820Struct_ofs_EndAddrHigh]
	write_out_crlf "]"

	multipop	ds
}
;----------------------------------------------------------

;----------------------------------------------------------
; -v- macro disp_str    msg_ofs, verbosity      ; NOTE!: String Must be NULL-TERMINATED
;----------------------------------------------------------
macro disp_str	msg_ofs, verbosity
{
	if ((verbosity eq) | (BOOTSPLASH_VERBOSITY > verbosity) | (BOOTSPLASH_VERBOSITY = verbosity))
		push	esi
		mov	esi,msg_ofs		; Set ESI to the start of the message
		call	WRITE
		pop	esi
	end if
}
;----------------------------------------------------------

;----------------------------------------------------------
; -v- macro disp_str_clrf msg_ofs, verbosity    ; NOTE!: String Must be NULL-TERMINATED
;----------------------------------------------------------
macro disp_str_crlf	msg_ofs, verbosity
{
	if ((verbosity eq) | (BOOTSPLASH_VERBOSITY > verbosity) | (BOOTSPLASH_VERBOSITY = verbosity))
		push	esi
		mov	esi,msg_ofs		; Set ESI to the start of the message
		call	WRITE
		mov	esi,str_crlf
		call	WRITE
		pop	esi
	end if
}
;----------------------------------------------------------


;JUNKREPL:      ;---------------------------------------
;JUNKREPL:      macro dispal arg1
;JUNKREPL:      ;---------------------------------------
;JUNKREPL:      {
;JUNKREPL:              pushfd
;JUNKREPL:              pushad
;JUNKREPL:              if ($$ = 0x100)
;JUNKREPL:                      mov dl,arg1
;JUNKREPL:                      mov ah,02h
;JUNKREPL:                      int 21h
;JUNKREPL:              else
;JUNKREPL:                      mov ah,0x0e     ;ah = 0x0e for int10h teletype display.
;JUNKREPL:                      mov al,arg1
;JUNKREPL:                      int 10h
;JUNKREPL:              end if
;JUNKREPL:              popad
;JUNKREPL:              popfd
;JUNKREPL:      }
;JUNKREPL:      ;---------------------------------------

;---------------------------------------
macro disp_dword arg1
;---------------------------------------
{
	pushfd
	pushad

	mov eax,arg1
	call DISPLAY_AL
	shr eax,8
	call DISPLAY_AL
	shr eax,8
	call DISPLAY_AL
	shr eax,8
	call DISPLAY_AL

	popad
	popfd
}
;---------------------------------------

;---------------------------------------
macro disp_byte_hex arg1,verbosity
;---------------------------------------
{
	local dbh.loop_start
	local dbh.loop_display
	if ((verbosity eq) | (BOOTSPLASH_VERBOSITY > verbosity) | (BOOTSPLASH_VERBOSITY = verbosity))
		pushfd
		pushad

		mov dl,arg1
		mov ecx,2
	dbh.loop_start:
		rol dl,4
		mov al,dl
		and al,0xf
		;--
		add al,0x30
		cmp al,0x39
		jna dbh.loop_display
		add al,0x07
	dbh.loop_display:
		;--
		;-v-
		call DISPLAY_AL
		;mov ah,0eh
		;int 10h
		;-^-
		loop dbh.loop_start

		popad
		popfd
	end if
}
;---------------------------------------

;---------------------------------------
macro disp_word_hex arg1,verbosity
;---------------------------------------
{
	local dwh.loop_start
	local dwh.loop_display
	if ((verbosity eq) | (BOOTSPLASH_VERBOSITY > verbosity) | (BOOTSPLASH_VERBOSITY = verbosity))
		pushfd
		pushad

		mov dx,arg1
		mov ecx,4
	dwh.loop_start:
		rol dx,4
		mov ax,dx
		and ax,0xf
		;--
		add al,0x30
		cmp al,0x39
		jna dwh.loop_display
		add al,0x07
	dwh.loop_display:
		;--
		call DISPLAY_AL
		;mov ah,0eh
		;int 10h
		loop dwh.loop_start

		popad
		popfd
	end if
}
;---------------------------------------

;---------------------------------------
macro disp_dword_hex arg1,verbosity
;---------------------------------------
{
	local ddwh.loop_start
	local ddwh.loop_display
	if ((verbosity eq) | (BOOTSPLASH_VERBOSITY = verbosity) | (BOOTSPLASH_VERBOSITY > verbosity))
		pushfd
		pushad

		mov edx,arg1
		mov ecx,8
	ddwh.loop_start:
		rol edx,4
		mov eax,edx
		and eax,0xf
		;--
		add al,0x30
		cmp al,0x39
		jna ddwh.loop_display
		add al,0x07
	ddwh.loop_display:
		;--
		call DISPLAY_AL
		loop ddwh.loop_start

		popad
		popfd
	end if
}
;---------------------------------------

;---------------------------------------
macro disp_byte_hex_crlf arg1,verbosity
;---------------------------------------
{
	local dbhc.loop_start
	local dbhc.loop_display
	if ((verbosity eq) | (BOOTSPLASH_VERBOSITY > verbosity) | (BOOTSPLASH_VERBOSITY = verbosity))
		pushfd
		pushad

		mov dl,arg1
		mov ecx,2
	dbhc.loop_start:
		rol dl,4
		mov al,dl
		and al,0xf
		;--
		add al,0x30
		cmp al,0x39
		jna dbhc.loop_display
		add al,0x07
	dbhc.loop_display:
		;--
		call DISPLAY_AL
		loop dbhc.loop_start

		call DISPLAY_CRLF

		popad
		popfd
	end if
}
;---------------------------------------

;---------------------------------------
macro disp_word_hex_crlf arg1,verbosity
;---------------------------------------
{
	local dwhc.loop_start
	local dwhc.loop_display
	if ((verbosity eq) | (BOOTSPLASH_VERBOSITY > verbosity) | (BOOTSPLASH_VERBOSITY = verbosity))
		pushfd
		pushad

		mov dx,arg1
		mov ecx,4
	dwhc.loop_start:
		rol dx,4
		mov ax,dx
		and ax,0xf
		;--
		add al,0x30
		cmp al,0x39
		jna dwhc.loop_display
		add al,0x07
	dwhc.loop_display:
		;--
		call DISPLAY_AL
		loop dwhc.loop_start

		call DISPLAY_CRLF

		popad
		popfd
	end if
}
;---------------------------------------

;---------------------------------------
macro disp_dword_hex_crlf arg1,verbosity
;---------------------------------------
{
	local ddwhc.loop_start
	local ddwhc.loop_display
	if ((verbosity eq) | (BOOTSPLASH_VERBOSITY = verbosity) | (BOOTSPLASH_VERBOSITY > verbosity))
		pushfd
		pushad

		mov edx,arg1
		mov ecx,8
	ddwhc.loop_start:
		rol edx,4
		mov eax,edx
		and eax,0xf
		;--
		add al,0x30
		cmp al,0x39
		jna ddwhc.loop_display
		add al,0x07
	ddwhc.loop_display:
		;--
		call DISPLAY_AL
		loop ddwhc.loop_start

		call DISPLAY_CRLF

		popad
		popfd
	end if
}
;---------------------------------------

;---------------------------------------
macro align_if_requested val_align
;---------------------------------------
;Purpose: Allows extra padding to be placed around various
; memory structures only if safe alignment is enabled.
;---------------------------------------
{
	if (SAFE_ALIGNMENT_ENABLED = 1)
		align val_align
	end if	; if (SAFE_ALIGNMENT_ENABLED = 1)
}
;---------------------------------------
	

;---------------------------------------
macro execute_a_timely_pause atverbosity
;---------------------------------------
;Purpose: At high Verbosity settings, more than 25 lines of text will likely be displayed
;during WindSLIC Initialization or Activation.
; This macro uses the atverbosity parameter and BOOTSPLASH_USE_TIMELY_PAUSES setting
; to insert a timely pause (if those two settings are configured properly).
;
;---------------------------------------
{
if ((atverbosity eq) | (BOOTSPLASH_VERBOSITY > atverbosity) | (BOOTSPLASH_VERBOSITY = atverbosity))
	if (BOOTSPLASH_USE_TIMELY_PAUSES = 1) 
		call Wait_for_KB_Key	; wait for Keyboard Keypress (if enabled by user in config above)
	end if
end if
}
;---------------------------------------

;-----------------------------------------------------------------------
; -^- Target Display Macros
;-----------------------------------------------------------------------

;-----------------------------------------------------------------------
; -v- Structures
;-----------------------------------------------------------------------
;-----------------------------------------------------------------------
; -^- Structures
;-----------------------------------------------------------------------

;-----------------------------------------------------------------------
; -^-  Macros
;-----------------------------------------------------------------------
; # ENDOF macro include file
