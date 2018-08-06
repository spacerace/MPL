        ;************************************************************;
        ;                                                            ;
        ;          Function 0CH: Flush Buffer, Read Keyboard         ;
        ;                                                            ;
        ;          int flush_kbd()                                   ;
        ;                                                            ;
        ;          Returns 0.                                        ;
        ;                                                            ;
        ;************************************************************;

cProc   flush_kbd,PUBLIC
cBegin
        mov     ax,0c00h        ; Just flush type-ahead buffer.
        int     21h             ; Call MS-DOS.
        xor     ax,ax           ; Return 0.
cEnd
