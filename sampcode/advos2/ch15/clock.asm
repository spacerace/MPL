        title   CLOCK - asynchronous periodic timer example
        page    55,132
        .286
; 
; CLOCK.ASM --- Example of an asynchronous periodic 
; timer.  Displays the current time at one-second 
; intervals in the upper-right corner of the screen.
;
; Copyright (C) 1987 Ray Duncan 
;
; Build:        MASM CLOCK;
;               LINK CLOCK,,,OS2,CLOCK  
;
; Usage:        CLOCK
;
; Terminate program with Ctrl-C or Ctrl-Break

stdin   equ     0
stdout  equ     1
stderr  equ     2

cr      equ     0dh                     ; ASCII carriage return
lf      equ     0ah                     ; ASCII line feed
escape  equ     01bh                    ; ASCII escape code

        extrn   DosExit:far
        extrn   DosCreateSem:far
        extrn   DosGetInfoSeg:far
        extrn   DosSemWait:far
        extrn   DosSemSet:far
        extrn   DosTimerStart:far
        extrn   DosWrite:far

jerr    macro   target                  ;; Macro to test return code
        local   zero                    ;; in AX and jump if non-zero
        or      ax,ax                   ;; Uses JMP16 to avoid
        jz      zero                    ;; branch out of range errors
        jmp     target
zero:
        endm

DGROUP  group   _DATA

_DATA   segment word public 'DATA'

sname   db      '\SEM\CLOCK.SEM',0      ; name of semaphore
shandle dd      ?                       ; handle of semaphore
semflag dw      0                       ; <>0 if creator of semaphore

thandle dw      ?                       ; handle of periodic timer

wlen    dw      ?                       ; receives actual length written

ten     db      10                      ; used by b2a conversion routine 

ginfseg dw      ?                       ; global information segment
linfseg dw      ?                       ; local information segment

msg1    db      cr,lf,'Unexpected OS/2 Error',cr,lf
msg1_length equ $-msg1

msg2    db      escape,'[s'             ; save cursor position
        db      escape,'[0;70H'         ; move to 0,70
msg2a   db      '00:'                   ; hours
msg2b   db      '00:'                   ; minutes
msg2c   db      '00'                    ; seconds
        db      escape,'[u'             ; restore cursor position
msg2_length equ $-msg2

_DATA   ends


_TEXT   segment word public 'CODE'

        assume  cs:_TEXT,ds:DGROUP

main    proc    far

                                        ; get selectors for info segments
        push    ds                      ; receives global selector
        push    offset DGROUP:ginfseg
        push    ds                      ; receives local selector
        push    offset DGROUP:linfseg
        call    DosGetInfoSeg           ; transfer to OS/2      
        jerr    main2                   ; jump if couldn't get selectors

                                        ; create system semaphore...
        push    1                       ; exclusive ownership not needed
        push    ds                      ; variable to receive semaphore handle
        push    offset DGROUP:shandle
        push    ds                      ; address of semaphore name
        push    offset DGROUP:sname
        call    DosCreateSem            ; transfer to OS/2
        jerr    main2                   ; jump if sem already exists

        push    0                       ; now create periodic timer...
        push    1000                    ; double word for 1000 msec.
        push    word ptr shandle+2      ; handle of our system semaphore
        push    word ptr shandle
        push    ds                      ; variable to receive timer handle
        push    offset DGROUP:thandle
        call    DosTimerStart           ; transfer to OS/2      
        jerr    main2                   ; jump if timer create failed

main1:                                  ; display time at 1-sec intervals

                                        ; first set our semaphore...
        push    word ptr shandle+2      ; system semaphore handle
        push    word ptr shandle
        call    DosSemSet               ; transfer to OS/2      
        jerr    main2                   ; terminate if set failed

        call    display                 ; display current time

        push    word ptr shandle+2      ; wait for 1 second timer
        push    word ptr shandle        ; to be triggered
        push    -1                      ; timeout = -1 = wait indefinitely
        push    -1
        call    DosSemWait              ; transfer to OS/2
        or      ax,ax                   ; did wait fail?
        jz      main1                   ; no, loop to display again

main2:                                  ; display error message...
        push    stdout                  ; standard output handle
        push    ds                      ; message address
        push    offset msg1
        push    msg1_length             ; message length
        push    ds                      ; receives actual length written        
        push    offset wlen
        call    DosWrite                ; transfer to OS/2

        push    1                       ; terminate all threads
        push    1                       ; return error code
        call    DosExit                 ; final exit to OS/2

main    endp


display proc    near                    ; display current time

        mov     es,ginfseg              ; get selector for global
                                        ; read-only information segment 

        mov     al,byte ptr es:[8]      ; convert hours to ASCII
        cbw
        div     ten
        add     ax,'00'
        mov     word ptr msg2a,ax

        mov     al,byte ptr es:[9]      ; convert minutes to ASCII
        cbw
        div     ten
        add     ax,'00'
        mov     word ptr msg2b,ax

        mov     al,byte ptr es:[0ah]    ; convert seconds to ASCII
        cbw
        div     ten
        add     ax,'00'
        mov     word ptr msg2c,ax

                                        ; display time...
        push    stdout                  ; standard output handle
        push    ds                      ; address of message
        push    offset msg2
        push    msg2_length             ; length of message
        push    ds                      ; receives bytes written
        push    offset wlen
        call    DosWrite                ; transfer to OS/2

        ret                             ; back to caller

display endp

_TEXT   ends

        end     main
