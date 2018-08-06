fname   db      'C:\LETTERS\MEMO.TXT',0
fhandle dw      ?
        .
        .
        .
        mov     dx,seg fname    ; DS:DX = address of
        mov     ds,dx           ; pathname for file
        mov     dx,offset fname
        xor     cx,cx           ; CX = normal attribute
        mov     ah,3ch          ; Function 3CH = create
        int     21h             ; transfer to MS-DOS
        jc      error           ; jump if create failed
        mov     fhandle,ax      ; else save file handle
        .
        .
        .
