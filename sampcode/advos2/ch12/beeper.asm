        title   BEEPER -- simple multithreaded application
        page    55,132
        .286

; BEEPER.ASM  --- A simple multithreaded application which
;                 creates a second thread to issue beeps
;                 at one-second intervals, while the first
;                 thread waits for a any keypress.
; Copyright (C) 1987 Ray Duncan 
;
; Build:        MASM BEEPER;
;               LINK BEEPER,,,OS2,BEEPER;
;
; Usage:        BEEPER

stdin   equ     0                       ; standard input handle
stdout  equ     1                       ; standard output handle
stderr  equ     2                       ; standard error handle

cr      equ     0dh                     ; ASCII carriage return
lf      equ     0ah                     ; ASCII line feed

stksiz  equ     4096                    ; stack size for 2nd thread

        extrn   DosAllocSeg:far         ; references to OS/2 API
        extrn   DosBeep:far
        extrn   DosCreateThread:far
        extrn   DosExecPgm:far
        extrn   DosExit:far
        extrn   DosSleep:far
        extrn   DosWrite:far
        extrn   KbdCharIn:far

DGROUP  group   _DATA

_DATA   segment word public 'DATA'

cdata   db      10 dup (0)              ; receives character data
wlen    dw      ?                       ; receives bytes written        
selector dw     ?                       ; receives segment selector
beepID  dw      ?                       ; receives thread ID

msg1    db      cr,lf
        db      'Press any key to end program...'
msg1_len equ $-msg1

msg2    db      cr,lf
        db      'Unexpected OS/2 Error'
        db      cr,lf
msg2_len equ $-msg2

_DATA   ends


_TEXT   segment word public 'CODE'

        assume  cs:_TEXT,ds:DGROUP

main    proc    far                     ; entry point for primary thread

                                        ; allocate stack for 2nd thread...
        push    stksiz                  ; size of new segment
        push    ds                      ; receives selector
        push    offset DGROUP:selector
        push    0                       ; 0=not sharable
        call    DosAllocSeg             ; transfer to OS/2
        or      ax,ax                   ; allocation successful?
        jnz     error                   ; jump if failed
        
                                        ; start new beeper thread...
        push    cs                      ; thread entry point
        push    offset _TEXT:Beeper
        push    ds                      ; receives Thread ID
        push    offset DGROUP:beepID
        push    selector                ; stack for new thread
        push    stksiz
        call    DosCreateThread         ; transfer to OS/2
        or      ax,ax                   ; new thread created?
        jnz     error                   ; jump if failed

                                        ; display message,
                                        ; 'Press any key to end program'...
        push    stdout                  ; standard output handle
        push    ds                      ; address of message
        push    offset msg1
        push    msg1_len                ; length of message
        push    ds                      ; receives bytes written 
        push    offset wlen
        call    DosWrite                ; transfer to OS/2

                                        ; now wait for key...
        push    ds                      ; receives data packet
        push    offset DGROUP:cdata
        push    0                       ; 0=wait for char.
        push    0                       ; keyboard handle
        call    KbdCharIn               ; transfer to OS/2

                                        ; final exit to OS/2...
        push    1                       ; terminate all threads
        push    0                       ; exit code=0 (success)
        call    DosExit                 ; transfer to OS/2

error:                                  ; display error message...
        push    stdout                  ; standard output handle
        push    ds                      ; message address
        push    offset msg1
        push    msg1_len                ; message length
        push    ds                      ; receives bytes written
        push    offset wlen
        call    DosWrite                ; transfer to OS/2

        push    1                       ; terminate all threads
        push    1                       ; return error code
        call    DosExit                 ; exit program

main    endp


beeper  proc    far                     ; thread entry point

                                        ; sound a tone...
        push    440                     ; 440 Hz
        push    100                     ; 100 milliseconds
        call    DosBeep                 ; transfer to OS/2

        push    0                       ; now suspend thread
        push    1000                    ; for 1 second...
        call    DosSleep                ; transfer to OS/2

        jmp     beeper                  ; do it again...

beeper  endp

_TEXT   ends

        end     main
