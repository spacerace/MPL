        title   TRYQFN -- QFN demo
        page    55,132
	.286

; TRYQFN.ASM --- Interactive demo of QFN routine (OS/2 version)
; Copyright (C) 1988 Ray Duncan
;
; Build:	MASM TRYQFN;
;		MASM QFN;
;		LINK TRYQFN+QFN,,,OS2,TRYQFN.DEF;
;
; Usage:	TRYQFN

cr      equ     0dh             ; ASCII carriage return
lf      equ     0ah             ; ASCII line feed

stdin   equ     0               ; standard input handle
stdout  equ     1               ; standard output handle
stderr  equ     2               ; standard error handle

	extrn	DosRead:far	; OS/2 API functions
	extrn	DosWrite:far
        extrn	DosExit:far

DGROUP  group   _DATA

_DATA   segment word public 'DATA'

ibuff   db      80 dup (0)      ; input buffer
ibuff_len equ	$-ibuff

rlen	dw	0		; length from DosRead
wlen	dw	0		; length from DosWrite

msg1    db      cr,lf,lf        
        db      'Enter filename:        '
msg1_len equ $-msg1

msg2    db      cr,lf
        db      'The full pathname is:  '
msg2_len equ $-msg2

msg3    db      cr,lf
        db      'Bad filename!'
msg3_len equ $-msg3

_DATA   ends


_TEXT   segment word public 'CODE'

        extrn   qfn:near

        assume  cs:_TEXT,ds:DGROUP

main    proc    near
                                
	push	ds		; set ES = DGROUP too
        pop	es

main1:                          ; display prompt...
	push	stdout		; standard output handle
        push	ds		; address of message
        push	offset DGROUP:msg1
        push	msg1_len	; length of message
        push	ds	        ; receives bytes written
        push	offset DGROUP:wlen
	call	DosWrite        ; transfer to OS/2

                                ; get a filename...
	push	stdin		; standard input handle
        push	ds		; input buffer address
        push	offset DGROUP:ibuff
        push	ibuff_len	; length of input buffer
        push	ds	        ; receives actual length
        push	offset DGROUP:rlen
        call	DosRead		; transfer to OS/2

	mov	ax,rlen		; anything entered?
        cmp     ax,2		; anything entered?
        je      main3           ; empty line, exit

                                ; call QFN routine to
                                ; validate and qualify
                                ; the filename...

        sub     ax,2 		; AX=length (remove CR-LF)
                                ; DS:SI = addr of filename
        mov     si,offset DGROUP:ibuff
        call    qfn             ; go qualify filename
        jc      main2           ; jump if bad filename
	mov	cx,ax		; save length

                                ; first display title...
	push	stdout		; standard output handle
        push	ds		; message address
        push	offset DGROUP:msg2
        push	msg2_len     	; message length
	push	ds	        ; receives bytes written
        push	offset DGROUP:wlen
	call	DosWrite        ; transfer to OS/2

				; display qualified filename
        push	stdout		; standard output handle
        push	ds		; address of filename
        push	si
        push	cx		; length of filename
        push	ds	        ; receives bytes written
        push	offset DGROUP:wlen
        call	DosWrite	; transfer to OS/2

        jmp     main1           ; get another filename

main2:                          ; display error message...
	push	stdout		; standard output handle
        push	ds		; message address
        push	offset DGROUP:msg3
	push	msg3_len	; message length
        push	ds	        ; receives bytes written
        push	offset DGROUP:wlen
        call	DosWrite	; transfer to OS/2

        jmp     main1           ; get another filename
                
main3:  push	1		; terminate with 
	push	0		; return code = 0
        call	DosExit		; transfer to OS/2

main    endp
        
_TEXT   ends

        end     main

