
;---------------------------------------------------------------------
; 
; name		key_scan - see if key is available from keyboard
;
; synopsis	ch = key_scan();
;
; description	Check the DOS keyboard buffer for characters available.
;		returns 0 if no characters area available, or character
;		and scan code in the same manner as key_getch().  Note
;		that this routine does not remove characters from the
;		buffer - you must use key_getch() to do that.  Several
;		calls to this routine will return the same character -
;		i.e. the first character available in keyboard buffer.
;
; Notes:
;		NOT identical to the CI-C86 library function of the 
;		same name, which returns EOF or 0xffff if no chars
;		are available.
;---------------------------------------------------------------------




	include	dos.mac

keyboard	equ	16h		; video interrupt number


	IF	LPROG
X	EQU	6		;OFFSET OF ARGUMENTS
	ELSE
X	EQU	4		;OFFSET OF ARGUMENTS
	ENDIF

	PSEG


	PUBLIC	key_scan


	IF	LPROG
key_scan	PROC	FAR
	ELSE
key_scan	PROC	NEAR
	ENDIF

	push	bp

	mov	ah,1		; scan keyboard function
	int	keyboard
	jz	no_key		; no character available
	jmp	short done
no_key:	xor	ax,ax		; return Z condition
done:	pop	bp   
	ret

key_scan	ENDP


	endps
	end



