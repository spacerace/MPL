



;---------------------------------------------------------------
;
; name		crt_cls - clear screen using BIOS scroll up call
;
; synopsis 	VOID	crt_cls()
;
;
; description	This function clears your 80 x 24 screen using the
;		BIOS scroll up function.  For graphics uses, you
;		should use crt_scrollu with the appropriate 
;		background attribute - crt_cls assumes text mode.
;
;
; notes		This routine works only on the currently selected 
;		(active) page.  It uses the "normal" (07) attribute
;		on scrolled lines.  Unlike the CI-C86 function with
;		the same name, it does not home the cursor.   
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


	PUBLIC	crt_cls


	IF	LPROG
crt_cls	PROC	FAR
	ELSE
crt_cls	PROC	NEAR
	ENDIF


	push	bp
	mov	ax,0600h
	mov	bx,7
	xor	cx,cx
	mov	dh,25
	mov	dl,80
	int	video
	pop	bp
	ret


crt_cls	ENDP


	endps
	end




