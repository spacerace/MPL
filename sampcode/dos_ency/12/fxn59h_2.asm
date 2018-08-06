myfile  db      'MYFILE.DAT',0  ; ASCIIZ filename
        .
        .
        .
        mov     dx,seg myfile   ; DS:DX = ASCIIZ filename
        mov     ds,dx
        mov     dx,offset myfile
        mov     ax,3d02h        ; open, read/write
        int     21h             ; transfer to MS-DOS
        jnc     success         ; jump, open succeeded
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
        .
        .
        .
