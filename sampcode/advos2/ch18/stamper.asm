        title   STAMPER: a simple keyboard monitor
        page    55,132
        .286
        .sall
; 
; STAMPER.EXE: a simple OS/2 monitor that inserts
; a date or time stamp into the keyboard data stream.
; Copyright (C) 1987 Ray Duncan
;
; Alt-D  is the hot-key for a date-stamp.
; Alt-T  is the hot-key for a time-stamp.
; Alt-X  causes the STAMPER.EXE monitor to exit.
;
; In order to keep this example program simple, it
; contains minimal error handling and does not check
; for previously loaded copies of itself in the same
; screen group etc.  For a more complete example, see
; the SNAP.ASM or SNAP.C program.
;
; Build with:   C>MASM STAMPER;
;               C>LINK STAMPER,,,OS2,STAMPER;
;
; Usage:        C>DETACH STAMPER

stdin   equ     0                       ; standard device handles
stdout  equ     1
stderr  equ     2

cr      equ     0dh                     ; ASCII carriage return
lf      equ     0ah                     ; ASCII line feed

                                        ; Hot-key definitions:
datekey equ     20h                     ; Alt-D = insert date
timekey equ     14h                     ; Alt-T = insert time
exitkey equ     2dh                     ; Alt-X = exit program

        extrn   DosExit:far             ; references to OS/2 API
        extrn   DosGetInfoSeg:far          
        extrn   DosMonClose:far
        extrn   DosMonOpen:far          
        extrn   DosMonRead:far
        extrn   DosMonReg:far
        extrn   DosMonWrite:far
        extrn   DosSetPrty:far
        extrn   DosSleep:far
        extrn   DosWrite:far
        extrn   VioEndPopUp:far
        extrn   VioGetAnsi:far
        extrn   VioPopUp:far
        extrn   VioWrtCharStr:far

jerr    macro   target                  ;; Macro to test return code
        local   zero                    ;; in AX and jump if non-zero.
        or      ax,ax                   ;; Uses JMP DISP16 to avoid
        jz      zero                    ;; branch out of range errors
        jmp     target
zero:
        endm

DGROUP  group   _DATA

_DATA   segment word public 'DATA'

kname   db      'KBD$',0                ; device name of keyboard
khandle dw      0                       ; handle from DosMonOpen

gseg    dw      ?                       ; global information segment
lseg    dw      ?                       ; local information segment
scrgrp  dw      ?                       ; foreground screen group 
ansi    dw      ?                       ; receives ANSI state
popflag dw      1                       ; wait for PopUp window
wlen    dw      ?                       ; receives length written

dstr    db      'mm/dd/yy',0            ; strings used by time and
tstr    db      'hh:mm',0               ; date formatting routines

monin   dw      128,64 dup (0)          ; buffers for monitor
monout  dw      128,64 dup (0)

packet  db      128 dup (0)             ; buffer for kbd data packet
pktlen  dw      ?                       ; contains max buffer length; 
                                        ; receives actual data length

msg1    db      cr,lf,'Start STAMPER with DETACH!',cr,lf
msg1_len equ $-msg1             

msg2    db      'STAMPER utility installed'
msg2_len equ $-msg2

msg3    db      'Alt-D to insert date stamp,'
msg3_len equ $-msg3

msg4    db      'Alt-T to insert time stamp,'
msg4_len equ $-msg4

msg5    db      'Alt-X to shut down STAMPER.'
msg5_len equ $-msg5

msg6    db      'STAMPER utility deactivated.'
msg6_len equ $-msg6

_DATA   ends


_TEXT   segment word public 'CODE'

        assume  cs:_TEXT,ds:DGROUP,ss:DGROUP

main    proc    far                     ; entry point from OS/2

                                        ; get info segment selectors...
        push    ds                      ; receives global selector
        push    offset DGROUP:gseg      
        push    ds                      ; receives local selector
        push    offset DGROUP:lseg
        call    DosGetInfoSeg           ; transfer to OS/2
        jerr    main6                   ; give up if can't get selectors

                                        ; make sure we are detached...
        push    ds                      ; receives ANSI state flag
        push    offset DGROUP:ansi
        push    0                       ; VIO handle
        call    VioGetAnsi
        or      ax,ax                   ; call should fail, otherwise
        jnz     main1                   ; we are not detached.

                                        ; not detached, display error 
                                        ; message and exit...
        push    stderr                  ; handle for standard error
        push    ds                      ; address of message
        push    offset DGROUP:msg1
        push    msg1_len                ; length of message
        push    ds                      ; receives bytes written
        push    offset DGROUP:wlen
        call    DosWrite                ; transfer to OS/2
        jmp     main6                   ; go exit with error code

