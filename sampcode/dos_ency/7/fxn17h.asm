renfcb  db      0               ; default drive
        db      '????????'      ; wildcard filename
        db      'ASM'           ; old extension
        db      5 dup (0)       ; reserved area
        db      '????????'      ; wildcard filename
        db      'COD'           ; new extension
        db      15 dup (0)      ; remainder of FCB

        .
        .
        .
        mov     dx,seg renfcb   ; DS:DX = address of
        mov     ds,dx           ; "special" FCB
        mov     dx,offset renfcb
        mov     ah,17h          ; Function 17H = rename
        int     21h             ; transfer to MS-DOS
        or      al,al           ; did function succeed?
        jnz     error           ; jump if rename failed
        .
        .
        .
