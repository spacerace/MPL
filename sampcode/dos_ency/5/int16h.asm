        mov     ah,00h          ; subfunction 00H = read character
        int     16h             ; transfer to ROM BIOS
                                ; now AH = scan code, AL = character
