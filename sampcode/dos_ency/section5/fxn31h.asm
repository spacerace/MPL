        ;************************************************************;
        ;                                                            ;
        ;          Function 31H: Terminate and Stay Resident         ;
        ;                                                            ;
        ;          void keep_process(exit_code,nparas)               ;
        ;               int exit_code,nparas;                        ;
        ;                                                            ;
        ;          Does NOT return!                                  ;
        ;                                                            ;
        ;************************************************************;

cProc   keep_process,PUBLIC
parmB   exit_code
parmW   nparas
cBegin
        mov     al,exit_code    ; Get return code.
        mov     dx,nparas       ; Set DX to number of paragraphs the
                                ; program wants to keep.
        mov     ah,31h          ; Set function code.
        int     21h             ; Ask MS-DOS to keep process.
cEnd
