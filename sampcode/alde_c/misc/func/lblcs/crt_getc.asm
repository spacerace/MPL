
;-----------------------------------------------------------------------
;
; name		crt_getca -- get character and attribute
;
; synopsis	VOID	crt_getca(page)
;			int	page;
;  
; description	Reads character and attribute under cursor on
;		video page number "page". On return, character
;		in low byte ( ch = crt_getca(0) & 255), and 
;		attribute is in high byte ( atr = crt_getca(0) >> 8).
; 
; notes		use crt_gotoxy to position cursor to desired row, 
;		col, and page. 
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


	PUBLIC	crt_getca

	IF	LPROG
crt_getca	PROC	FAR
	ELSE
crt_getca	PROC	NEAR
	ENDIF


	push	bp
	mov	bp,sp
	mov	bh,[bp+x]	
	mov	ah,8
	int	video
	pop	bp
	ret

crt_getca	endp


	ENDPS	
	END

