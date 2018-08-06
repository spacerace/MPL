        .
        .
        .

_TEXT   segment word public 'CODE'      ; executable code segment

        assume  cs:_TEXT,ds:_DATA,ss:_STACK

main    proc    far             ; entry point from MS-DOS
                                ; CS = _TEXT segment,
                                ; DS = ES = PSP

        mov     ax,_DATA        ; set DS = our data segment
        mov     ds,ax

                                ; give back extra memory...
        mov     ax,es           ; let AX = segment of PSP base
        mov     bx,ss           ; and BX = segment of stack base
        sub     bx,ax           ; reserve seg stack - seg psp
        add     bx,stksize/16   ; plus paragraphs of stack
        inc     bx              ; round up
        mov     ah,4ah          ; Fxn 4AH = resize memory block
        int     21h
        jc      error

main    endp

_TEXT   ends


_DATA   segment word public 'DATA'      ; static & variable data

        .
        .
        .

_DATA   ends


STACK   segment para stack 'STACK'

        db      stksize dup (?)

STACK   ends

        end     main            ; defines program entry point
