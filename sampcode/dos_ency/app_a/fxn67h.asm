        .
        .
        .
        mov     ah,67h          ; Function 67H = set handle count.
        mov     bx,30           ; Maximum number of handles.
        int     21h             ; Transfer to MS-DOS.
        jc      error           ; Jump if function failed.
        .
        .
        .
