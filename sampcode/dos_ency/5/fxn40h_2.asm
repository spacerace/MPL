msg     db      'This is a test message'
msg_len equ     $-msg
        .
        .
        .
        mov     dx,seg msg      ; DS:DX = address of message
        mov     ds,dx
        mov     dx,offset msg
        mov     cx,msg_len      ; CX = length of message
        mov     bx,3            ; BX = handle for standard aux.
        mov     ah,40h          ; function 40H = write file/device
        int     21h             ; transfer to MS-DOS
        jc      error           ; jump if write operation failed
