        title	SHOWENV -- demo of GETENV subroutine
	page    55,132
	.286

; SHOWENV.ASM - Demonstrates use of GETENV routine (OS/2 version).
;		Prompts user for the name of an environment 
;		variable, then displays the associated string.
; Copyright (C) 1987 Ray Duncan
;
; Build:	MASM SHOWENV;  
;		MASM GETENV;
;		LINK SHOWENV+GETENV,,,OS2,SHOWENV
;
; Usage:	SHOWENV

stdin	equ	0		; standard input handle
stdout	equ	1		; standard output handle

cr	equ	0dh       	; ASCII carriage return
lf	equ	0ah       	; ASCII line feed

	extrn	DosExit:far	; OS/2 API functions
        extrn	DosRead:far
        extrn	DosWrite:far

	extrn	getenv:near	; returns address of env. string

DGROUP	group	_DATA

_DATA	segment	word public 'DATA'

msg1	db	cr,lf,lf,'Enter name:    '
msg1_len equ $-msg1

msg2	db	cr,lf,   'The value is:  '
msg2_len equ $-msg2

msg3	db	'not found!'
msg3_len equ $-msg3

inbuf	db	64 dup (0)	; keyboard input buffer
inbuf_len equ $-inbuf

rlen	dw	?		; receives actual number
				; of bytes read

wlen	dw	?		; receives actual number
				; of bytes written

_DATA	ends


_TEXT	segment	word public 'CODE'

	assume	cs:_TEXT,ds:DGROUP

main	proc	far       	; entry point from OS/2

				; get env. variable,
				; display cue to user...
	push	stdout		; standard output handle
        push	ds		; address of message
        push	offset DGROUP:msg1
        push	msg1_len	; length of message
        push	ds		; receives bytes written
        push	offset DGROUP:wlen
        call	DosWrite	; transfer to OS/2

				; get name of environment
			        ; variable from user...
	push	stdin		; standard input handle
        push	ds	        ; address of input buffer
        push	offset DGROUP:inbuf
        push	inbuf_len	; length of input buffer
        push	ds	        ; receives actual length
        push	offset DGROUP:rlen
	call	DosRead	        ; transfer to OS/2

	mov	ax,rlen		; get length of input
	sub	ax,2		; remove cr-lf characters
	or	ax,ax		; anything entered?
	jz	main2		; no, exit

	mov	bx,ax		; append null to string
	mov	byte ptr [bx+inbuf],0

				; display "The value is:"
	push	stdout		; standard output handle
        push	ds		; address of message
        push	offset DGROUP:msg2
        push	msg2_len	; length of message
        push	ds		; receives bytes written
        push	offset DGROUP:wlen
        call	DosWrite	; transfer to OS/2

	mov	si,offset inbuf	; address of ASCIIZ string
	call	strupr		; fold to upper case
	call	getenv		; then search environment	

	or	ax,ax		; find anything?
	jz	main1		; no, display error message

				; yes, display value of
			        ; environment string
	push	stdout		; standard output handle
        push	es	        ; address of string
        push	di
        push	ax	        ; length of string
        push	ds	        ; receives bytes written
        push	offset DGROUP:wlen
	call	DosWrite	; transfer to OS/2	

	jmp	main		; go ask for another

main1:				; env. variable not found
	push	stdout		; standard output handle
        push	ds		; address of error message
        push	offset DGROUP:msg3
        push	msg3_len	; length of message
        push	ds		; receives bytes written
        push	offset DGROUP:wlen
        call	DosWrite        ; transfer to OS/2

	jmp	main		; go look for another

main2:				; final exit to OS/2
	push	1		; terminate all threads
        push	0		; return code = 0
        call	DosExit		; transfer to OS/2

main	endp


strupr	proc	near		; convert ASCII string to
				;  upper case
				; call with DS:SI = string
	push	si		; save string address

strup1:	lodsb			; next character
	or	al,al		; found end (null byte) ? 
	jz	strup2		; yes, jump
	cmp	al,'a'		; test if in range 'a'-'z'
	jb	strup1		; skip it if not >= a
	cmp	al,'z'
	ja	strup1		; skip it if not <= z
				; change char to lower case
	sub	byte ptr [si-1],'a'-'A'
	jmp	strup1		; get another char

strup2:	pop	si		; restore original string
	ret			; address and return
	
strupr	endp

_TEXT	ends

	end	main

