         name      child
         title     'CHILD process'
;
; CHILD.EXE --- a simple process loaded by PARENT.EXE
; to demonstrate the MS-DOS EXEC call, Subfunction 00H.
;
; Ray Duncan, June 1987
;

stdin   equ     0                       ; standard input
stdout  equ     1                       ; standard output
stderr  equ     2                       ; standard error

cr      equ     0dh                     ; ASCII carriage return
lf      equ     0ah                     ; ASCII linefeed


DGROUP  group   _DATA,STACK


_TEXT   segment byte public 'CODE'      ; executable code segment

        assume  cs:_TEXT,ds:_DATA,ss:STACK

main    proc    far                     ; entry point from MS-DOS

        mov     ax,_DATA                ; set DS = our data segment
        mov     ds,ax

                                        ; display child message ...
        mov     dx,offset msg           ; DS:DX = address of message
        mov     cx,msg_len              ; CX = length of message
        mov     bx,stdout               ; BX = standard output handle 
        mov     ah,40h                  ; AH = fxn 40H, write file/device
        int     21h                     ; transfer to MS-DOS
        jc      main2                   ; jump if any error

        mov     ax,4c00h                ; no error, terminate child
        int     21h                     ; with return code = 0

main2:  mov     ax,4c01h                ; error, terminate child
        int     21h                     ; with return code = 1 

main    endp                            ; end of main procedure

_TEXT   ends


_DATA   segment para public 'DATA'      ; static & variable data segment

msg     db      cr,lf,'Child executing!',cr,lf
msg_len equ     $-msg

_DATA   ends


STACK   segment para stack 'STACK'

        dw      64 dup (?)

STACK   ends


        end     main                    ; defines program entry point
