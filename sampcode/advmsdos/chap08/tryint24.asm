         title     TRYINT24 -- tests critical error handler
         page      55,132
;
; TRYINT24.ASM - Test program for INT24.ASM
;                replacement critical-error handler
;
; Copyright (C) June 1987 Ray Duncan
;
; Build:   MASM TRYINT24;
;          MASM INT24;
;          LINK TRYINT24+INT24;
;
; Usage:   TRYINT24
;

stdin	equ	0		; standard input device
stdout	equ	1		; standard output device	
stderr	equ	2		; standard error device

cr	equ	0dh		; ASCII carriage return
lf	equ	0ah		; ASCII line feed


DGROUP	group	_DATA,STACK	; 'automatic data group'

	extrn	get24:near
	extrn	res24:near

_TEXT	segment	word public 'CODE'

	assume	cs:_TEXT,ds:DGROUP,ss:STACK

main	proc	far       	; entry point from MS-DOS

	mov	ax,DGROUP	; set DS = our data segment
	mov	ds,ax

	call	get24		; capture int 24h for our
				; own handler

				; display sign-on message...
	mov	dx,offset DGROUP:msg	; DS:DX = address of message
	mov	cx,msg_len	; CX = length of message
	mov	bx,stdout	; BX = handle for std output
	mov	ah,40h		; AH = 40h, write file or device
	int	21h		; transfer to MS-DOS

err:	mov	dl,'*'		; write character to printer
	mov	ah,05		; to generate a critical error
	int	21h
	jmp	err

	call	res24		; restore previous int 24h handler

	mov	ax,4c00h	; no error, terminate program 
	int	21h		; with return code = 0

main2:	call	res24		; restore previous int 24h handler

	mov	ax,4c01h	; error, terminate program
	int	21h		; with return code = 1 

main	endp			; end of main procedure

_TEXT	ends


_DATA	segment	word public 'DATA'

msg	db	cr,lf,'Int 24H Handler Demo',cr,lf
msg_len	equ	$-msg

_DATA	ends


STACK	segment	para stack 'STACK'

	dw	64 dup (?)

STACK	ends

	end	main		; defines program entry point

