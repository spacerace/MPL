fname   db      'C:\WORK\MYFILE.DAT',0
        .
        .
        .
        mov     dx,seg fname    ; DS:DX = address of filename
        mov     ds,dx
        mov     dx,offset fname
        mov     ah,41h          ; Function 41H = delete
        int     21h             ; transfer to MS-DOS
        jc      error           ; jump if delete failed
        .
        .
        .
