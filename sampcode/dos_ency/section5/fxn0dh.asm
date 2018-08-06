        ;************************************************************;
        ;                                                            ;
        ;                   Function 0DH: Disk Reset                 ;
        ;                                                            ;
        ;                   int reset_disk()                         ;
        ;                                                            ;
        ;                   Returns 0.                               ;
        ;                                                            ;
        ;************************************************************;

cProc   reset_disk,PUBLIC
cBegin
        mov     ah,0dh          ; Set function code.
        int     21h             ; Ask MS-DOS to write all dirty file
                                ; buffers to the disk.
        xor     ax,ax           ; Return 0.
cEnd