main1:  mov     es,gseg                 ; get foreground screen group 
        mov     al,byte ptr es:[0018h]  ; from global info segment
        cbw
        mov     scrgrp,ax               ; and save it   

                                        ; open monitor connection... 
        push    ds                      ; address of device name KBD$ 
        push    offset DGROUP:kname
        push    ds                      ; receives monitor handle
        push    offset DGROUP:khandle
        call    DosMonOpen              ; transfer to OS/2
        jerr    main6                   ; give up if can't open it

                                        ; register as keyboard monitor...
        push    khandle                 ; handle from DosMonOpen
        push    ds                      ; addr of monitor input buffer
        push    offset DGROUP:monin
        push    ds                      ; addr of monitor output buffer
        push    offset DGROUP:monout
        push    1                       ; request front of list
        push    scrgrp                  ; index = screen group 
        call    DosMonReg               ; transfer to OS/2
        jerr    main6                   ; give up if can't register

        call    signon                  ; else announce our presence 

                                        ; promote main thread to time-
                                        ; critical priority...
        push    2                       ; scope = single thread this process
        push    3                       ; class = time-critical
        push    0                       ; level = 0 within class
        push    1                       ; thread ID 
        call    DosSetPrty              ; transfer to OS/2

main2:                                  ; monitor the keyboard character
                                        ; stream; when hot key detected,
                                        ; insert the appropriate date or
                                        ; time stamp, or exit.

        mov     pktlen,pktlen-packet    ; set max buffer length for read

                                        ; get next keyboard data packet
        push    ds                      ; address of monitor input buffer
        push    offset DGROUP:monin
        push    0                       ; wait until data available
        push    ds
        push    offset DGROUP:packet    ; buffer for keyboard data packet
        push    ds
        push    offset DGROUP:pktlen    ; receives length of data packet
        call    DosMonRead              ; transfer to OS/2

        cmp     byte ptr packet+2,0     ; is this extended code?
        jnz     main4                   ; no, just pass it on

        cmp     byte ptr packet+3,exitkey
        jz      main5                   ; jump if exit hot-key

        cmp     byte ptr packet+3,timekey
        jnz     main3                   ; jump if not time hot-key

        cmp     word ptr packet+12,0    ; discard break packets
        jnz     main2

        call    time                    ; insert the time stamp

        jmp     main2                   ; discard this key

main3:                                  ; is it datestamp hot-key?
        cmp     byte ptr packet+3,datekey
        jnz     main4                   ; no, jump

        cmp     word ptr packet+12,0    ; discard break packets
        jnz     main2

        call    date                    ; insert the date stamp

        jmp     main2                   ; discard this key

main4:                                  ; Not hot-key, pass packet on.
        push    ds                      ; address of monitor output buffer
        push    offset DGROUP:monout
        push    ds                      ; address of keyboard data packet
        push    offset DGROUP:packet    
        push    pktlen                  ; length of data packet
        call    DosMonWrite             ; transfer to OS/2      
        
        jmp     main2                   ; wait for another packet

main5:                                  ; hotkey for de-install detected

        cmp     word ptr packet+12,0    ; make sure it's Break packet
        jz      main2                   ; if not just discard it

        push    khandle                 ; close the monitor connection
        call    DosMonClose             ; transfer to OS/2

        call    signoff                 ; announce STAMPER exit 

        push    1                       ; terminate all threads
        push    0                       ; return success code
        call    DosExit                 ; final exit to OS/2

main6:                                  ; common error exit point...
        push    1                       ; terminate all threads
        push    1                       ; return error code
        call    DosExit                 ; final exit to OS/2

main    endp


date    proc    near                    ; format and insert date stamp

        mov     es,gseg                 ; get selector for global
                                        ; read-only information segment 

        mov     al,byte ptr es:[11h]    ; convert month to ASCII
        aam
        add     ax,'00'
        xchg    al,ah
        mov     word ptr dstr,ax

        mov     al,byte ptr es:[10h]    ; convert day to ASCII
        aam
        add     ax,'00'
        xchg    al,ah
        mov     word ptr dstr+3,ax

        mov     ax,word ptr es:[12h]    ; convert year to ASCII
        sub     ax,1900
        aam
        add     ax,'00'
        xchg    al,ah
        mov     word ptr dstr+6,ax

        mov     si,offset DGROUP:dstr   ; insert date stamp string
        call    stuff                   ; into keyboard data stream

        ret                             ; back to caller

