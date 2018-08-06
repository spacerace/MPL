delfcb  db      0               ; default drive
        db      'A???????'      ; wildcard filename
        db      'BAK'           ; extension
        db      25 dup (0)      ; remainder of FCB

        .
        .
        .
        mov     dx,seg delfcb   ; DS:DX = FCB address
        mov     ds,dx
        mov     dx,offset delfcb
        mov     ah,13h          ; Function 13H = delete
        int     21h             ; transfer to MS-DOS
        or      al,al           ; did function succeed?
        jnz     error           ; jump if delete failed
        .
        .
        .
