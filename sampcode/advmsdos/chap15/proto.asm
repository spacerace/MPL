         name      proto
         page      55,132
         title     PROTO.ASM --- prototype filter
;
; PROTO.ASM:  prototype character-oriented filter
;
; Copyright 1988 Ray Duncan
;
; Build:  MASM PROTO;
;         LINK PROTO;
;
; Usage:  PROTO
;

stdin   equ     0               ; standard input handle
stdout  equ     1               ; standard output handle
stderr  equ     2               ; standard error handle

cr      equ     0dh             ; ASCII carriage return
lf      equ     0ah             ; ASCII linefeed


_TEXT   segment word public 'CODE'

        assume  cs:_TEXT,ds:_DATA,ss:STACK

main    proc    far             ; entry point from MS-DOS

        mov     ax,_DATA        ; set DS = our data segment
        mov     ds,ax

main1:                          ; read char. from stdin...
        mov     dx,offset char  ; DS:DX = buffer address
        mov     cx,1            ; CX = length to read
        mov     bx,stdin        ; BX = standard input handle
        mov     ah,3fh          ; Fxn 3FH = read 
        int     21h             ; transfer to MS-DOS
        jc      main3           ; if error, terminate

        cmp     ax,1            ; any character read?
        jne     main2           ; if end of file, terminate

        call    translate       ; translate character 

                                ; write char. to stdout...
        mov     dx,offset char  ; DS:DX = buffer address
        mov     cx,1            ; CX = length to write
        mov     bx,stdout       ; BX = standard output handle
        mov     ah,40h          ; Fxn 40H = write
        int     21h             ; transfer to MS-DOS
        jc      main3           ; if error, terminate

        cmp     ax,1            ; was character written?
        jne     main3           ; if disk full, terminate 

        jmp     main1           ; get another character

main2:                          ; end of file reached
        mov     ax,4c00h        ; Fxn 4CH = terminate
                                ; return code = 0
        int     21h             ; transfer to MS-DOS

main3:                          ; error or disk full    
        mov     ax,4c01h        ; Fxn 4CH = terminate
                                ; return code = 1
        int     21h             ; transfer to MS-DOS

main    endp

;
; Perform any necessary translation on character 
; from standard input stored in variable 'char'.
; This example just leaves character unchanged.
;
translate proc  near

        ret                     ; does nothing

translate endp

_TEXT   ends


_DATA   segment word public 'DATA'

char    db      0               ; storage for input character

_DATA   ends


STACK   segment para stack 'STACK'

        dw      64 dup (?)

STACK   ends

        end     main            ; defines program entry point
