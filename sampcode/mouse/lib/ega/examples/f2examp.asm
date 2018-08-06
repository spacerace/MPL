; The following example saves the contents of the Attribute Controller
; Palette registers in "paltable".

paltable db	16 dup (?)

	mov	ax, ds			; assume paltable in data segment
	mov	es, ax			; es = data segment
	mov	bx, offset paltable	; es:bx = paltable address
	mov	ah, 0f2h		; f2 = read register range
	mov	cx, 0010h		; ch = start index of 0
					; cl = 16 registers to read
	mov	dx, 0018h		; dx = attribute controller
	int	10h			; read them!
