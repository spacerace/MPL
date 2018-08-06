myfcb   db      0               ; drive = default
        db      'MYFILE  '      ; filename, 8 chars
        db      'DAT'           ; extension, 3 chars
        db      25 dup (0)      ; remainder of FCB
        .
        .
        .
        mov     dx,seg myfcb    ; DS:DX = FCB
        mov     ds,dx
        mov     dx,offset myfcb
        mov     ah,0fh          ; function 0FH = Open FCB

        int     21h             ; transfer to MS-DOS
        or      al,al           ; test status
        jz      success         ; jump, open succeeded
                                ; open failed, get
                                ; extended error info
        mov     bx,0            ; BX = 00H for ver. 2.x-3.x
        mov     ah,59h          ; function 59H = Get Info
        int     21h             ; transfer to MS-DOS
        or      ax,ax           ; really an error?
        jz      success         ; no error, jump
                                ; test recommended actions
        cmp     bl,01h
        jz      retry           ; if BL = 01H retry operation
        cmp     bl,04h
        jz      cleanup         ; if BL = 04H clean up and exit
        cmp     bl,05h
        jz      panic           ; if BL = 05H exit immediately
        .
        .
        .
