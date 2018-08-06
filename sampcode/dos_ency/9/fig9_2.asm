        .
        .
        .
_TEXT   segment para public 'CODE'

        org     100h

        assume  cs:_TEXT,ds:_TEXT,es:_TEXT,ss:_TEXT

main    proc    near            ; entry point from MS-DOS
                                ; CS = DS = ES = SS = PSP

                                ; first move our stack
        mov     sp,offset stk   ; to a safe place...

                                ; now release extra memory...
        mov     bx,offset stk   ; calculate paragraphs to keep
        mov     cl,4            ; (divide offset of end of
        shr     bx,cl           ; program by 16 and round up)
        inc     bx
        mov     ah,4ah          ; Fxn 4AH = resize mem block
        int     21h             ; transfer to MS-DOS
        jc      error           ; jump if resize failed
        .
        .                       ; otherwise go on with work...
        .

main    endp

        .
        .
        .

        dw      64 dup (?)
stk     equ     $               ; base of new stack area

_TEXT   ends

        end     main            ; defines program entry point
