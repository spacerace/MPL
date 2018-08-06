file1   db      'MYFILE.DAT',0
file2   db      'MYFILE.BAK',0

handle1 dw      ?               ; handle for MYFILE.DAT
handle2 dw      ?               ; handle for MYFILE.BAK

buff    db      512 dup (?)     ; buffer for file I/O
        .
        .
        .
                                ; open MYFILE.DAT...
        mov     dx,seg file1    ; DS:DX = address of filename
        mov     ds,dx
        mov     dx,offset file1
        mov     ax,3d00h        ; Function 3DH = open (read-only)
        int     21h             ; transfer to MS-DOS
        jc      error           ; jump if open failed
        mov     handle1,ax      ; save handle for file

                                ; create MYFILE.BAK...
        mov     dx,offset file2 ; DS:DX = address of filename
        mov     cx,0            ; CX = normal attribute 
        mov     ah,3ch          ; Function 3CH = create
        int     21h             ; transfer to MS-DOS
        jc      error           ; jump if create failed
        mov     handle2,ax      ; save handle for file

loop:                           ; read MYFILE.DAT
        mov     dx,offset buff  ; DS:DX = buffer address
        mov     cx,512          ; CX = length to read
        mov     bx,handle1      ; BX = handle for MYFILE.DAT
        mov     ah,3fh          ; Function 3FH = read
        int     21h             ; transfer to MS-DOS
        jc      error           ; jump if read failed
        or      ax,ax           ; were any bytes read?
        jz      done            ; no, end of file reached

                                ; write MYFILE.BAK
        mov     dx,offset buff  ; DS:DX = buffer address
        mov     cx,ax           ; CX = length to write
        mov     bx,handle2      ; BX = handle for MYFILE.BAK
        mov     ah,40h          ; Function 40H = write
        int     21h             ; transfer to MS-DOS
        jc      error           ; jump if write failed
        cmp     ax,cx           ; was write complete?
        jne     error           ; jump if disk full
        jmp     loop            ; continue to end of file

done:                           ; now close files...
        mov     bx,handle1      ; handle for MYFILE.DAT
        mov     ah,3eh          ; Function 3EH = close file
        int     21h             ; transfer to MS-DOS
        jc      error           ; jump if close failed

        mov     bx,handle2      ; handle for MYFILE.BAK
        mov     ah,3eh          ; Function 3EH = close file
        int     21h             ; transfer to MS-DOS
        jc      error           ; jump if close failed

        .
        .
        .
