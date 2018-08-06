        ;************************************************************;
        ;                                                            ;
        ;              Function 25H: Set Interrupt Vector            ;
        ;                                                            ;
        ;              typedef void (far *FCP)();                    ;
        ;              int set_vector(intnum,vector)                 ;
        ;                  int intnum;                               ;
        ;                  FCP vector;                               ;
        ;                                                            ;
        ;              Returns 0.                                    ;
        ;                                                            ;
        ;************************************************************;

cProc   set_vector,PUBLIC,ds
parmB   intnum
parmD   vector
cBegin
        lds     dx,vector       ; Get vector segment:offset into
                                ; DS:DX.
        mov     al,intnum       ; Get interrupt number into AL.
        mov     ah,25h          ; Select "set vector" function.
        int     21h             ; Ask MS-DOS to change vector.
        xor     ax,ax           ; Return 0.
cEnd
