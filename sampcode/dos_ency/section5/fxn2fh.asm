        ;************************************************************;
        ;                                                            ;
        ;           Function 2FH: Get DTA Address                    ;
        ;                                                            ;
        ;           char far *get_DTA()                              ;
        ;                                                            ;
        ;           Returns a far pointer to the DTA buffer.         ;
        ;                                                            ;
        ;************************************************************;

cProc   get_DTA,PUBLIC
cBegin
        mov     ah,2fh          ; Set function code.
        int     21h             ; Ask MS-DOS for current DTA address.
        mov     ax,bx           ; Return offset in AX.
        mov     dx,es           ; Return segment in DX.
cEnd
