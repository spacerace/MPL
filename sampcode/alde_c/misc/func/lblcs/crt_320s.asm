;------------------------------------------------------------------
; name		crt_ps320sg 
;
; synopsis	crt_ps320sg(segment, offset, row, col, color)
;		int	segment;	/* 0xb000=mono    0xb800=color	*/
;		int	offset;		/* use 0 for graphics adaptor	*/
;		int	row;		/*  row number 0 to 199		*/
;		int	col;		/*  col number 0 to 399		*/
;		int	color;		/*  color      0 to 3		*/
;
; description	This subroutine sets a point on the 320 x 200
;		graphics screen.  Hand coded in assembly, It
;		operates 8 - 10 times faster than the corres-
;		ponding BIOS call.  
;
; notes		This routine will treat any passed segment value 
;		like an interlaced graphics adaptor.  It could 
;		just as well be a block of memory that has been
;		allocated, (i.e. a graphics buffer that is to be moved
;		to the screen after a group of points are set ),
;		or some other segment value for a non-PC compatible
;		display.  If you are addressing the display adaptors,
;		use an offst of 0.  If you are handling a buffer, use
;		the buffers segment and offset.  Large model pointers
;		must be broken into segment and offset which are 
;		passed separately.  
;		
;		This routine handles graphics interlacing 
;		(normal IBM PC graphics).  It would not function
;		on a machine that does not interlace graphics -
;		i.e. the Tandy T2000.
;
;		If you pass the correct segment value based on the 
;		equipment flag setting, this routine will work on
;		both a color and mono display adaptor.
;		 
; bugs		row, col, and color are not validated - make sure
;		they are in range or you could crash the system. 
;
;-------------------------------------------------------------------

	include	dos.mac

	IF	LPROG
X	EQU	6		;OFFSET OF ARGUMENTS
	ELSE
X	EQU	4		;OFFSET OF ARGUMENTS
	ENDIF

	PSEG

	PUBLIC	crt_ps320sg

	IF	LPROG
crt_ps320sg	PROC	FAR
	ELSE
crt_ps320sg	PROC	NEAR
	ENDIF

SEGM	equ	x+2
OFS	equ	x+4
COL	equ	x+6
ROW	equ	x+8
COLR	equ	x+10


	push	bp
	push	es
	mov	bp,sp
	mov	ax,[bp+SEGM]
	mov	es,ax		; set es to color video segment

	mov	ax,[bp+ROW]	; get row
	mov	cx,[bp+COL]	; get columnm
	mov	dl,[bp+CoLR]	; get color

	
	and	ax,0feh		; strip odd/even bit of Y (row)

	sal	ax,1
	sal	ax,1

	sal	ax,1		; ax = ax * 8
	mov	bx,ax		; bx = ax * 8

	sal	ax,1		; ax = ax * 16
	sal	ax,1		; ax = ax * 32

	add	ax,bx		; ax = ax * 40
	mov	bx,[bp+ROW]
	sar	bx,1		; low bit into carry
	jnb	even		; if low bit 0, y is even
	add	ax,02000h	; adjust for odd y - other video ram bank

even:
	sar	cx,1		
	sar	cx,1		; cx = x / 4
	add	ax,cx		; ax has final address
	mov	cx,[bp+OFS]
	add	ax,cx		; add in offset value
	mov	si,ax		
	mov	cx,[bp+COL]
; now determine the bit to set
	and	cx,0003h	; cx = x mod 4

	clc			; clear carry
	rcr	dl,1		; move color bit to cary
	rcr	dl,1		; move color bit to bit 7 of dl
	rcr	dl,1		; move color bits to 7,6
	mov	al,0c0h		; start with 1100000b

	add	cl,cl		; cl = cl * 2
	shr	al,cl		; shift bit to right position
	shr	dl,cl	

	not	al		; do 1's complement
	and	al,es:[si]	; mask the right bit
	or	al,dl		; or in the color bit

	mov	es:[si],al	; put back on screen

	pop	es
	pop	bp		; restore registers
	ret

crt_ps320sg	endp

	ENDPS	
	END



