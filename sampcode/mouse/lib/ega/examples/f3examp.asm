; The following example writes the contents of "cursloc" into the CRT
; Controller Cursor Location High and Cursor Location Low registers.

cursloc	db	01h, 00h

	mov	ax, ds			; assume cursloc in data segment
	mov	es, ax			; es = data segment
	mov	bx, offset cursloc	; es:bx = cursloc address
	mov	ah, 0f3h		; f2 = write register range
	mov	cx, 0e02h		; ch = start index of 14
					; cl = 2 registers to write
	mov	dx, 0000h		; dx = crt controller
	int	10h			; write them!
