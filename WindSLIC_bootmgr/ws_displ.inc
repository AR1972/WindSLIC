;-----------------------------------------------------------------------
WRITE:				;CS:ESI points to the null-terminated string to display
				;entry: esi; export: No
;-----------------------------------------------------------------------
	pushfd
	pushad
  WRITE_1:
	mov al,[cs:esi]
	cmp al,0h
	je WRITE_2
	call DISPLAY_AL
	inc esi
	jmp WRITE_1
  WRITE_2:
	popad
	popfd
	retn
;-----------------------------------------------------------------------

;-----------------------------------------------------------------------
DISPLAY_AL:			;the al content, as a character, and directly displayed.
				;entry: al; export: No
;-----------------------------------------------------------------------

	pushfd
	pushad
	mov ah,0eh		;AH=0xE,to write characters to the cursor, the cursor into a
	xor bl,bl		;foreground color 0 (black)
	int 10h
	popad
	popfd
	retn
;-----------------------------------------------------------------------

;-----------------------------------------------------------------------
SET_TEXT_MODE:			;entry: none; export: No
;-----------------------------------------------------------------------
	pushfd
	pushad		
	mov ah,00h
	mov al,03h
	int 10h
	popad
	popfd
	retn
;-----------------------------------------------------------------------
