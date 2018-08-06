fhandle dw      ?               ; handle from previous open

        .
        .
        .
                                ; position the file pointer
                                ; to the end of file...
        mov     cx,0            ; CX = high part of offset
        mov     dx,0            ; DX = low part of offset
        mov     bx,fhandle      ; BX = handle for file
        mov     al,2            ; AL = positioning mode
        mov     ah,42h          ; Function 42H = position
        int     21h             ; transfer to MS-DOS
        jc      error           ; jump if function call failed

                                ; if call succeeded, DX:AX
                                ; now contains the file size
        .
        .
        .
