;***	_int33h - simple program to call the mouse interput hex 33
;*
;*	Passed in [bp+4] is the offset in ds of a six word
;*	array containing the data to place in ax,bx,cx, and
;*	dx for the int 33h.  An int 33h is then called
;*	and ax,bx,cx and dx are returned in the same array.
;*	To call from C, store the registers you wish to pass
;*	in a near array and call like this:
;*
;*		int33h (&registers);
;*
;
_TEXT	SEGMENT  BYTE PUBLIC 'CODE'
_TEXT	ENDS
_DATA	SEGMENT  WORD PUBLIC 'DATA'
_DATA	ENDS
CONST	SEGMENT  WORD PUBLIC 'CONST'
CONST	ENDS
_BSS	SEGMENT  WORD PUBLIC 'BSS'
_BSS	ENDS
DGROUP	GROUP	CONST, _BSS, _DATA
	ASSUME	CS: _TEXT, DS: DGROUP, SS: DGROUP, ES: DGROUP
_TEXT	   SEGMENT
	PUBLIC	_int33h
_int33h PROC NEAR
	push	bp
	mov	bp,sp
	push	di
	push	si
	mov	si,WORD PTR [bp+4]	; (ds:si) = array of passed registers
	mov	ax,[si] 		; load registers for int
	mov	bx,[si+2]
	mov	cx,[si+4]
	mov	dx,[si+6]
	int	33h
	mov	[si],ax 		; load in returned registers
	mov	[si+2],bx
	mov	[si+4],cx
	mov	[si+6],dx
	pop	si
	pop	di
	mov	sp,bp			; exit
	pop	bp
	ret
_int33h ENDP
_TEXT	ENDS
END
