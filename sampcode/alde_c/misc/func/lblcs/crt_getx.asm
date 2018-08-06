;-----------------------------------------------------------------------
;
; name		crt_getxy -- get current cursor setting
;
; synopsis	int	crt_getxy(page)
;			int 	page;
;  
; description	returns the current cursor setting in page # "page".
;		In text mode, it is possible to have up to 8 different 
;		pages, all with different cursor settings. 
;
; example	rc = crt_getxy(0);
;		column =  rc & 0xff;	/* col is lower byte		*/
;		row    =  rc >> 8;	/* row is upper byte  		*/
;					/* an interesting alternative	*/
;					/* to the above: row=rc % 0x100	*/
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

	PUBLIC	crt_getxy

	IF	LPROG
crt_getxy	PROC	FAR
	ELSE
crt_getxy	PROC	NEAR
	ENDIF

	push	bp
	mov	bp,sp
	mov	bh,[bp+x]		; get the page # into bh
	mov	ah,3
	int	video
	mov	ax,dx			; return ( 256*row + col )
	pop	bp
	ret

crt_getxy	endp


	ENDPS	
	END




