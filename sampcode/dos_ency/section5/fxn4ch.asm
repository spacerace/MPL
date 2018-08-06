        ;************************************************************;
        ;                                                            ;
        ;       Function 4CH: Terminate Process with Return Code     ;
        ;                                                            ;
        ;       int terminate(returncode)                            ;
        ;           int returncode;                                  ;
        ;                                                            ;
        ;       Does NOT return at all!                              ;
        ;                                                            ;
        ;************************************************************;

cProc   terminate,PUBLIC
parmB   returncode
cBegin
        mov     al,returncode   ; Set return code.
        mov     ah,4ch          ; Set function code.
        int     21h             ; Call MS-DOS to terminate process.
cEnd
