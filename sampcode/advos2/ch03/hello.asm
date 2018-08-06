	title   HELLO -- Display Message on stdout
        page    55,132
        .286

;
; HELLO.EXE
;
; A simple OS/2 assembly language program.
;
; Copyright (C) 1986 Ray Duncan
;

stdin   equ     0          ; standard input handle
stdout  equ     1          ; standard output handle
stderr  equ     2          ; standard error handle

        extrn   DosWrite:far
        extrn   DosExit:far

DGROUP  group   _DATA


_DATA   segment word public 'DATA'

msg     db      0dh,0ah,"Hello World!",0dh,0ah
msg_len equ     $-msg

wlen    dw      ?          ; receives bytes written

_DATA   ends


_TEXT   segment word public 'CODE'

        assume  cs:_TEXT,ds:DGROUP

print   proc    far

        push    stdout     ; standard output handle
        push    ds         ; address of data
        push    offset DGROUP:msg
        push    msg_len    ; length of data
        push    ds         ; receives bytes written
        push    offset DGROUP:wlen
        call    DosWrite   ; transfer to OS/2
	or	ax,ax      ; was write successful?
	jnz	error      ; jump if function failed

        push    1          ; terminate all threads
        push    0          ; return success code
        call    DosExit    ; transfer to OS/2

error:  push    1          ; terminate all threads
        push    1          ; return error code
        call    DosExit    ; transfer to OS/2

print   endp

_TEXT   ends

        end     print

