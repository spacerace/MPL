kbuf    db      64,0,64 dup (0)
prompt  db      0dh,0ah,'Enter filename: $'
myfcb   db      37 dup (0)

        .
        .
        .
                                ; display the prompt...
        mov     dx,seg prompt   ; DS:DX = prompt address
        mov     ds,dx
        mov     es,dx
        mov     dx,offset prompt
        mov     ah,09H          ; Function 09H = print string
        int     21h             ; transfer to MS-DOS

                                ; now input filename...
        mov     dx,offset kbuf  ; DS:DX = buffer address
        mov     ah,0ah          ; Function 0AH = enter string
        int     21h             ; transfer to MS-DOS

                                ; parse filename into FCB...
        mov     si,offset kbuf+2 ; DS:SI = address of filename
        mov     di,offset myfcb ; ES:DI = address of fcb
        mov     ax,2900h        ; Function 29H = parse name
        int     21h             ; transfer to MS-DOS
        or      al,al           ; jump if bad drive or
        jnz     error           ; wildcard characters in name

                                ; try to open file...
        mov     dx,offset myfcb ; DS:DX = FCB address
        mov     ah,0fh          ; Function 0FH = open file
        int     21h             ; transfer to MS-DOS
        or      al,al           ; check status 
        jz      proceed         ; jump if open successful

                                ; else create file...
        mov     dx,offset myfcb ; DS:DX = FCB address
        mov     ah,16h          ; Function 16H = create
        int     21h             ; transfer to MS-DOS
        or      al,al           ; did create succeed?
        jnz     error           ; jump if create failed

proceed:
        .                       ; file has been opened or
        .                       ; created, and FCB is valid
        .                       ; for read/write operations...
