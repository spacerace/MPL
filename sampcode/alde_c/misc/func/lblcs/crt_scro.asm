;--------------------------------------------------------------------------
;
; name		crt_scrolld -- scroll screen down
;
; synopsis	VOID	crt_scrolld(ulrow,ulcol,lrrow,lrcol,count,attrib)
;
;		int	ulrow;		upper left row
;		int	ulcol;		upper left column
;		int	lrrow;		lower right row
;		int	lrcol;		lower right column
;		int	count;		# lines to scroll ( 0=whole window )
;		int	attrib;		attribute used on blank lines
;
; description	scrolls the window specified by "ulcol", "ulrow",
;		"lrcol", "lrrow" down by "count" lines using "attrib"
;		on each blank line.  If "count" is 0, the entire
;		screen is scrolled.
;
;----------------------------------------------------------	


;----------------------------------------------------------
;
; name		crt_scrollu -- scroll screen up
;
; synopsis	VOID	crt_scrollu(ulrow,ulcol,lrrow,lrcol,count,attrib)
;		int	ulrow;		upper left row
;		int	ulcol;		upper left column
;		int	lrrow;		lower right row
;		int	lrcol;		lower right column
;		int	count;		# lines to scroll ( 0=whole window )
;		int	attrib;		attribute used on blank lines
;
; description	scrolls the window specified by "ulcol", "ulrow",
;		"lrcol", "lrrow" up by "count" lines using "attrib"
;		on each blank line.  If "count" is 0, the entire
;		screen is scrolled.
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



	PUBLIC	crt_scrolld

	IF	LPROG
crt_scrolld	PROC	FAR
	ELSE
crt_scrolld	PROC	NEAR
	ENDIF


	PUSH	BP			;SAVE BP
	call	scrl1
	mov	ah,7
	int	video
	POP	BP
	RET

crt_scrolld	ENDP

;-------------------
; internal routine
;-------------------

scrl1	proc	near

	mov	bp,sp
	mov	ch,[bp+x+2]	; ulrow
	mov	cl,[bp+x+2+2]	; ulcol
	mov	dh,[bp+x+2+4]	; lrrow
	mov	dl,[bp+x+2+6]	; lrcol
	mov	al,[bp+x+2+8]	; count
	mov	bh,[bp+x+2+10]	; attrib
	ret

scrl1	endp

;-------------------------
; end of internal routine
;-------------------------

	PUBLIC	crt_scrollu

	IF	LPROG
crt_scrollu	PROC	FAR
	ELSE
crt_scrollu	PROC	NEAR
	ENDIF


	PUSH	BP			;SAVE BP
	call	scrl1
	mov	ah,6			; scroll up function
	int	video
	POP	BP
	RET

crt_scrollu	ENDP


	endps
	end

