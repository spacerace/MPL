msg     db      'This is a test message'
msg_len equ     $-msg
        .
        .
        .
        mov     dx,seg msg      ; DS:DX = address of text
        mov     ds,dx           ; to display
        mov     dx,offset msg
        mov     cx,msg_len      ; CX = length of text
        mov     bx,1            ; BX = handle for standard output
        mov     ah,40h          ; function 40H = write file/device
        int     21h             ; transfer to MS-DOS
