;-------------------------------------------------------------------------------

return_boot:

	popad
	mov	ax, ds
	pop	bx
	push	bx
	sub	ax, bx
	shl	ax, 4
	sub	ax, 3D28h
	push	ax
	lgdt	fword [ds:SavedGDT]
	retf

;-----------------------------------------------------------------------