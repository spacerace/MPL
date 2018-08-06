fname   db      'MYFILE.DAT',0  ; ASCIIZ filename.
fhandle dw      ?               ; Handle from Open operation.
        .
        .
        .
        mov     ah,68h          ; Function 68H = commit file.
        mov     bx,fhandle      ; Handle from previous open.
        int     21h             ; Transfer to MS-DOS.
        jc      error           ; Jump if function failed.
        .
        .
        .
