        title     SHOWARGS -- ARGC and ARGV demo
        page      55,132
        .286
;
; SHOWARGS.ASM --- demonstrate command line parsing with
;                  ARGC.ASM and ARGV.ASM (OS/2 version)
;
; Copyright (C) 1987 Ray Duncan
;

stdin   equ     0               ; standard input handle
stdout  equ     1               ; standard output handle
stderr  equ     2               ; standard error handle

cr      equ     0dh             ; ASCII carriage return
lf      equ     0ah             ; ASCII line feed
blank   equ     020h            ; ASCII blank
tab     equ     09h             ; ASCII tab 

        extrn   argc:near       ; returns argument count
        extrn   argv:near       ; returns argument pointer

                                ; OS/2 API functions
        extrn   DosWrite:far    ; write file or device
        extrn   DosExit:far     ; terminate process


DGROUP  group   _DATA

_DATA   segment word public 'DATA'

curarg  dw      0               ; current command line argument
totargs dw      0               ; total command line arguments

wlen    dw      ?               ; bytes actually written

msg1    db      cr,lf
        db      'The command line contains '
msg1a   db      'xx arguments'
msg1_len equ $-msg1

msg2    db      cr,lf
        db      'Argument '
msg2a   db      'xx is:  '
msg2_len equ $-msg2

_DATA   ends


_TEXT   segment word public 'CODE'

        assume  cs:_TEXT,ds:DGROUP

main    proc    far             ; entry point from OS/2

        call    argc            ; get and save number of 
        mov     totargs,ax      ; command line arguments

        mov     bx,offset msg1a ; convert argument count 
        call    b2dec           ; to ASCII for output

                                ; display argument count
        push    stdout          ; standard output handle
        push    ds              ; address of message
        push    offset DGROUP:msg1
        push    msg1_len        ; length of message
        push    ds              ; receives bytes written
        push    offset DGROUP:wlen
        call    DosWrite        ; transfer to OS/2

main1:  mov     ax,curarg       ; display next argument
        cmp     ax,totargs      ; are we all done?      
        je      main2           ; yes, exit

        mov     bx,offset msg2a ; no, convert argument 
        call    b2dec           ; number to ASCII

                                ; display argument number
        push    stdout          ; standard output handle
        push    ds              ; address of message
        push    offset DGROUP:msg2
        push    msg2_len        ; length of message
        push    ds              ; receives bytes written
        push    offset DGROUP:wlen
        call    DosWrite        ; transfer to OS/2

        mov     ax,curarg       ; now get actual argument
        call    argv            ; ES:BX=addr,AX=length

        push    stdout          ; standard output handle
        push    es              ; command argument address
        push    bx
        push    ax              ; command argument length
        push    ds              ; receives bytes written
        push    offset DGROUP:wlen
        call    DosWrite        ; transfer to OS/2

        inc     word ptr curarg ; go to next argument
        jmp     main1

main2:  push    1               ; terminate process
        push    0               ; return code = zero
        call    DosExit         ; final exit to OS/2

main    endp


b2dec   proc    near            ; convert binary 0-99
                                ; to decimal ASCII
                                ; call with 
                                ; AL = binary data
                                ; BX = addr. for 2 chars.
        
        aam                     ; divide AL by 10, leaving
                                ; AH=quotient, AL=remainder
        add     ax,'00'         ; convert to ASCII
        mov     [bx],ah         ; store ten's digit
        mov     [bx+1],al       ; store one's digit
        ret                     ; return to caller

b2dec   endp

_TEXT   ends

        end     main            ; defines entry point
