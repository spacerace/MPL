         title     TEST0DIV - test for ZERODIV.COM
         page      55,132

;
; TEST0DIV.ASM:  Test program for the ZERODIV.ASM
;                divide-by-zero interrupt handler
;
; Copyright (C) 1988 Ray Duncan
;
; Build:   MASM TEST0DIV;
;          LINK TEST0DIV;
;
; Usage:   ZERODIV 	(load divide by zero handler) 
;          TEST0DIV	(exercise the handler)
;

cr	equ	0dh      		;ASCII carriage return
lf	equ     0ah       		;ASCII line feed

_TEXT	segment	word public 'CODE'

	org	100H

	assume  cs:_TEXT,ds:_TEXT,es:_TEXT,ss:_TEXT

main   	proc    near

        mov	dx,offset signon	; display sign-on message
        mov     ah,9
	int     21h 
	
	mov	dx,0			; try register divide
	mov	ax,1
	mov	bx,0
	div	bx

	mov	dx,offset msg1		; display success message
	mov	ah,9
	int	21h

	mov	dx,0			; try direct divide
	mov	ax,1
	div	word ptr zerow

	mov	dx,offset msg2		; display success message
	mov	ah,9
	int	21h

	mov	dx,0			; try mod = 1 divide
	mov	ax,1
	mov	bx,offset zerow
	dec	bx
	div	word ptr [bx+1]

	mov	dx,offset msg3		; display success message
	mov	ah,9
	int	21h

	mov	dx,0			; try mod = 2 divide
	mov	ax,1
	mov	bx,0
	div	word ptr [bx+offset zerow2]

	mov	dx,offset msg4		; display success message
	mov	ah,9
	int	21h

	mov	ax,4c00h		; exit to MS-DOS
	int	21h

main   	endp 

signon	db      cr,lf,'Zero Divide Test program',cr,lf,'$'

msg1	db	cr,lf,'Back from register divide',cr,lf,'$'

msg2	db	cr,lf,'Back from direct mem divide',cr,lf,'$'

msg3	db	cr,lf,'Back from mod = 1 divide',cr,lf,'$'

msg4	db	cr,lf,'Back from mod = 2 divide',cr,lf,'$'

zerob	db	0			; byte-sized operand
zerow	dw	0			; word-sized operand

	db	256 dup (?)		; some empty space

zerow2	dw	0

_TEXT   ends

       	end   	main

