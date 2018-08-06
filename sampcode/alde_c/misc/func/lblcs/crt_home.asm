

;---------------------------------------------------------------
;
; name		crt_home - home cursor on given page
;
; synopsis	VOID crt_home(page)
;			int	page;
;
; description	sets cursor to 0,0 on selected page.  This routine 
;		differs slightly from the CI-C86 function with the
;		same name - the CIC86 function always assumes page 0.
;
; notes		There is basically no difference between doing a
;		crt_home and a crt_goto to row 0, col 0. 
;
;	 
;--------------------------------------------------------------

	include	dos.mac

video	equ	10h		; video interrupt number


	IF	LPROG
X	EQU	6		;OFFSET OF ARGUMENTS
	ELSE
X	EQU	4		;OFFSET OF ARGUMENTS
	ENDIF

	PSEG


	PUBLIC	crt_home


	IF	LPROG
crt_home	PROC	FAR
	ELSE
crt_home	PROC	NEAR
	ENDIF


	push	bp
	mov	bp,sp
	mov	bh,[bp+x]	; get page #		
	mov	ax,0200h
	xor	dx,dx
	int	video
	pop	bp
	ret


crt_home	ENDP


	endps
	end


