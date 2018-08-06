	title	  SHOWARGS -- ASMHELP.DLL Demo
        page      55,132
	.286

;
; SHOWARGS.ASM
;
; Demonstrates parsing of command line by calls to ASMHELP.DLL.
;
; Assemble with:  C> masm showargs.asm;
; Link with:  C> link showargs,,,os2,showargs
;
; Usage is:  C> showargs [argument(s)]
;
; Copyright (C) 1988 Ray Duncan
;

stdin   equ     0               ; standard input handle
stdout  equ     1               ; standard output handle
stderr  equ     2               ; standard error handle

cr      equ     0dh             ; ASCII carriage return
lf	equ	0ah		; ASCII linefeed
blank   equ     020h            ; ASCII blank
tab     equ     09h             ; ASCII tab 

                                ; references to ASMHELP.DLL
        extrn   ARGC:far        ; returns argument count
        extrn   ARGV:far        ; returns pointer to argument

        extrn   DosWrite:far    ; references to OS/2
        extrn   DosExit:far
        extrn   DosGetEnv:far

DGROUP  group   _DATA

_DATA   segment word public 'DATA'

argno   dw      0               ; receives argument count
argptr  dw      0,0             ; receives argument pointer
arglen  dw      0               ; receives argument length

curarg	dw	0		; current command line argument

wlen    dw      0               ; bytes actually written

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

                                ; get number of command
                                ;  line arguments...

        push    ds              ; receives argument count
        push    offset DGROUP:argno
        call    argc            ; call ASMHELP.DLL

        mov     ax,argno        ; convert count to ASCII
        mov     bx,offset msg1a ; for output
        call    b2dec

                                ; now display number
                                ; of arguments...

        push    stdout          ; standard output handle
        push    ds              ; address of message
        push    offset DGROUP:msg1
        push    msg1_len        ; length of message
        push    ds              ; receives bytes written
        push    offset DGROUP:wlen
        call    DosWrite        ; transfer to OS/2

main1:                          ; display next argument...

        mov     ax,curarg       ; are we all done?
        cmp     ax,argno
        je      main2           ; yes, exit

        mov     bx,offset msg2a ; no, convert argument number
        call    b2dec

                                ; display argument number...
        push    stdout          ; standard output handle
        push    ds              ; address of message
        push    offset DGROUP:msg2
        push    msg2_len        ; length of message
        push    ds              ; receives bytes written
        push    offset DGROUP:wlen
        call    DosWrite        ; transfer to OS/2

                                ; get argument pointer...
        push    curarg          ; argument number
        push    ds              ; receives pointer
        push    offset DGROUP:argptr
        push    ds              ; receives length
        push    offset DGROUP:arglen
        call    argv            ; call ASMHELP.DLL

                                ; display the argument...
        push    stdout          ; standard output handle
        push    argptr+2        ; pointer to argument
        push    argptr
        push    arglen          ; length of argument
        push    ds              ; receives bytes written
        push    offset DGROUP:wlen
        call    DosWrite        ; transfer to OS/2

        inc     word ptr curarg ; go to next argument
        jmp     main1

main2:                          ; common exit point
        push    1               ; terminate all threads
        push    0               ; return code = zero
        call    DosExit         ; transfer to OS/2

main    endp


b2dec   proc    near            ; convert binary value 0-99
                                ;  to two decimal ASCII 
                                ; call with 
                                ; AL = binary data
				; BX = address for 2 chars.
        
        aam                     ; divide AL by 10, leaving
				; AH = quotient, AL = remainder
        add     ax,'00'         ; convert to ASCII
	mov	[bx],ah 	; store tens digit
	mov	[bx+1],al	; store ones digit
        ret                     ; return to caller

b2dec   endp

_TEXT   ends

        end     main            ; defines entry point
