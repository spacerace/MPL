file1   db      '\WORK\MYFILE.DAT',0
file2   db      '\BACKUP\MYFILE.OLD',0
        .
        .
        .
        mov     dx,seg file1    ; DS:DX = old filename
        mov     ds,dx
        mov     es,dx
        mov     dx,offset file1
        mov     di,offset file2 ; ES:DI = new filename
        mov     ah,56h          ; Function 56H = rename
        int     21h             ; transfer to MS-DOS
        jc      error           ; jump if rename failed
        .
        .
        .
