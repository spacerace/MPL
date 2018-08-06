; The following example writes the contents of "myvalue" into the CRT
; Controller Cursor Start register.

myvalue	db	3h

	mov	ah, 0f1h	; f1 = write one register
	mov	bh, myvalue	; bh = data from myvalue
	mov	bl, 000ah	; bl = cursor start index
	mov	dx, 0000h	; dx = crt controller
	int	10h		; write it!


;---------------------------------------------------------------------


; The following example writes the contents of "myvalue" into the Feature
; Control register.

myvalue	db	2h

	mov	ah, 0f1h	; f1 = write one register
	mov	bl, myvalue	; bl = data from myvalue
	mov	dx, 0028h	; dx = feature control register
	int	10h		; write it!
