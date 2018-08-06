        .
        .
        .
        mov     bx,800h         ; 800H paragraphs = 32 KB
        mov     ah,48h          ; Fxn 48H = allocate block
        int     21h             ; transfer to MS-DOS
        jc      error           ; jump if allocation failed
        mov     bufseg,ax       ; save segment of block

                                ; open working file...
        mov     dx,offset file1 ; DS:DX = filename address
        mov     ax,3d00h        ; Fxn 3DH = open, read only
        int     21h             ; transfer to MS-DOS
        jc      error           ; jump if open failed
        mov     handle1,ax      ; save handle for work file

                                ; create backup file...
        mov     dx,offset file2 ; DS:DX = filename address
        mov     cx,0            ; CX = attribute (normal)
        mov     ah,3ch          ; Fxn 3CH = create file
        int     21h             ; transfer to MS-DOS
        jc      error           ; jump if create failed
        mov     handle2,ax      ; save handle for backup file

        push    ds              ; set ES = our data segment
        pop     es
        mov     ds,bufseg       ; set DS:DX = allocated block
        xor     dx,dx

        assume  ds:NOTHING,es:_DATA     ; tell assembler

next:                           ; read working file...
        mov     bx,handle1      ; handle for work file
        mov     cx,8000h        ; try to read 32 KB
        mov     ah,3fh          ; Fxn 3FH = read
        int     21h             ; transfer to MS-DOS
        jc      error           ; jump if read failed
        or      ax,ax           ; was end of file reached?
        jz      done            ; yes, exit this loop

                                ; now write backup file...
        mov     cx,ax           ; set write length = read length
        mov     bx,handle2      ; handle for backup file
        mov     ah,40h          ; Fxn 40H = write
        int     21h             ; transfer to MS-DOS
        jc      error           ; jump if write failed
        cmp     ax,cx           ; was write complete?
        jne     error           ; no, disk must be full
        jmp     next            ; transfer another record

done:   push    es              ; restore DS = data segment
        pop     ds

        assume  ds:_DATA,es:NOTHING     ; tell assembler

                                ; release allocated block...
        mov     es,bufseg       ; segment base of block
        mov     ah,49h          ; Fxn 49H = release block
        int     21h             ; transfer to MS-DOS
        jc      error           ; (should never fail)

                                ; now close backup file...
        mov     bx,handle2      ; handle for backup file
        mov     ah,3eh          ; Fxn 3EH = close
        int     21h             ; transfer to MS-DOS
        jc      error           ; jump if close failed
        .
        .
        .

file1   db      'MYFILE.DAT',0  ; name of working file
file2   db      'MYFILE.BAK',0  ; name of backup file

handle1 dw      ?               ; handle for working file
handle2 dw      ?               ; handle for backup file
bufseg  dw      ?               ; segment of allocated block
