;-----------------------------------------------------------------------
Check_KB_Input: 		;check the keyboard input keys, and associated processing 
				;entry: none; Export: No
;-----------------------------------------------------------------------

	pushad
	mov ah,BOOTSPLASH_WAIT	;this value is multiplied by 55ms
	call DELAY
	call Test_KB_Code
	popad
	retn
;-----------------------------------------------------------------------

;-----------------------------------------------------------------------
DELAY:				; to achieve the delay function
				; entry: ah; export: No
;-----------------------------------------------------------------------
	pushfd			
	push eax
	push es 		;the delay length of time for the AH * 55 ms
	sti			;pause, put number of 55ms idle ticks in AH
	push word 00h		;enable interrupts otherwise timer will not be updated
	pop es			;00=segment of timer
	add ah,[es:046ch]	;add ticks to required delay
  @@:
	cmp ah,[es:046ch]	;compare current and required value
	jne @b			;if not then repeat
	pop es			;The DELAY code is copied from ROMOS
	pop eax
	popfd
	retn
;-----------------------------------------------------------------------

;-----------------------------------------------------------------------
Test_KB_Code:
;-----------------------------------------------------------------------
        pushad
        mov ah,01h
        int 16h
        cmp ah, BOOTSPLASH_SKIP_KEYCODE
        je  Is_The_Key
IsNot_The_Key:
        clc
        jmp Test_KB_Code_done
Is_The_Key:
        stc
        jmp Test_KB_Code_done
Test_KB_Code_done:
        popad
        retn
;-----------------------------------------------------------------------