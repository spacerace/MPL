msg     db      'This is a test message','$'
        .
        .
        .
        mov     dx,seg msg      ; DS:DX = address of text
        mov     ds,dx           ; to display
        mov     dx,offset msg
        mov     ah,09H          ; function 09H = display string
        int     21h             ; transfer to MS-DOS
