;------------------------------------------------------------------------
;ws_mcivt.inc
;------------------------------------------------------------------------
;-----------------------------------------------------------------------
; -v- Interrupt-Vector-Table Macros
;-----------------------------------------------------------------------

;------------------------ 
; -v- macro get_ivt_entry_to_reg regto,intno
;------------------------ 
;     where : regto is register to move data into 
;           : intno is int# to get
;------------------------ 
macro get_ivt_entry_to_reg regto,intno
;------------------------ 
{
	pushfd
	if (~(regto eq eax))		;if desired dest reg is EAX, don't push/pop it.
		multipush eax
	end if

	multipush fs,esi 
 
	;set up fs:si as 0x0000:(intno*4), the location of the int-intno handler 
	xor eax,eax			; Set EAX to 0 
	mov fs,ax			; mov ax into fs (set fs to 0) 
	mov esi,4*intno 		 ; each int handler address is 4-bytes (CS:IP) 
 
	mov eax,[fs:esi] 
	 
	mov regto,eax 

	multipop fs,esi 

	if (~(regto eq eax))		;if desired dest reg is EAX, don't push/pop it.
		multipop  eax
	end if
	popfd
}
;------------------------ 

;------------------------ 
; -v- macro set_ivt_entry_from_literal intno,valfrom
;------------------------ 
;     where : regfrom is register to set from
;           : intno is int# to set
;------------------------ 
macro set_ivt_entry_from_literal intno,valfrom
{
	pushfd
	multipush eax,ebx,gs,edi
	
	mov ebx,valfrom 		;move src value into ebx
 
	;set up gs:edi as 0x0000:(intno*4), the location of the int-intno handler 
	xor eax,eax			; Set EAX to 0 
	mov gs,ax			; mov ax into gs (set gs to 0) 
	mov edi,4*intno 		; each int handler address is 4-bytes (CS:IP) 
 
	 
	mov [gs:edi],ebx 

	multipop eax,ebx,gs,edi
	popfd
}
;------------------------ 

;------------------------ 
; -v- macro set_ivt_entry_from_reg intno,regfrom
;------------------------ 
;     where : regfrom is register to set from
;           : intno is int# to set
;------------------------ 
macro set_ivt_entry_from_reg intno,regfrom
{
	pushfd
	multipush eax,ebx,gs,edi
	
	mov ebx,regfrom 		;move src reg into ebx
 
	;set up gs:edi as 0x0000:(intno*4), the location of the int-intno handler 
	xor eax,eax			; Set EAX to 0 
	mov gs,ax			; mov ax into gs (set gs to 0) 
	mov edi,4*intno 		; each int handler address is 4-bytes (CS:IP) 
 
	 
	mov [gs:edi],ebx 

	multipop eax,ebx,gs,edi
	popfd
}
;------------------------ 

;------------------------ 
; -v- macro cmp_ivt_entry_against intno,cmpval
;------------------------ 
;     where : intno is int# to get
;           : cmpval is the val to compare to
;------------------------ 
macro cmp_ivt_entry_against intno,cmpval
;------------------------ 
{
	multipush eax,fs,esi 
 
	;set up fs:esi as 0x0000:(intno*4), the location of the int-intno handler 
	xor eax,eax			; Set EAX to 0 
	mov fs,ax			; mov ax into fs (set fs to 0) 
	mov esi,4*intno 		 ; each int handler address is 4-bytes (CS:IP) 
 
	mov eax,[fs:esi] 
	 
	cmp eax,cmpval			; compare EAX to cmpval

	multipop  eax,fs,esi 
}
;------------------------ 

;------------------------ 
; -v- macro test_ivt_entry_against intno,testval
;------------------------ 
;     where : intno is int# to get
;           : testval is the val to test against
;------------------------ 
macro tst_ivt_entry_to intno,testval
;------------------------ 
{
	multipush eax,fs,esi 
 
	;set up fs:esi as 0x0000:(intno*4), the location of the int-intno handler 
	xor eax,eax			; Set EAX to 0 
	mov fs,ax			; mov ax into fs (set fs to 0) 
	mov esi,4*intno 		; each int handler address is 4-bytes (CS:IP) 
 
	mov eax,[fs:esi] 
	 
	test eax,testval		; test EAX against tstval

	multipop  eax,fs,esi 
}
;------------------------ 

;------------------------ 
; -v- macro get_ivt_entry_to_loc segto,offto,intno
;------------------------ 
;     where : segto is segment to save to 
;           : offto is offset to save to 
;           : intno is int# to save  
;------------------------ 
macro get_ivt_entry_to_loc segto,offto,intno
{
	pushfd
	multipush eax,fs,gs,edi,esi 
 
	;set up fs:esi as 0x0000:(intno*4), the location of the int-intno handler 
	xor eax,eax			; Set EAX to 0 
	mov fs,ax			; mov ax into es (set es to 0) 
	mov esi,4*intno 		 ; each int handler address is 4-bytes (CS:IP) 
 
	mov eax,[fs:esi] 
 
	mov [segto:offto],eax 
	multipop eax,fs,gs,edi,esi 
	popfd
}
;------------------------ 
;------------------------ 
; -v- macro set_ivt_entry_from_segoff intno,segto,offto
;------------------------ 
;     where : segto is segment to set to 
;           : offto is offset to set to 
;           : intno is int# to set
;------------------------ 
macro set_ivt_entry_from_segoff intno,segto,offto
{
	pushfd
	multipush eax,fs,edi
 
	;set up gs:edi as 0x0000:(intno*4), the location of the int-intno handler 
	xor eax,eax			; Set EAX to 0 
	mov fs,ax			; mov ax into gs (set gs to 0)
	mov edi,4*intno 		; each int handler address is 4-bytes (CS:IP)
	get_ivt_entry_to_reg eax,ENTRY_HOLD_RSDP_ADDR
	mov [fs:edi],eax

	multipop eax,fs,edi
	popfd
}
;------------------------ 

;-----------------------------------------------------------------------
; -^- Interrupt-Vector-Table Macros
;-----------------------------------------------------------------------

;------------------------------------------------------------------------
;eof - ws_mcivt.inc
