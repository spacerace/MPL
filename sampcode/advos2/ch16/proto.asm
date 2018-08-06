	title	  PROTO -- Filter Template
        page      55,132
        .286

;
; PROTO.ASM
;
; MASM filter template for OS/2.
;
; Assemble with:  C> masm proto.asm;
; Link with:  C> link proto,,,os2;
;
; Usage is:  C> proto <source >destination
;
; Copyright (C) 1988 Ray Duncan
;

stdin   equ     0               ; standard input device
stdout  equ     1               ; standard output device        
stderr  equ     2               ; standard error device

cr      equ     0dh             ; ASCII carriage return
lf      equ     0ah             ; ASCII linefeed

bufsize equ     256             ; I/O buffer size

        extrn   DosRead:far
        extrn   DosWrite:far
        extrn   DosExit:far


DGROUP  group   _DATA           ; 'automatic data group'

_DATA   segment word public 'DATA'

input   db      bufsize dup (?) ; storage for input line 
output  db      bufsize dup (?) ; storage for output line

rlen    dw      ?               ; receives bytes read   
wlen    dw      ?               ; receives bytes written

_DATA   ends


_TEXT   segment word public 'CODE'

        assume  cs:_TEXT,ds:DGROUP

main    proc    far             ; entry point from OS/2

        push    ds              ; make DGROUP addressable
        pop     es              ; via ES register too
        assume  es:DGROUP

        cld                     ; safety first

main1:                          ; read line from standard input...
        push    stdin           ; standard input handle
        push    ds              ; buffer address
        push    offset DGROUP:input
        push    bufsize         ; buffer length
        push    ds              ; receives bytes read
        push    offset DGROUP:rlen
        call    DosRead         ; transfer to OS/2
        or      ax,ax           ; was read successful?
        jnz     main3           ; exit if any error

        mov     ax,rlen         ; get length of input
        or      ax,ax           ; any characters read?
        jz      main2           ; jump if end of stream

        call    translate       ; translate line if necessary

        or      ax,ax           ; anything to output?
        jz      main1           ; no, go read another line

                                ; write line to standard output...
        push    stdout          ; standard output handle
        push    ds              ; buffer address
        push    offset DGROUP:output
        push    ax              ; buffer length
        push    ds              ; receives bytes written
        push    offset DGROUP:wlen
        call    DosWrite        ; transfer to OS/2
        or      ax,ax           ; write successful?
        jnz     main3           ; exit if any error

        jmp     main1           ; go read another line

main2:                          ; end of stream reached
        push    1               ; 1 = end all threads
        push    0               ; 0 = exit code
        call    DosExit         ; final exit to OS/2

main3:                          ; error encountered
        push    1               ; 1 = end all threads
        push    1               ; 1 = exit code
        call    DosExit         ; final exit to OS/2

main    endp                    ; end of main procedure


; Perform any necessary translation on line stored in
; 'input' buffer, leaving result in 'output' buffer.
;
; Call with:	AX = length of data in 'input' buffer
;
; Return:	AX = length to write to standard output
;
; Action of template routine is just to copy the line.

translate proc  near

                                ; copy input to output...
        mov     si,offset DGROUP:input
        mov     di,offset DGROUP:output
        mov     cx,ax
        rep movsb
        ret                     ; return AX = length unchanged

translate endp


_TEXT   ends

        end     main            ; program entry point
