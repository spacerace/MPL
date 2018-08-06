fhandle dw      ?               ; handle from previous open

        .
        .
        .
                                ; duplicate the handle...
        mov     bx,fhandle      ; BX = handle for file
        mov     ah,45h          ; Function 45H = dup handle
        int     21h             ; transfer to MS-DOS
        jc      error           ; jump if function call failed

                                ; now close the new handle...
        mov     bx,ax           ; BX = duplicated handle
        mov     ah,3eh          ; Function 3EH = close 
        int     21h             ; transfer to MS-DOS
        jc      error           ; jump if close failed
        mov     bx,fhandle      ; replace closed handle with active handle
        .
        .
        .
