        ;************************************************************;
        ;                                                            ;
        ;           Function 35H: Get Interrupt Vector               ;
        ;                                                            ;
        ;           typedef void (far *FCP)();                       ;
        ;           FCP get_vector(intnum)                           ;
        ;               int intnum;                                  ;
        ;                                                            ;
        ;           Returns a far code pointer that is the           ;
        ;           segment:offset of the interrupt vector.          ;
        ;                                                            ;
        ;************************************************************;

cProc   get_vector,PUBLIC
parmB   intnum
cBegin
        mov     al,intnum       ; Get interrupt number into AL.
        mov     ah,35h          ; Select "get vector" function.
        int     21h             ; Call MS-DOS.
        mov     ax,bx           ; Return vector offset.
        mov     dx,es           ; Return vector segment.
cEnd
