        title   VOLNAME - displays volume label
        page    55,132
        .286
; 
; VOLNAME.ASM --- displays volume label for current drive,
;                 demonstrates use of DosQFSInfo.
; Copyright (C) October 1986 Ray Duncan 
;
; Build:        MASM VOLNAME;
;               LINK VOLNAME,,,OS2,VOLNAME;
;
; Usage:        VOLNAME

stdin   equ     0                       ; standard input handle
stdout  equ     1                       ; standard output handle
stderr  equ     2                       ; standard error handle

cr      equ     0dh                     ; ASCII carriage return
lf      equ     0ah                     ; ASCII line feed

        extrn   DosExit:far             ; references to OS/2 API
        extrn   DosQCurDisk:far
        extrn   DosQFSInfo:far  
        extrn   DosWrite:far

_vlinfo struc                           ; volume label structure
date    dw      ?                       ; date of creation      
time    dw      ?                       ; time of creation
count   db      ?                       ; length of volume label
vname   db      13 dup (?)              ; volume label text
_vlinfo ends

DGROUP  group   _DATA

_DATA   segment word public 'DATA'

wlen    dw      ?                       ; receives bytes written

vlinf   _vlinfo <>                      ; volume label info
vlinf_len equ $-vlinf

drive   dw      0                       ; receives drive ID
drvmap  dd      0                       ; receives drive bitmap

msg1    db      cr,lf,'Volume in drive '
msg1a   db      'X has no label.',cr,lf
msg1_length equ $-msg1

msg2    db      cr,lf
msg2_length equ $-msg2

msg3    db      cr,lf,'Volume in drive ' 
msg3a   db      'X is '
msg3_length equ $-msg3

_DATA   ends


_TEXT   segment word public 'CODE'

        assume  cs:_TEXT,ds:DGROUP

volname proc    far                     ; entry point from OS/2
                                
                                        ; get current drive...
        push    ds                      ; receives drive code
        push    offset DGROUP:drive
        push    ds                      ; receives bitmap for
        push    offset DGROUP:drvmap    ; available drives
        call    DosQCurDisk             ; transfer to OS/2
        or      ax,ax                   ; did function succeed?
        jz      vol1                    ; yes, proceed
        jmp     vol5                    ; exit if function failed

vol1:   mov     al,'A'-1                ; convert drive code to
        add     ax,drive                ; ASCII letter and store
        mov     msg3a,al                ; for output
        mov     msg1a,al

                                        ; get volume label...
        push    0                       ; drive = default
        push    2                       ; file info level 2
        push    ds                      ; buffer receives volume
        push    offset DGROUP:vlinf     ; date, time, and name
        push    vlinf_len               ; size of buffer
        call    DosQFSInfo              ; transfer to OS/2
        or      ax,ax                   ; call successful?
        jnz     vol2                    ; jump, got volume label
                                        
        cmp     vlinf.count,0           ; null string?
        jne     vol3                    ; no, go display label

vol2:                                   ; volume has no label...
        push    stdout                  ; standard output handle
        push    ds                      ; message address
        push    offset DGROUP:msg1
        push    msg1_length             ; message length
        push    ds                      ; receives actual bytes written
        push    offset DGROUP:wlen
        call    DosWrite                ; transfer to OS/2
        jmp     vol4                    ; now eixt

vol3:                                   ; display initial message...
        push    stdout                  ; standard output handle
        push    ds                      ; message address
        push    offset DGROUP:msg3
        push    msg3_length             ; message length
        push    ds                      ; receives actual bytes written
        push    offset DGROUP:wlen      
        call    DosWrite                ; transfer to OS/2

                                        ; display actual volume label...
        push    stdout                  ; standard output handle
        push    ds                      ; label address
        push    offset DGROUP:vlinf.vname
        mov     al,vlinf.count          ; label length
        xor     ah,ah
        push    ax
        push    ds                      ; receives bytes written
        push    offset DGROUP:wlen
        call    DosWrite                ; transfer to OS/2

                                        ; output CR-LF sequence...
        push    stdout                  ; standard output handle
        push    ds                      ; address of CR-LF
        push    offset DGROUP:msg2
        push    msg2_length             ; length of CR-LF
        push    ds                      ; receives bytes written
        push    offset DGROUP:wlen
        call    DosWrite                ; transfer to OS/2

vol4:                                   ; success exit point...
        push    1                       ; terminate all threads
        push    0                       ; return code=0 (success)
        call    DosExit                 ; transfer to OS/2

vol5:                                   ; error exit point...
        push    1                       ; terminate all threads
        push    1                       ; return code=1 (error)
        call    DosExit                 ; transfer to OS/2

volname endp

_TEXT   ends

        end     volname

