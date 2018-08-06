        title   SCHEX Example directory search program
        page    55,132
        .286

; SCH-EX.ASM   Example of OS/2 directory search.
;              Finds and displays names of all files
;              in current directory with extension .ASM.
; Copyright (C) 1988 Ray Duncan 
;
; Build:        MASM SCHEX;
;               LINK SCHEX,,,OS2,SCHEX
;
; Usage:        SCHEX

stdin   equ     0                       ; standard input handle
stdout  equ     1                       ; standard output handle
stderr  equ     2                       ; standard error handle

cr      equ     0dh                     ; ASCII carriage return
lf      equ     0ah                     ; ASCII line feed

        extrn   DosExit:far
        extrn   DosFindClose:far
        extrn   DosFindFirst:far
        extrn   DosFindNext:far
        extrn   DosWrite:far

DGROUP  group   _DATA

_DATA   segment word public 'DATA'

fname   db      '*.ASM',0               ; target name for search

sbuf    db      64 dup (?)              ; receives search results
sbuf_len equ $-sbuf                     ; length of result buffer

dirhan  dw      ?                       ; receives search handle
schcnt  dw      ?                       ; receives match count
wlen    dw      ?                       ; receives bytes written
counter dw      0                       ; number of files matched

newline db      cr,lf                   ; carriage return-linefeed
newline_len equ $-newline

msg1    db      cr,lf
        db      'No matching files.'
        db      cr,lf
msg1_len equ $-msg1

_DATA   ends


_TEXT   segment word public 'CODE'

        assume  cs:_TEXT,ds:DGROUP

main    proc    far                     ; entry point from OS/2

        mov     dirhan,-1               ; request new search handle
        mov     schcnt,1                ; set max match count

                                        ; search for first match...
        push    ds                      ; search target filename
        push    offset DGROUP:fname
        push    ds                      ; receives search handle
        push    offset DGROUP:dirhan
        push    0                       ; attribute=normal files
        push    ds                      ; result buffer address
        push    offset DGROUP:sbuf
        push    sbuf_len                ; result buffer length
        push    ds                      ; receives match count
        push    offset DGROUP:schcnt                    
        push    0                       ; reserved DWORD 0
        push    0
        call    DosFindFirst            ; transfer to OS/2

        or      ax,ax                   ; any files found?
        jnz     main2                   ; no, exit

        cmp     schcnt,1                ; confirm match was found
        jne     main2                   ; no, exit
        
main1:                                  ; match was found...
        inc     counter                 ; count matching files

                                        ; display one filename...
        push    stdout                  ; standard output handle
        push    ds                      ; file name address
        push    offset DGROUP:sbuf+17h
        mov     al,byte ptr sbuf+16h    ; file name length
        xor     ah,ah
        push    ax
        push    ds                      ; receives bytes written
        push    offset DGROUP:wlen
        call    DosWrite                ; transfer to OS/2

                                        ; send new-line sequence
        push    stdout                  ; standard output handle
        push    ds                      ; new-line sequence address
        push    offset DGROUP:newline
        push    newline_len             ; new-line sequence length
        push    ds                      ; receives bytes written
        push    offset DGROUP:wlen
        call    DosWrite                ; transfer to OS/2

                                        ; search for next file...
        push    dirhan                  ; search handle
        push    ds                      ; result buffer address
        push    offset DGROUP:sbuf
        push    sbuf_len                ; result buffer length
        push    ds                      ; receives match count
        push    offset schcnt                   
        call    DosFindNext             ; transfer to OS/2

        or      ax,ax                   ; any more files?
        jnz     main2                   ; no, exit

        cmp     schcnt,1                ; confirm match was found 
        je      main1                   ; yes, proceed

main2:  push    dirhan                  ; release search handle...
        call    DosFindClose            ; transfer to OS/2

        cmp     counter,0               ; any files found?
        jne     main3                   ; yes

                                        ; no, display 'no matches'
        push    stdout                  ; standard output handle
        push    ds                      ; message address
        push    offset DGROUP:msg1
        push    msg1_len                ; message length
        push    ds                      ; receives bytes written
        push    offset DGROUP:wlen
        call    DosWrite                ; transfer to OS/2

main3:  push    1                       ; terminate all threads
        push    0                       ; return success code
        call    DosExit                 ; exit program

main    endp

_TEXT   ends

        end     main

