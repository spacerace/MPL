
;-----------------------------------------------------------------------
;
; name		crt_setmode -- set screen mode
;
; synopsis	VOID	crt_setmode(mode)
;			int	mode;
;
; description      Set screen to graphics or text mode.
;
;
;		TEXT MODES
;		----------
;		0	=	40 x 25  monochrome
;		1	=	40 x 25  color		
;		2	=	80 x 25  monochrome	
;		3	=	80 x 25  color		
;
;		GRAPHICS MODES
;		--------------
;		4	=	320 x 200 color		
;		5	=	320 x 200 monochrome	
;		6	=	640 x 400 monochrome	
;		7	=	unknown
;
;		OTHER MODES	(some require HI-RES board)
;		-----------
;		8	=	640 x 400 color		
;		9	=	640 x 400 monochrome	
;     	       10	=	160 x 200 color (16 colors)
;
;------------------------------------------------------------------

;-----------------------------------------------------------------------
;
; name		crt_getmode -- get current video mode
;
; synopsis	int	crt_getmode()
;
; description	Returns current value of mode setting.
;		See set_mode for further info about modes.
;
;-----------------------------------------------------------------------


	include	dos.mac

video	equ	10h		; video interrupt number


	IF	LPROG
X	EQU	6		;OFFSET OF ARGUMENTS
	ELSE
X	EQU	4		;OFFSET OF ARGUMENTS
	ENDIF

	PSEG


	PAGE	60,132

	PUBLIC	crt_setmode

	IF	LPROG
crt_setmode	PROC	FAR
	ELSE
crt_setmode	PROC	NEAR
	ENDIF


	PUSH	BP			;SAVE BP
	MOV	BP,SP

	mov	al,[bp+x]		; get mode to set
	xor	ah,ah			; reset display adaptor function
	int	video

	POP	BP
	RET
crt_setmode	ENDP

	page	60,132


	PUBLIC	crt_getmode

	IF	LPROG
crt_getmode	PROC	FAR
	ELSE
crt_getmode	PROC	NEAR
	ENDIF


	mov	ah,15		; get current CRT mode function
	int	video
	xor	ah,ah		; make full word in ax
	ret

crt_getmode	ENDP




;=======================================================================



	ENDPS	
	END


                               