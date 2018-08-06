        .
        .
        .
myflag  dw      ?                       ; variable to be incremented
                                        ; on each timer-tick interrupt

oldint8 dd      ?                       ; contains address of previous
                                        ; timer-tick interrupt handler
        .
        .                               ; get the previous contents
        .                               ; of the Interrupt 0BH vector...
        mov     ax,3508h                ; AH = 35H (Get Interrupt Vector)
        int     21h                     ; AL = Interrupt number (08H)
        mov     word ptr oldint8,bx     ; save the address of
        mov     word ptr oldint8+2,es   ; the previous Int 08H Handler
        mov     dx,seg myint8           ; put address of the new
        mov     ds,dx                   ; interrupt handler into DS:DX
        mov     dx,offset myint8        ; and call MS-DOS to set vector
        mov     ax,2508h                ; AH = 25H (Set Interrupt Vector)
        int     21h                     ; AL = Interrupt number (08H)
        .
        .
        .
myint8:                                 ; this is the new handler
                                        ; for Interrupt 08H

        inc     cs:myflag               ; increment variable on each
                                        ; timer-tick interrupt

        jmp     dword ptr cs:[oldint8]  ; then chain to the
                                        ; previous interrupt handler
