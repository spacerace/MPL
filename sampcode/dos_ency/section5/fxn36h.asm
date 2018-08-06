        ;************************************************************;
        ;                                                            ;
        ;             Function 36H: Get Disk Free Space              ;
        ;                                                            ;
        ;             long free_space(drive_ltr)                     ;
        ;                 char drive_ltr;                            ;
        ;                                                            ;
        ;             Returns the number of bytes free as            ;
        ;             a long integer.                                ;
        ;                                                            ;
        ;************************************************************;

cProc   free_space,PUBLIC
parmB   drive_ltr
cBegin
        mov     dl,drive_ltr    ; Get drive letter.
        or      dl,dl           ; Leave 0 alone.
        jz      fsp
        and     dl,not 20h      ; Convert letter to uppercase.
        sub     dl,'A'-1        ; Convert to drive number: 'A' = 1,
                                ; 'B' = 2, etc.
fsp:
        mov     ah,36h          ; Set function code.
        int     21h             ; Ask MS-DOS to get disk information.
        mul     cx              ; Bytes/sector * sectors/cluster
        mul     bx              ; * free clusters.
cEnd
