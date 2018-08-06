
;-------------------------------------------------------------------
;
; name		crt_pset - use BIOS call to set point
;
; synopsis	crt_pset( row, col, color)
;		int	row,col,color;
;
; description	this routine uses the BIOS "set a point call", so it 
;		can be used in mono or color in any graphics mode.  The
;		major draw-back to using the BIOS call is that it
;		is SLOOOOW when compared to routines that set points
;		in specific graphics modes.
;
; notes		legal row, col, and color values depend on the 
;		particular graphics modes set.
;
;------------------------------------------------------------------------




	include	dos.mac

video	equ	10h		; video interrupt number


	IF	LPROG
X	EQU	6		;OFFSET OF ARGUMENTS
	ELSE
X	EQU	4		;OFFSET OF ARGUMENTS
	ENDIF

	PSEG


	PUBLIC	crt_pset


	IF	LPROG
crt_pset	PROC	FAR
	ELSE
crt_pset	PROC	NEAR
	ENDIF



ROW     EQU     [BP+x]
COL     EQU     [BP+x+2]
COLOR   EQU     [BP+x+4]


        push    BP
        mov     BP,SP
        mov     AH,12
        mov     AL,COLOR
        mov     CX,COL
        mov     DX,ROW
        int     10H
        pop     BP
        ret

crt_pset        ENDP


	endps
	end


