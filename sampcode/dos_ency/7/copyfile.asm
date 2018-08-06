fcb1    db      0               ; drive = default
        db      'MYFILE  '      ; 8 character filename
        db      'DAT'           ; 3 character extension
        db      25 dup (0)      ; remainder of fcb1
fcb2    db      0               ; drive = default
        db      'MYFILE  '      ; 8 character filename
        db      'BAK'           ; 3 character extension
        db      25 dup (0)      ; remainder of fcb2
buff    db      512 dup (?)     ; buffer for file I/O
        .
        .
        .
                                ; open MYFILE.DAT...
        mov     dx,seg fcb1     ; DS:DX = address of FCB
        mov     ds,dx
        mov     dx,offset fcb1
        mov     ah,0fh          ; Function 0FH = open
        int     21h             ; transfer to MS-DOS
        or      al,al           ; did open succeed?
        jnz     error           ; jump if open failed
                                ; create MYFILE.BAK...
        mov     dx,offset fcb2  ; DS:DX = address of FCB
        mov     ah,16h          ; Function 16H = create
        int     21h             ; transfer to MS-DOS
        or      al,al           ; did create succeed?
        jnz     error           ; jump if create failed
                                ; set record length to 512
        mov     word ptr fcb1+0eh,512
        mov     word ptr fcb2+0eh,512
                                ; set DTA to our buffer...
        mov     dx,offset buff  ; DS:DX = buffer address
        mov     ah,1ah          ; Function 1AH = set DTA
        int     21h             ; transfer to MS-DOS
loop:                           ; read MYFILE.DAT
        mov     dx,offset fcb1  ; DS:DX = FCB address
        mov     ah,14h          ; Function 14H = seq. read
        int     21h             ; transfer to MS-DOS
        or      al,al           ; was read successful?
        jnz     done            ; no, quit
                                ; write MYFILE.BAK...
        mov     dx,offset fcb2  ; DS:DX = FCB address
        mov     ah,15h          ; Function 15H = seq. write
        int     21h             ; transfer to MS-DOS
        or      al,al           ; was write successful?
        jnz     error           ; jump if write failed
        jmp     loop            ; continue to end of file
done:                           ; now close files...
        mov     dx,offset fcb1  ; DS:DX = FCB for MYFILE.DAT
        mov     ah,10h          ; Function 10H = close file
        int     21h             ; transfer to MS-DOS
        or      al,al           ; did close succeed?
        jnz     error           ; jump if close failed
        mov     dx,offset fcb2  ; DS:DX = FCB for MYFILE.BAK
        mov     ah,10h          ; Function 10H = close file
        int     21h             ; transfer to MS-DOS
        or      al,al           ; did close succeed?
        jnz     error           ; jump if close failed
        .
        .
        .
