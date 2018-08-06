; The following example defnes default values for the Attribute Controller.

attrdflt db	00h, 01h, 02h, 03h, 04h, 05h, 06h, 07h
	 db	10h, 11h, 12h, 13h, 14h, 15h, 16h, 17h
	 db	08h, 00h, 0fh, 00h

	mov	ax, ds			; assume attrdflt in data segment
	mov	es, ax			; es = data segment
	mov	bx, offset attrdflt	; es:bx = attrdflt address
	mov	ah, 0f7h		; f7 = define default register table
	mov	dx, 0018h		; dx = attribute controller
	int	10h			; do it!


;---------------------------------------------------------------------------


; The following example defines a default value for the Feature Control
; register.

featdflt db	00h

	mov	ax, ds			; assume featdflt in data segment
	mov	es, ax			; es = data segment
	mov	bx, offset attrdflt	; es:bx = featdflt address
	mov	ah, 0f7h		; f7 = define default register table
	mov	dx, 0028h		; dx = feature control register
	int	10h			; do it!
