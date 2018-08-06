        mov     ah,0eh          ; subfunction 0EH = write character
                                ; in teletype mode
        mov     al,'*'          ; AL = character to display
        mov     bh,0            ; select display page 0
        int     10h             ; transfer to ROM BIOS video driver
