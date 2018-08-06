;----------------------------------------------------------
;
; name		crt_setborder -- set border color 
;
; synopsis	VOID	crt_setborder(color)
;			int	color;
;
; description	set border to specified color (0-15)
;
;----------------------------------------------------------	

	include	dos.mac

video	equ	10h		; video interrupt number


	IF	LPROG
X	EQU	6		;OFFSET OF ARGUMENTS
	ELSE
X	EQU	4		;OFFSET OF ARGUMENTS
	ENDIF

	PSEG


	PUBLIC	crt_setborder

	IF	LPROG
crt_setborder	PROC	FAR
	ELSE
crt_setborder	PROC	NEAR
	ENDIF


	PUSH	BP			;SAVE BP
	MOV	BP,SP

	mov	ah,11		; set border function
	mov	bh,0		;  "    "      "
	mov	bl,[bp+x]	; get the color to set
	int	video

	POP	BP
	RET

crt_setborder	ENDP


	ENDPS	
	END


