

;-----------------------------------------------------------------------
;
; name		crt_putcamult -- put multiple characters with attribute
;
; synopsis	VOID	crt_putcamult(ch, page, count)
;		int ch, page, count;
;
;  
; description	write character and attribute at current cursor location
;		high byte of ch is attribute, low byte is character
;
;		page is page # to place text on (text modes only)
;
;		Count is # of times to put character (good for top
;		and bottom of boxes).
;
;----------------------------------------------------------------------

;-----------------------------------------------------------------------
;
; name		crt_putch -- put character only at current cursor
;
; synopsis	VOID	crt_putch(ch, page, count)
;			int ch, page, count;
;
;  
; description	Write character only at current cursor location -
;		existing attributes are used.
;
; notes		Page is page # to place text on (text modes only).
;
;		Count is # of times to put character.
;
;----------------------------------------------------------------------



	include	dos.mac

video	equ	10h		; video interrupt number


	IF	LPROG
X	EQU	6		;OFFSET OF ARGUMENTS
	ELSE
X	EQU	4		;OFFSET OF ARGUMENTS
	ENDIF

	PSEG




	PUBLIC	crt_putcamult

	IF	LPROG
crt_putcamult	PROC	FAR
	ELSE
crt_putcamult	PROC	NEAR
	ENDIF


	push	bp
	call	vput1		; set up for routine
	mov	ah,9		; multiple times
	int	video
	pop	bp
	ret

crt_putcamult	endp

;
; internal routine
;

vput1	proc	near

	mov	bp,sp
	mov	ax,[bp+x+2]	; attribute and character
	mov	bh,[bp+x+4]
	mov	cx,[bp+x+6]
	mov	bl,ah
	ret

vput1	endp	
	





	PUBLIC	crt_putch

	IF	LPROG
crt_putch	PROC	FAR
	ELSE
crt_putch	PROC	NEAR
	ENDIF


	push	bp
	call	vput1
	mov	ah,10		; write character only function
	int	video
	pop	bp
	ret

crt_putch	endp


	endps
	end
	