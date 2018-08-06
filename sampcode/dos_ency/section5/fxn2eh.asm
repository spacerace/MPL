        ;************************************************************;
        ;                                                            ;
        ;             Function 2EH: Set/Reset Verify Flag            ;
        ;                                                            ;
        ;             int set_verify(newvflag)                       ;
        ;                 char newvflag;                             ;
        ;                                                            ;
        ;             Returns 0.                                     ;
        ;                                                            ;
        ;************************************************************;

cProc   set_verify,PUBLIC
parmB   newvflag
cBegin
        mov     al,newvflag     ; Get new value of verify flag.
        mov     ah,2eh          ; Set function code.
        int     21h             ; Ask MS-DOS to store flag.
        xor     ax,ax           ; Return 0.
cEnd
