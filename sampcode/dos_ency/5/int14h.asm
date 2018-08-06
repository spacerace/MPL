msg     db      'This is a test message'
msg_len equ     $-msg
        .
        .
        .
        mov     bx,seg msg      ; DS:BX = address of message
        mov     ds,bx
        mov     bx,offset msg
        mov     cx,msg_len      ; CX = length of message
        mov     dx,0            ; DX = 0 for COM1
L1:     mov     al,[bx]         ; get next character into AL
        mov     ah,01h          ; subfunction 01H = output
        int     14h             ; transfer to ROM BIOS
        inc     bx              ; bump pointer to output string
        loop    L1              ; and loop until all chars. sent
