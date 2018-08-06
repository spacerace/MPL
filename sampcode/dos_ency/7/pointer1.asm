fhandle dw      ?               ; handle from previous open
buff    db      512 dup (?)     ; buffer for data from file

        .
        .
        .
                                ; position the file pointer...
        mov     cx,0            ; CX = high part of file offset
        mov     dx,32768        ; DX = low part of file offset
        mov     bx,fhandle      ; BX = handle for file
        mov     al,0            ; AL = positioning mode
        mov     ah,42h          ; Function 42H = position
        int     21h             ; transfer to MS-DOS
        jc      error           ; jump if function call failed

                                ; now read 512 bytes from file
        mov     dx,offset buff  ; DS:DX = address of buffer
        mov     cx,512          ; CX = length of 512 bytes
        mov     bx,fhandle      ; BX = handle for file
        mov     ah,3fh          ; Function 3FH = read
        int     21h             ; transfer to MS-DOS
        jc      error           ; jump if read failed
        cmp     ax,512          ; was 512 bytes read?
        jne     error           ; jump if partial rec. or EOF
        .
        .
        .
