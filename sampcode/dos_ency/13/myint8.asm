myint8:                                 ; this is the new handler
                                        ; for Interrupt 08H

        mov     ax,1                    ; test and set interrupt-
        xchg    cs:myflag,ax            ; handling-in-progress semaphore

        push    ax                      ; save the semaphore

        pushf                           ; simulate interrupt, allowing 
        call    dword ptr cs:oldint8    ; the previous handler for the
                                        ; Interrupt 08H vector to run

        pop     ax                      ; get the semaphore back
        or      ax,ax                   ; is our interrupt handler
                                        ; already running?

        jnz     myint8x                 ; yes, skip this one

        .                               ; now perform our interrupt
        .                               ; processing here...
        .

        mov     cs:myflag,0             ; clear the interrupt-handling-
                                        ; in-progress flag

myint8x:
        iret                            ; return from interrupt
