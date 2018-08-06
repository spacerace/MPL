        title   PORTS -- IOPL Demo Program
        page    55,132
        .286

;
; PORTS.ASM
;
; An OS/2 IOPL demonstration program with that reads and displays the
; first 256 I/O ports.	Requires the separate module PORTIO.ASM.
;
; Assemble with:  C> masm ports.asm;
; Link with:  C> link ports+portio,ports,,os2,ports
;
; Usage is:  C> ports
;
; Copyright (C) 1988 Ray Duncan
;

cr      equ     0dh                     ; ASCII carriage return
lf	equ	0ah			; ASCII linefeed
blank   equ     20h                     ; ASCII space code

stdout  equ     1                       ; standard output handle
stderr  equ     2                       ; standard error handle

bport   equ     0                       ; first port to display
eport   equ     255                     ; last port to display
request equ     0                       ; request port access
release equ     1                       ; release port access

        extrn   DosPortAccess:far       ; kernel API functions
        extrn   DosWrite:far
        extrn   DosExit:far
        
        extrn   rport:far               ; PORTIO.ASM functions
        extrn   wport:far

DGROUP  group   _DATA

_DATA   segment word public 'DATA'

wlen    dw      0                       ; actual number of bytes
                                        ; written by DosWrite

msg1    db      cr,lf                   ; error message
        db      'DosPortAccess failed.'
        db      cr,lf
msg1_len equ    $-msg1

msg2    db      cr,lf                   ; heading
        db      '        0  1  2  3  4  5  6'
        db      '  7  8  9  A  B  C  D  E  F'
msg2_len equ    $-msg2

msg3    db      cr,lf                   ; display port number
msg3a   db      'NNNN  '
msg3_len equ    $-msg3

msg4    db      ' '                     ; display port data
msg4a   db      'NN'
msg4_len equ    $-msg4

_DATA   ends    


_TEXT   segment word public 'CODE'

        assume  cs:_TEXT,ds:DGROUP

main    proc    far                     ; entry point from OS/2

        push    ds                      ; make DGROUP addressable
        pop     es                      ; with ES too

                                        ; request port access...
        push    0                       ; reserved
        push    request                 ; request/release
        push    bport                   ; first port number
        push    eport                   ; last port number
        call    DosPortAccess           ; transfer to OS/2
        or      ax,ax                   ; call successful?
        jz      main1                   ; yes, jump

                                        ; display error message...
        push    stderr                  ; standard error handle
        push    ds                      ; message address
        push    offset DGROUP:msg1
        push    msg1_len                ; message length
        push    ds                      ; receives bytes written
        push    offset DGROUP:wlen
        call    DosWrite                ; transfer to OS/2

                                        ; now terminate process...
        push    1                       ; terminate all threads
	push	1			; return code = 1 (error)
        call    DosExit                 ; transfer to OS/2

main1:                                  ; print heading...
        push    stdout                  ; standard output handle
        push    ds                      ; address of heading
        push    offset DGROUP:msg2
        push    msg2_len                ; length of heading
        push    ds                      ; receives bytes written        
        push    offset DGROUP:wlen
        call    DosWrite                ; transfer to OS/2      

        mov     dx,bport                ; initialize port number
        
main2:  test    dx,0fh                  ; new line needed?
        jnz     main3                   ; no, jump
        
        mov     ax,dx                   ; convert port number
        mov     di,offset DGROUP:msg3a  ; to ASCII for display
        call    wtoa

                                        ; display port number...
        push    stdout                  ; standard output handle
        push    ds                      ; message address
        push    offset DGROUP:msg3
        push    msg3_len                ; message length
        push    ds                      ; receives bytes written        
        push    offset DGROUP:wlen
        call    DosWrite                ; transfer to OS/2      

main3:  push    dx                      ; call 'rport' to read port
        call    rport                   ; returns AX = data

        mov     di,offset msg4a         ; convert port data 
        call    btoa                    ; to ASCII for display

                                        ; display port data...
        push    stdout                  ; standard output handle
        push    ds                      ; message address
        push    offset DGROUP:msg4
        push    msg4_len                ; message length
        push    ds                      ; receives bytes written        
        push    offset DGROUP:wlen
        call    DosWrite                ; transfer to OS/2      

        inc     dx                      ; increment port number
        cmp     dx,eport                ; done with all ports?
        jbe     main2                   ; not yet, read another

                                        ; release port access...
        push    0                       ; reserved
        push    release                 ; request/release
        push    bport                   ; first port number
        push    eport                   ; last port number
        call    DosPortAccess           ; transfer to OS/2

                                        ; final exit to OS/2...
        push    1                       ; terminate all threads
	push	0			; return code = 0 (success)
        call    DosExit                 ; transfer to OS/2
        
main    endp

; WTOA:         Convert word to hex ASCII
; 
; Call with:    AX    = data to convert
;               ES:DI = storage address
; Returns:      nothing
; Uses:         AX, CL, DI

wtoa    proc    near

        push    ax                      ; save original value
        mov     al,ah
        call    btoa                    ; convert upper byte    

        pop     ax                      ; restore original value
        call    btoa                    ; convert lower byte

        ret                             ; back to caller

wtoa    endp


; BTOA:         Convert byte to hex ASCII
; 
; Call with:    AL    = data to convert
;               ES:DI = storage address
; Returns:      nothing
; Uses:         AX, CL, DI

btoa    proc    near

        sub     ah,ah                   ; clear upper byte

        mov     cl,16                   ; divide by 16
        div     cl

        call    ascii                   ; convert quotient
        stosb                           ; store ASCII character

        mov     al,ah
        call    ascii                   ; convert remainder 
        stosb                           ; store ASCII character

        ret                             ; back to caller

btoa    endp


; ASCII:        Convert nibble to hex ASCII
; 
; Call with:    AL    = data to convert in low 4 bits
; Returns:      AL    = ASCII character
; Uses:         nothing

ascii   proc    near

        add     al,'0'                  ; add base ASCII value  
        cmp     al,'9'                  ; is it in range 0-9?
        jle     ascii2                  ; jump if it is

        add     al,'A'-'9'-1            ; no, adjust for range A-F

ascii2: ret				; return ASCII character in AL

ascii   endp

_TEXT   ends

        end     main                    ; defines entry point
