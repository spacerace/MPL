
;---------------------------------------------------------------
;
; name		key_flush	- flush keyboard buffer
;
; synopsis	VOID	key_flush()
;
; description	Flushes (clears out) the keyboard buffer.
;
;
;--------------------------------------------------------------

	include	dos.mac

keyboard	equ	16h	; keyboard interrupt number


	IF	LPROG
X	EQU	6		;OFFSET OF ARGUMENTS
	ELSE
X	EQU	4		;OFFSET OF ARGUMENTS
	ENDIF

	PSEG


	PUBLIC	key_flush


	IF	LPROG
key_flush	PROC	FAR
	ELSE
key_flush	PROC	NEAR
	ENDIF


	push	bp

	mov	ah,3
	int	keyboard

	pop	bp
	ret

key_flush	ENDP

	endps
	end


