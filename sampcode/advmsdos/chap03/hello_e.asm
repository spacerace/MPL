	title	HELLO.EXE --- print Hello on terminal
	page	55,132
;
; HELLO-E.ASM	Demonstrates components of a
; 		functional EXE-type assembly 
; 		language program, use of segments, 
;		and a MS-DOS function call.
;
; Copyright (C) 1988 Ray Duncan
;
; Build:   MASM HELLO-E;
;          LINK HELLO-E;
;
; Usage:   HELLO-E
;

stdin	equ	0		; standard input handle
stdout	equ	1		; standard output handle
stderr	equ	2		; standard error handle

cr	equ	0dh       	; ASCII carriage return
lf	equ	0ah       	; ASCII line feed


_TEXT	segment	word public 'CODE'

	assume	cs:_TEXT,ds:_DATA,ss:STACK

print 	proc	far 		; entry point from MS-DOS      

	mov	ax,_DATA	; make our data segment
        mov     ds,ax		; addressable...

	mov	ah,40h		; function 40h = write
	mov	bx,stdout	; standard output handle
	mov	cx,msg_len	; length of message
	mov	dx,offset msg	; address of message
	int	21h		; transfer to MS-DOS

	mov	ax,4c00h	; exit, return code = 0
	int	21h		; transfer to MS-DOS

print	endp

_TEXT	ends


_DATA	segment	word public 'DATA'

msg	db	cr,lf		; message to display
	db	'Hello World!',cr,lf

msg_len	equ	$-msg		; length of message

_DATA	ends


STACK	segment	para stack 'STACK'

	db	128 dup (?)

STACK	ends

 	end	print		; defines entry point

