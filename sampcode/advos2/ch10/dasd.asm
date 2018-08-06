        title   DASD --- direct disk access demo
        page    55,132
        .286

; DASD.EXE  --- Demonstrates direct disk access under OS/2.
;               Reads the boot sector for the current drive
;               into a buffer, then writes it into a file
;               named \BOOT.BIN on the same drive.
;               The lock operation will fail if any files
;               are open on the current drive (you can't
;               use it on the boot drive, for example).
; Copyright (C) Ray Duncan October 1988
;
; Build:        MASM DASD;
;               LINK DASD,,,OS2,DASD;
;
; Usage:        DASD

stdin   equ     0                       ; standard input handle
stdout  equ     1                       ; standard output handle
stderr  equ     2                       ; standard error handle

cr      equ     0dh                     ; ASCII carriage return
lf      equ     0ah                     ; ASCII line feed

secsize equ     512                     ; physical sector size

        extrn   DosChgFilePtr:far       ; references to OS/2 API
        extrn   DosClose:far
        extrn   DosDevIOCtl:far
        extrn   DosExit:far
        extrn   DosOpen:far
        extrn   DosQCurDisk:far
        extrn   DosRead:far
        extrn   DosWrite:far

DGROUP  group   _DATA

_DATA   segment word public 'DATA'

dname   db      'X:',0                  ; logical drive identifier
dhandle dw      ?                       ; receives drive handle
daction dw      ?                       ; receives DosOpen action

drive   dw      0                       ; receives drive ID
drvmap  dd      0                       ; receives drive bitmap

fname   db      '\BOOT.BIN',0           ; file to receive boot sector
rlen    dw      ?                       ; actual bytes read
wlen    dw      ?                       ; actual bytes written

bootsec db      secsize dup (?)         ; boot sector read here

parblk  db      0                       ; DosDevIOCtl dummy parameter

msg1    db      cr,lf
        db      'DosQCurDisk failed.'
        db      cr,lf
msg1_len equ $-msg1

msg2    db      cr,lf
        db      'Can''t open drive '
msg2a   db      'X:'
        db      cr,lf
msg2_len equ $-msg2

msg3    db      cr,lf
        db      'Can''t lock drive '
msg3a   db      'X:'
        db      cr,lf
msg3_len equ $-msg3

msg4    db      cr,lf
        db      'Read of boot sector failed.'
        db      cr,lf
msg4_len equ $-msg4

msg5    db      cr,lf
        db      'Can''t create file BOOT.BIN'
        db      cr,lf
msg5_len equ $-msg5

msg6    db      cr,lf
        db      'Can''t write file BOOT.BIN'
        db      cr,lf
msg6_len equ $-msg6

msg7    db      cr,lf
        db      'Boot sector from drive '
msg7a   db      'X: written into file '
msg7b   db      'X:BOOT.BIN.'
        db      cr,lf
msg7_len equ $-msg7

_DATA   ends


_TEXT   segment word public 'CODE'

        assume  cs:_TEXT,ds:DGROUP

main    proc    far

                                        ; get current drive...
        push    ds                      ; receives drive code
        push    offset DGROUP:drive
        push    ds                      ; receives bitmap for
        push    offset DGROUP:drvmap    ; available drives
        call    DosQCurDisk             ; transfer to OS/2
        or      ax,ax                   ; did function succeed?
        jz      main1                   ; yes, proceed

        mov     dx,offset DGROUP:msg1   ; no, display error message
        mov     cx,msg1_len             ; and exit
        jmp     error

main1:  mov     ax,drive                ; convert drive code to
        add     al,'A'-1                ; ASCII letter and store
        mov     dname,al                ; into drive name string
        mov     msg2a,al                ; and error messages    
        mov     msg3a,al
        mov     msg7a,al
        mov     msg7b,al
                                        ; open current drive for 
                                        ; direct access ...
        push    ds                      ; address of drive name
        push    offset DGROUP:dname
        push    ds                      ; receives drive handle
        push    offset DGROUP:dhandle
        push    ds                      ; receives DosOpen action
        push    offset DGROUP:daction
        push    0                       ; file allocation (N/A)
        push    0
        push    0                       ; file attribute (N/A)
        push    01h                     ; action: open if exists,
                                        ;         fail if doesn't
        push    8012h                   ; mode: DASD, read/write, 
                                        ;       deny all
        push    0                       ; reserved DWORD 0
        push    0
        call    DosOpen                 ; transfer to OS/2
        or      ax,ax                   ; was open successful?
        jz      main2                   ; yes, proceed

        mov     dx,offset DGROUP:msg2   ; no, display error message
        mov     cx,msg2_len             ; and exit
        jmp     error

