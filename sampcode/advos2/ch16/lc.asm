        title     LC.ASM --- lowercasing filter
        page      55,132
        .286
;
; LC.ASM        Filter to translate upper case 
;               chararacters to lower case.
; Copyright (C) 1988 Ray Duncan
;
; Build:        MASM LC;
;               LINK LC,,,OS2,LC;
;
; Usage:        LC [<source] [>destination]

stdin   equ     0                       ; standard input device
stdout  equ     1                       ; standard output device        
stderr  equ     2                       ; standard error device

cr      equ     0dh                     ; ASCII carriage return
lf      equ     0ah                     ; ASCII linefeed

bufsize equ     256                     ; max amount to read or write

        extrn   DosExit:far             ; references to OS/2 API
        extrn   DosRead:far
        extrn   DosWrite:far

DGROUP  group   _DATA           

_DATA   segment word public 'DATA'

input   db      bufsize dup (?)         ; storage for input line 
output  db      bufsize dup (?)         ; storage for output line

rlen    dw      ?                       ; receives bytes read   
wlen    dw      ?                       ; receives bytes written

_DATA   ends


_TEXT   segment word public 'CODE'

        assume  cs:_TEXT,ds:DGROUP

main    proc    far                     ; entry point from OS/2

        mov     ax,ds                   ; make DGROUP addressable
        mov     es,ax                   ; via ES register too

main1:                                  ; read line from standard input
        push    stdin                   ; standard input handle
        push    ds                      ; buffer address
        push    offset DGROUP:input
        push    bufsize                 ; buffer length
        push    ds                      ; receives bytes read
        push    offset DGROUP:rlen
        call    DosRead                 ; transfer to OS/2

        mov     ax,rlen                 ; get length of input
        or      ax,ax                   ; any characters read?
        jz      main2                   ; jump if end of stream

        call    translate               ; translate line if necessary

        or      ax,ax                   ; anything to output?
        jz      main1                   ; no, go read another line

                                        ; write line to standard output 
        push    stdout                  ; standard output handle
        push    ds                      ; buffer address
        push    offset DGROUP:output
        push    ax                      ; buffer length
        push    ds                      ; receives bytes written
        push    offset DGROUP:wlen
        call    DosWrite                ; transfer to OS/2

        jmp     main1                   ; go read another line

main2:                                  ; end of stream reached
        push    1                       ; 1 = end all threads
        push    0                       ; 0 = exit code
        call    DosExit                 ; final exit to OS/2

main    endp                            ; end of main procedure


; Copy input buffer to output buffer, translating
; any upper case characters to lower case.
;
; Call with:    AX = length of data in 'input' buffer.
;
; Return:       AX = length to write to standard output.

translate proc  near

        push    ax                      ; save original length

        mov     si,offset DGROUP:input  ; address of original line
        mov     di,offset DGROUP:output ; address for converted line
        mov     cx,ax                   ; length to convert
        jcxz    trans3                  ; exit if empty line

trans1:                                 ; convert char. by char...
        lodsb                           ; get input character
        cmp     al,'A'                  ; if not A-Z, leave it alone
        jb      trans2
        cmp     al,'Z'
        ja      trans2

        add     al,'a'-'A'              ; it's A-Z, convert it

trans2: stosb                           ; put into output buffer

        loop    trans1                  ; loop until all characters
                                        ; have been transferred

trans3: pop     ax                      ; get back line length 
        ret                             ; and return it in AX

translate endp

_TEXT   ends

        end     main            
