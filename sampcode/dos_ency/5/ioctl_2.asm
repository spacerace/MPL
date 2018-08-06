        mov     ax,4406H        ; AH = function 44H, IOCTL
                                ; AL = subfunction 06H, get
                                ; input status
        mov     bx,3            ; BX = handle for standard aux
        int     21h             ; transfer to MS-DOS
        or      al,al           ; test status of AUX driver
        jnz     ready           ; jump if input character ready
                                ; else no character is waiting
