;**************************************************************
;*
;*	Tests Hercules BIOS, specifically the functions 9 & 10
;*	to put characters on the screen in graphics mode.
;*
;***************************************************************
VIDI	equ	10H		; video interrupt, 10H (50H for debug)
cseg	segment	common
	assume	cs:cseg,ds:cseg
START	proc
;			; prompt for IBM or Hercules graphics mode
	mov	ah,9		; DOS 9 = display string
	mov	dx,offset mode_prmt+100H
				; display mode prompt
	int	21H		; DOS function
	mov	ah,1		; DOS 1 = kbd input
	int	21H
	mov	bl,al		; input char --> BL
	mov	ax,6		; ibm graphics mode
	cmp	bl,"h"		; input "h" for Hercules
	jne	i_mode
	add	ax,2		; hercules mode
i_mode:	int	VIDI
;
	xor	bh,bh		; page 0
	mov	bl,7		; normal attribute
	mov	dh,1		; cursor at <1,1> to start
	mov	dl,1
;
	mov	cx,24		; do 24 rows
row:	push	cx		; save row counter
	inc	dh		; next row
	mov	dl,0		; back to first column
	mov	ah,2		; cursor move function
	int	VIDI
;			; compute video mode
	mov	bl,dh		; row # to BL
	xor	bh,bh
	shr	bl,1		; 4 rows per mode
	shr	bl,1
	mov	bl,mode_seq[bx+100H]	; index into mode sequence for mode
;
;			; for debug, first print [row+64]
	mov	al,dh
	add	al,64
	mov	ah,10		; fn 10 prints w/o attributes
	mov	cx,1
	int	VIDI
;
	xor	bh,bh		; page 0
	mov	cx,64		; each row 64 characters
achar:	push	cx		; save char counter
	mov	ah,9		; write_char function
	mov	al,dh		; character = col + (row mod 2)*64
	and	al,1		; row mod 2
	ror	al,1		; *128
	shr	al,1
	add	al,dl		; + col
	mov	cx,1		; write one of them
	int	VIDI
	inc	dl		; increment row counter
	mov	ah,2		; cursor move function
	int	VIDI
	pop	cx		; restore character counter
	loop	achar
	pop	cx		; restore row counter
	loop	row
;
;			; draw a line where it should start
	mov	dx,5		; row 5
	mov	cx,0		; col 0
line:	push	cx
	push	dx
	mov	ax,0C01H	; function 12=pixel
	int 	VIDI
	pop	dx
	pop	cx
	inc	cx		; step col
	cmp	cx,700		; ...until 700
	jle	line
;
;			; wait for a keystroke
	mov	ah,1
	int	21H
;
	mov	ax,7		; back to alpha mode
	int 	VIDI
;
;	  		; Now return to system
	xor	ax,ax		; zero --> AX
	int	21H		; DOS function call 0 - terminate normally
;
mode_prmt	db	"hercules or ibm mode? [ibm] $"
mode_seq	db	7,0Fh,1,70h,0,9	; sequence norm,hi,ul,rev,invis,norm
START	endp
cseg	ends
        end     START
