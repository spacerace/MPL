
		page	60,132
;-----------------------------------------------------------------------
;
; name		key_getch -- read a char from the keyboard
;
; synopsis	int	ch;
;		ch = key_getch();
;		scan = ch >> 8;
;		ascii = ch & 255;
;
; description	Waits for a key to be pressed on the keyboard and
;		returns the character.  Removes character from input 
;		buffer.  Returns both ASCII char and scan code.
;		scan code is in the high 8 bits, ASCII in the low 8 bits.
; Notes:
;		arrow keys and some others return only scan codes. Shift
;		keys, control keys and some others do not show up at all
; 		when using this function.
;
;----------------------------------------------------------------------


	include	dos.mac


	IF	LPROG
X	EQU	6		;OFFSET OF ARGUMENTS
	ELSE
X	EQU	4		;OFFSET OF ARGUMENTS
	ENDIF

	PSEG


	PUBLIC	key_getch

	IF	LPROG
key_getch	PROC	FAR
	ELSE
key_getch	PROC	NEAR
	ENDIF


	mov	ah,0
	int	16h		; call keyboard function	
	ret

key_getch	endp

	endps
	end