main2:                                  ; lock logical drive...
        push    0                       ; data buffer address
        push    0                       ; (not needed)
        push    ds                      ; parameter buffer address
        push    offset DGROUP:parblk
        push    0                       ; function
        push    8                       ; category
        push    dhandle                 ; drive handle
        call    DosDevIOCtl             ; transfer to OS/2
        or      ax,ax                   ; was lock successful?
        jz      main3                   ; yes, proceed

        mov     dx,offset DGROUP:msg3   ; no, display error message
        mov     cx,msg3_len             ; and exit
        jmp     error

main3:                                  ; now read boot sector...
        push    dhandle                 ; drive handle
        push    ds                      ; buffer address
        push    offset DGROUP:bootsec
        push    secsize                 ; buffer length
        push    ds                      ; receives actual length
        push    offset DGROUP:rlen
        call    DosRead                 ; transfer to OS/2
        or      ax,ax                   ; read successful?
        jnz     main4                   ; no, read failed

        cmp     rlen,secsize            ; actual = expected size?
        je      main5                   ; yes, proceed

main4:                                  ; read failed... 
        mov     dx,offset DGROUP:msg4   ; display error message
        mov     cx,msg4_len             ; and exit
        jmp     error

main5:                                  ; unlock logical drive...
        push    0                       ; data buffer address
        push    0                       ; (not needed)
        push    ds                      ; parameter buffer address
        push    offset DGROUP:parblk
        push    1                       ; function
        push    8                       ; category
        push    dhandle                 ; drive handle
        call    DosDevIOCtl             ; transfer to OS/2

                                        ; close logical drive...
        push    dhandle                 ; drive handle
        call    DosClose                ; transfer to OS/2

                                        ; now open file BOOT.BIN...
        push    ds                      ; address of filename
        push    offset DGROUP:fname
        push    ds                      ; receives file handle
        push    offset DGROUP:dhandle
        push    ds                      ; receives DosOpen action
        push    offset DGROUP:daction
        push    0                       ; file allocation (N/A)
        push    0
        push    0                       ; file attribute (N/A)
        push    12h                     ; action: create or replace
        push    11h                     ; mode: write-only, deny all
        push    0                       ; reserved DWORD 0
        push    0
        call    DosOpen                 ; transfer to OS/2
        or      ax,ax                   ; was open successful?
        jz      main6                   ; yes, proceed

        mov     dx,offset DGROUP:msg5   ; display error message
        mov     cx,msg5_len             ; and exit
        jmp     error

main6:                                  ; write sector into BOOT.BIN
        push    dhandle                 ; file handle
        push    ds                      ; address of data
        push    offset DGROUP:bootsec
        push    secsize                 ; length of data
        push    ds                      ; receives bytes written
        push    offset DGROUP:wlen
        call    DosWrite                ; transfer to OS/2
        or      ax,ax                   ; did write succeed?
        jnz     main7                   ; no, display error msg.
        cmp     wlen,secsize            ; was write complete?
        je      main8                   ; yes, jump

main7:                                  ; write failed...
        mov     dx,offset DGROUP:msg6   ; display error message
        mov     cx,msg6_len             ; and exit
        jmp     error

main8:  push    dhandle                 ; close the file
        call    DosClose                ; transfer to OS/2

                                        ; display success message...
        push    stdout                  ; standard output handle
        push    ds                      ; message address
        push    offset DGROUP:msg7                      
        push    msg7_len                ; message length
        push    ds                      ; receives bytes written        
        push    offset DGROUP:wlen
        call    DosWrite                ; transfer to OS/2      

        push    1                       ; terminate all threads
        push    0                       ; return success code
        call    DosExit                 ; exit program

error:                                  ; display error message...
        push    stderr                  ; standard error handle
        push    ds                      ; message address
        push    dx                      
        push    cx                      ; message length
        push    ds                      ; receives bytes written        
        push    offset DGROUP:wlen
        call    DosWrite                ; transfer to OS/2      

        push    1                       ; terminate all threads
        push    1                       ; return error code
        call    DosExit                 ; exit program

main    endp

_TEXT   ends


        end     main
