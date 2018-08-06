msg     db      'This is a test message'
msg_len equ     $-msg
        .
        .
        .
        mov     bx,seg msg      ; DS:BX = address of message
        mov     ds,bx
        mov     bx,offset msg
        mov     cx,msg_len      ; CX = length of message
        mov     dx,0            ; DX = 0 for LPT1
L1:     mov     al,[bx]         ; get next character into AL
        mov     ah,00H          ; subfunction 00H = output
        int     17h             ; transfer to ROM BIOS
        inc     bx              ; bump pointer to output string
        loop    L1              ; and loop until all chars. sent
