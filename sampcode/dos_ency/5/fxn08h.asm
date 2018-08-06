        mov     ah,08h          ; function 08H = character input
                                ;   without echo
        int     21h             ; transfer to MS-DOS
                                ; now AL = character
