buffer  db      41 dup (0)      ; Receives country information.
        .
        .
        .
        mov     ax,6501h        ; Function = get extended info.
        mov     bx,437          ; Code page.
        mov     cx,41           ; Length of buffer.
        mov     dx,-1           ; Default country.
        mov     di,seg buffer   ; ES:DI = buffer address.
        mov     es,di
        mov     di,offset buffer
        int     21h             ; Transfer to MS-DOS.
        jc      error           ; Jump if function failed.
        .
        .
        .
