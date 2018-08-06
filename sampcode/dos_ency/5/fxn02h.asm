        mov     ah,02h          ; function 02H = display character
        mov     dl,'*'          ; DL = character to display
        int     21h             ; transfer to MS-DOS
