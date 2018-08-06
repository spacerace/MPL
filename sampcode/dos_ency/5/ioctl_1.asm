info    dw      ?               ; save device information word here
        .
        .
        .
        mov     ax,4400h        ; AH = function 44H, IOCTL
                                ; AL = subfunction 00H, get device
                                ; information word
        mov     bx,0            ; BX = handle for standard input
        int     21h             ; transfer to MS-DOS
        mov     info,dx         ; save device information word
                                ; (assumes DS = data segment)
        or      dl,20h          ; set raw mode bit
        mov     dh,0            ; and clear DH as MS-DOS requires
        mov     ax,4401h        ; AL = subfunction 01H, set device
                                ; information word
                                ; (BX still contains handle)
        int     21h             ; transfer to MS-DOS
