
;-------------------------------------------------------------------
;
; name		crt_pread - return color of pixel on graphics screen 
;
; synopsis	int color;
;		color = crt_pread(row, col);
;		int  row, col;
;
; description	This routine reads the color of the pixel on
;		a graphics screen.  It uses the BIOS "read-a-
;		points-color" routine, so it functions on any
;		type of monitor and under all graphics modes.
;
; notes		I  have no idea what happens if you attempt to call
;		the BIOS routine with crazy row and column values.
;
;-------------------------------------------------------------------


	include	dos.mac

video	equ	10h		; video interrupt number


	IF	LPROG
X	EQU	6		;OFFSET OF ARGUMENTS
	ELSE
X	EQU	4		;OFFSET OF ARGUMENTS
	ENDIF

	PSEG


	PUBLIC	crt_pread


	IF	LPROG
crt_pread	PROC	FAR
	ELSE
crt_pread	PROC	NEAR
	ENDIF


ROW     EQU     [BP+x]
COL     EQU     [BP+x+2]


	push	bp
	mov	bp,sp

        mov     AH,13
        mov     CX,COL
        mov     DX,ROW
        int     video
        xor     AH,AH

	pop	bp
	ret


crt_pread	ENDP


	endps
	end



