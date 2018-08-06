        ;************************************************************;
        ;                                                            ;
        ;       Function 62H: Get Program Segment Prefix Address     ;
        ;                                                            ;
        ;       int get_psp()                                        ;
        ;                                                            ;
        ;       Returns PSP segment.                                 ;
        ;                                                            ;
        ;************************************************************;

cProc   get_psp,PUBLIC
cBegin
        mov     ah,62h          ; Set function code.
        int     21h             ; Get PSP address.
        mov     ax,bx           ; Return it in AX.
cEnd
