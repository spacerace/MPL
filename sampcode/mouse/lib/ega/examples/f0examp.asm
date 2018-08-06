; The following example saves the contents of the Sequencer Map Mask register
; in "myvalue".

myvalue	db	?

	mov	ah, 0f0h	; f0 = read one register
	mov	bx, 0002h	; bh = 0
				; bl = map mask index
	mov	dx, 0008h	; dx = sequencer
	int	10h		; get it!
	mov	myvalue, bl	; save it!


;---------------------------------------------------------------------


; The following example saves the contents of the Miscellaneous Output
; register in "myvalue".

myvalue	db	?

	mov	ah, 0f0h	; f0 = read one register
	mov	dx, 0020h	; dx = miscellaneous output register
	int	10h		; get it!
	mov	myvalue, bl	; save it!
