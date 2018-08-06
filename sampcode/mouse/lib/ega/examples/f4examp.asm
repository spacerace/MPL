; The following example saves the contents of the Miscellaneous Output
; register, Sequencer Memory Mode register, and CRT Controller Mode Control
; register in "results".

outvals	dw	0020h		; miscellaneous output register
	db	0		; 0 for single registers
	db	?		; returned value

	dw	0008h		; sequencer
	db	04h		; memory mode register index
	db	?		; returned value

	dw	0000h		; crt controller
	db	17h		; mode control register index
	db	?		; returned value

results	db	3 dup (?)

	mov	ax, ds			; assume outvals in data segment
	mov	es, ax			; es = data segment
	mov	bx, offset outvals	; es:bx = outvals address
	mov	ah, 0f4h		; f4 = read register set
	mov	cx, 3			; number of entries in outvals
	int	10h			; get values into outvals!
	mov	si, 3			; move the returned values from
	add	si, offset outvals	;  outvals
	mov	di, offset results	;  to results
	mov	cx, 3			; 3 values to move

movloop:
	mov	ax, [si]		; move one value from outvals
	mov	[di], ax		;  to results
	add	si, 4			; skip to next source byte
	inc	di			; point to next destination byte
	loop	movloop
