;
;
;  this is called by a 'c' program to get the FAT Ident. Byte
;
;
_TEXT	 SEGMENT BYTE PUBLIC 'CODE'
	 ASSUME CS:_TEXT
	 PUBLIC _GETFATID
;
_getfatid proc near
;
; STANDARD C ENTRY
;
	 PUSH  BP
	 MOV   BP,SP
;
;
	mov	dx,[bp+4]		; get drive # from stack frame
	push	ds
	mov	ah,1Ch			; get drive allocation table info
	int	21h
	mov	ah,[bx] 		; get FAT ident. byte
	pop	ds
	inc	al			; check for error return
	jnz	atiok			; jump if not error
;
	xor	ax,ax			; 0 return = some error
;
atiok:	mov	al,ah			; put FAT ID byte in al for return
	xor	ah,ah			; return the low half of the word
;
;  STANDARD C EXIT
;
	 MOV   SP,BP
	 POP   BP
	 RET
;
_getfatid endp

_TEXT	 ENDS
	 END
