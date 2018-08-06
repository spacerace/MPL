fname   db      'C:\TEMP\'      ; generated ASCIIZ filename
        db      13 dup (0)      ; is appended by MS-DOS

fhandle dw      ?
        .
        .
        .
        mov     dx,seg fname    ; DS:DX = address of
        mov     ds,dx           ; path for temporary file
        mov     dx,offset fname
        xor     cx,cx           ; CX = normal attribute
        mov     ah,5ah          ; Function 5AH = create
                                ; temporary file
        int     21h             ; transfer to MS-DOS
        jc      error           ; jump if create failed
        mov     fhandle,ax      ; else save file handle
        .
        .
        .
