buffer  db      5 dup (0)       ; Receives pointer information.
        .
        .
        .
        mov     ax,6502h        ; Function = get pointer to
                                ; uppercase table.
        mov     bx,437          ; Code page.
        mov     cx,5            ; Length of buffer.
        mov     dx,-1           ; Default country.
        mov     di,seg buffer   ; ES:DI = buffer address.
        mov     es,di
        mov     di,offset buffer
        int     21h             ; Transfer to MS-DOS.
        jc      error           ; Jump if function failed.
        .
        .
        .
