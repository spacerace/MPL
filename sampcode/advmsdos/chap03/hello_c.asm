	title	HELLO.COM --- print hello on terminal
	page	55,132

;
; HELLO-C.ASM	Demonstrates components of a
; 		functional COM-type assembly 
; 		language program, and an MS-DOS
;		function call.
;
; Copyright (C) 1988 Ray Duncan
;
; Build:  MASM HELLO-C;
;         LINK HELLO-C;
;         EXE2BIN HELLO.EXE HELLO.COM
;         DEL HELLO.EXE
;
; Usage:  HELLO
;

stdin	equ	0		; standard input handle
stdout	equ	1		; standard output handle
stderr	equ	2		; standard error handle

cr	equ	0dh       	; ASCII carriage return
lf	equ	0ah       	; ASCII line feed


_TEXT	segment	word public 'CODE'

	org	100h		; COM files always have
				; an origin of 100H

	assume	cs:_TEXT,ds:_TEXT,es:_TEXT,ss:_TEXT

print 	proc	near       	; entry point from MS-DOS

	mov	ah,40h		; function 40H = write
	mov	bx,stdout	; handle for standard output
	mov	cx,msg_len	; length of message
	mov	dx,offset msg	; address of message
	int	21h		; transfer to MS-DOS

	mov	ax,4c00h	; exit, return code = 0
	int	21h		; transfer to MS-DOS

print	endp


msg	db	cr,lf		; message to display
	db	'Hello World!',cr,lf

msg_len	equ	$-msg		; length of message
				

_TEXT	ends

 	end	print		; defines entry point