date    endp


time    proc    near                    ; format and insert time stamp

        mov     es,gseg                 ; get selector for global
                                        ; read-only information segment 

        mov     al,byte ptr es:[8]      ; convert hours to ASCII
        aam
        add     ax,'00'
        xchg    al,ah
        mov     word ptr tstr,ax

        mov     al,byte ptr es:[9]      ; convert minutes to ASCII
        aam
        add     ax,'00'
        xchg    al,ah
        mov     word ptr tstr+3,ax

        mov     si,offset DGROUP:tstr   ; insert time stamp string
        call    stuff                   ; into keyboard data stream

        ret                             ; back to caller

time    endp


stuff   proc    near                    ; insert string into keyboard
                                        ; data stream.  Call with
                                        ; SI = ASCIIZ string (null
                                        ;      is discarded)
                                        ; AL, SI destroyed.

stuff1: lodsb                           ; get next character
        or      al,al                   ; is it null?
        jnz     stuff2                  ; no, use it
        ret                             ; yes, exit

stuff2: mov     packet+2,al             ; place ASCII code into packet

                                        ; now send this character
                                        ; to the keyboard driver...
        push    ds                      ; monitor output buffer address
        push    offset DGROUP:monout
        push    ds                      ; keyboard data packet address
        push    offset DGROUP:packet    
        push    pktlen                  ; data packet length
        call    DosMonWrite             ; transfer to OS/2

        jmp     stuff1                  ; do another character  

stuff   endp


signon  proc    near                    ; use pop-up window to 
                                        ; display help message

        push    ds                      ; put up PopUp window
        push    offset DGROUP:popflag   ; (wait until available)
        push    0                       ; VIO handle
        call    VioPopUp                ; transfer to OS/2

        mov     dx,offset DGROUP:msg2   ; message address
        mov     cx,msg2_len             ; length
        mov     ax,9                    ; Y coordinate
        call    center                  ; display it

        mov     dx,offset DGROUP:msg3   ; message address
        mov     cx,msg3_len             ; length
        mov     ax,13                   ; Y coordinate
        call    center                  ; display it

        mov     dx,offset DGROUP:msg4   ; message address
        mov     cx,msg4_len             ; length
        mov     ax,15                   ; Y coordinate
        call    center                  ; display it

        mov     dx,offset DGROUP:msg5   ; message address
        mov     cx,msg5_len             ; length
        mov     ax,17                   ; Y coordinate
        call    center                  ; display it

        push    0                       ; pause for 3 seconds   
        push    3000                    ; (user must be quick reader!)
        call    DosSleep                ; transfer to OS/2

        push    0                       ; take down PopUp window
        call    VioEndPopUp             ; transfer to OS/2

        ret                             ; back to caller

signon  endp


signoff proc    near                    ; use pop-up window to
                                        ; announce exit

        push    ds                      ; put up PopUp window
        push    offset DGROUP:popflag   ; (wait until available)
        push    0                       ; VIO handle
        call    VioPopUp                ; transfer to OS/2

        mov     dx,offset DGROUP:msg6   ; address of signoff message
        mov     cx,msg6_len             ; length
        mov     ax,12                   ; Y coordinate
        call    center                  ; display it

        push    0                       ; pause for 1 second so
        push    1000                    ; user can read message
        call    DosSleep                ; transfer to OS/2

        push    0                       ; take down PopUp window
        call    VioEndPopUp             ; transfer to OS/2

        ret                             ; back to caller

signoff endp


center  proc    near                    ; center a message on screen
                                        ; call DX = msg offset,
                                        ; CX = length, AX = Y coordinate

        push    ds                      ; address of message
        push    dx
        push    cx                      ; length of message
        push    ax                      ; Y
        sub     cx,80                   ; X=((80-length)/2)
        neg     cx                      ; to center message
        shr     cx,1
        push    cx
        push    0                       ; VIO handle
        call    VioWrtCharStr           ; transfer to OS/2
        ret                             ; back to caller

center  endp

_TEXT   ends

        end     main
