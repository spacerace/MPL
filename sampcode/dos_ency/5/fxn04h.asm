msg     db      'This is a test message'
msg_len equ     $-msg
        .
        .
        .
        mov     bx,seg msg      ; set DS:BX = address of message
        mov     ds,bx
        mov     bx,offset msg
        mov     cx,msg_len      ; set CX = length of message
L1:     mov     dl,[bx]         ; get next character into DL
        mov     ah,04H          ; function 04H = auxiliary output
        int     21h             ; transfer to MS-DOS
        inc     bx              ; bump pointer to output string
        loop    L1              ; and loop until all chars. sent
