; The following example writes the contents of "outvals" to the Miscellaneous
; Output register, Sequencer Memory Mode register, and CRT Controller Mode
; Control register.

outvals	dw	0020h		; miscellaneous output register
	db	0		; 0 for single registers
	db	0a7h		; output value

	dw	0008h		; sequencer
	db	04h		; memory mode register index
	db	03h		; output value

	dw	0000h		; crt controller
	db	17h		; mode control register index
	db	0a3h		; output value

	mov	ax, ds			; assume outvals in data segment
	mov	es, ax			; es = data segment
	mov	bx, offset outvals	; es:bx = outvals address
	mov	ah, 0f5h		; f5 = write register set
	mov	cx, 3			; number of entries in outvals
	int	10h			; write the registers!
