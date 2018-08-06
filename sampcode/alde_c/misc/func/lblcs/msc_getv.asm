
;---------------------------------------------------------------
;
; name		msc_getversion	- get library version
;
; synopsis	int vers;
;		vers = msc_getversion();
;
;		printf("\nlibrary version:  %2d . %02d",vers>>8,vers&255);
;
; description:	returns library version which can be printed as in
;		the synopsis above.
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


	PUBLIC	msc_getversion


	IF	LPROG
msc_getversion	PROC	FAR
	ELSE
msc_getversion	PROC	NEAR
	ENDIF
	mov	ax,010h
	ret


msc_getversion	ENDP


	endps
	end


