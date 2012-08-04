;-------------------------------------------------------------------------------

 getboot_boot:

	call	$+3
	pop	bx
	sub	bx, getboot_boot
	shr	bx, 4
	mov	ax, cs
	add	bx, ax
	push	cs
	pushad
	push	bx
	mov	ds, bx
	push	BEV
	sgdt	fword [ds:SavedGDT]
	retf

SavedGDT df 0
;-------------------------------------------------------------------------------