        ;************************************************************;
        ;                                                            ;
        ;       Function 26H: Create New Program Segment Prefix      ;
        ;                                                            ;
        ;       int create_psp(pspseg)                               ;
        ;           int  pspseg;                                     ;
        ;                                                            ;
        ;       Returns 0.                                           ;
        ;                                                            ;
        ;************************************************************;

cProc   create_psp,PUBLIC
parmW   pspseg
cBegin
        mov     dx,pspseg       ; Get segment address of new PSP.
        mov     ah,26h          ; Set function code.
        int     21h             ; Ask MS-DOS to create new PSP.
        xor     ax,ax           ; Return 0.
cEnd
