        title   SIGNAL.ASM Example signal handle
        page    55,132
        .286
; 
; SIGNAL.ASM  Example signal handler for OS/2.
;             Registers a signal handler then goes to
;             sleep for 10 seconds.  During the sleep
;             you can enter ^C and the signal handler
;             will display a message.
; Copyright (C) 1987 Ray Duncan
;
; Build:        MASM SIGNAL;
;               LINK SIGNAL,,,OS2,SIGNAL
;
; Usage:        SIGNAL

stdin   equ     0                       ; standard input handle
stdout  equ     1                       ; standard output handle
stderr  equ     2                       ; standard error handle

cr      equ     0dh                     ; ASCII carriage return
lf      equ     0ah                     ; ASCII line feed

                                        ; OS/2 signal codes
SIGINTR         equ     1               ; Ctrl-C
SIGBROKENPIPE   equ     2               ; broken pipe
SIGTERM         equ     3               ; program terminated
SIGBREAK        equ     4               ; Ctrl-Break
FlagA           equ     5               ; Event flag A
FlagB           equ     6               ; Event flag B
FlagC           equ     7               ; Event flag C

        extrn   DosExit:far             ; references to OS/2 API
        extrn   DosFlagProcess:far
        extrn   DosHoldSignal:far
        extrn   DosSetSigHandler:far
        extrn   DosSleep:far
        extrn   DosWrite:far

DGROUP  group   _DATA

_DATA   segment word public 'DATA'

msg1    db      cr,lf,'Signal detected!',cr,lf
msg1_len equ $-msg1

msg2    db      cr,lf 
        db      'Signal handler installed.'
        db      cr,lf
        db      'Press Ctrl-C to demonstrate handler...'
        db      cr,lf
msg2_len equ $-msg2

wlen    dw      ?                       ; receives bytes written

prvact  dw      ?                       ; receives previous signal action
prvhdlr dd      ?                       ; receives previous handler address

_DATA   ends


_TEXT   segment word public 'CODE'

        assume  cs:_TEXT,ds:DGROUP

main    proc    far                     ; OS/2 entry point

                                        ; register signal handler...
        push    cs                      ; address of handler
        push    offset _TEXT:handler
        push    ds                      ; receives previous handler address
        push    offset DGROUP:prvhdlr
        push    ds                      ; receives previous action
        push    offset DGROUP:prvact
        push    2                       ; action = 2, call handler
        push    SIGINTR                 ; signal of interest= ^C
        call    DosSetSigHandler        ; transfer to OS/2
        or      ax,ax                   ; was set successful?
        jnz     error                   ; jump if set failed.

                                        ; display message 'Press Ctrl-C
                                        ; to demonstrate handler'
        push    stdout                  ; standard output handle
        push    ds                      ; message address
        push    offset DGROUP:msg2
        push    msg2_len                ; message length
        push    ds                      ; receives bytes written
        push    offset DGROUP:wlen
        call    DosWrite                ; transfer to OS/2

        mov     cx,10                   ; delay for 10 seconds...

main1:  push    0                       ; delay for 1 second
        push    1000
        call    DosSleep

        loop    main1

exit:   push    1                       ; terminate all threads
        push    0                       ; return success code
        call    DosExit                 ; exit program

error:  push    1                       ; terminate all threads
        push    1                       ; return error code
        call    DosExit

main    endp


handler proc    far                     ; this is the signal handler    
                                              
                                        ; display message showing
                                        ; handler is active...
        push    stdout                  ; standard output handle
        push    ds                      ; address of data to write
        push    offset DGROUP:msg1
        push    msg1_len                ; length of data
        push    ds                      ; receives length written
        push    offset DGROUP:wlen
        call    DosWrite                ; transfer to OS/2

                                        ; now reset signal...
        push    cs                      ; addr of handler
        push    offset _TEXT:handler
        push    ds                      ; receives addr previous handler
        push    offset DGROUP:prvhdlr
        push    ds                      ; receives previous action
        push    offset DGROUP:prvact
        push    4                       ; action = 4; reset signal
        push    SIGINTR                 ; signal of interest= ^C
        call    DosSetSigHandler        ; transfer to OS/2
        or      ax,ax                   ; handler set successfully?
        jnz     error                   ; jump if set failed

        ret     4                       ; return from handler, clear stack 

handler endp

_TEXT   ends

        end     main

