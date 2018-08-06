        ;************************************************************;
        ;                                                            ;
        ;            Function 54H: Get Verify Flag                   ;
        ;                                                            ;
        ;            int get_verify()                                ;
        ;                                                            ;
        ;            Returns current value of verify flag.           ;
        ;                                                            ;
        ;************************************************************;

cProc   get_verify,PUBLIC
cBegin
        mov     ah,54h          ; Set function code.
        int     21h             ; Read flag from MS-DOS.
        cbw                     ; Clear high byte of return value.

cEnd
