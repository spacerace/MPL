fname   db      'C:\BACKUP\MYFILE.DAT',0
        .
        .
        .
        mov     dx,seg fname    ; DS:DX = address of filename
        mov     ds,dx
        mov     dx,offset fname
        mov     cx,1            ; CX = attribute (read-only)
        mov     al,1            ; AL = mode (0 = get, 1 = set)
        mov     ah,43h          ; Function 43H = get/set attr
        int     21h             ; transfer to MS-DOS
        jc      error           ; jump if set attrib. failed
        .
        .
        .
