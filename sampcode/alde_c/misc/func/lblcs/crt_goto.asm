
;--------------------------------------------------------------------------
;
; name		crt_gotoxy -- cursor set
;
; synopsis	VOID	crt_gotoxy(row, col, page)
;			int row;
;			int col;
;			int page;
;
; description	This function does a cursor position set to 
;		row, col on the specified page.  (0-7 in 40
;		column mode; 0-3 in 80 column mode)  It does
;		not, however actually change the current text
;		page. (See crt_setpage function)
;
;-------------------------------------------------------------------------

	include	dos.mac

video	equ	10h		; video interrupt number


	IF	LPROG
X	EQU	6		;OFFSET OF ARGUMENTS
	ELSE
X	EQU	4		;OFFSET OF ARGUMENTS
	ENDIF

	PSEG


	PUBLIC	crt_gotoxy

	IF	LPROG
crt_gotoxy	PROC	FAR
	ELSE
crt_gotoxy	PROC	NEAR
	ENDIF


	push	bp		;SAVE BP
	mov	bp,sp

	mov	ah,2			; set cursor position function
	mov	dh,[BP + X ]		; get row     (get byte out of int)
	mov	dl,[bp + x + 2]		; get column  (get byte out of int)
	mov	bh,[bp + x + 4]		; get page #  (get byte out of int)
	int	video			; video BIOS functions

	POP	BP
	RET

crt_gotoxy	ENDP


	ENDPS	
	END





