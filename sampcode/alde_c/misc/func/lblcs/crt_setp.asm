
;-----------------------------------------------------------------------
;
; name		crt_setpage -- set current text page
;
;
; synopsis	VOID	crt_setpage(page)
;			int	page;
;  
; description	Sets current active video page number to "page".
;		Used in text mode only.  Valid page #'s are 0-3 for
;		80 x 25 display and 0-7 for 40 x 25 display. 
;
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


	PUBLIC	crt_setpage


	IF	LPROG
crt_setpage	PROC	FAR
	ELSE
crt_setpage	PROC	NEAR
	ENDIF



	push	bp
	mov	bp,sp



	mov	al,[bp+x]		; page number
	mov	ah,5			; set page function
	int	video


	pop	bp
	ret


crt_setpage	ENDP


	endps
	end




