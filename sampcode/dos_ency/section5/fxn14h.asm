        ;************************************************************;
        ;                                                            ;
        ;           Function 14H: Sequential Read, FCB-based         ;
        ;                                                            ;
        ;           int FCB_sread(oXFCB)                             ;
        ;               char *oXFCB;                                 ;
        ;                                                            ;
        ;           Returns 0 if record read OK, otherwise           ;
        ;           returns error code 1, 2, or 3.                   ;
        ;                                                            ;
        ;************************************************************;

cProc   FCB_sread,PUBLIC,ds
parmDP  poXFCB
cBegin
        loadDP  ds,dx,poXFCB    ; Pointer to opened extended FCB.
        mov     ah,14h          ; Ask MS-DOS to read next record,
                                ; placing it at DTA.
        int     21h
        cbw                     ; Clear high byte for return value.
cEnd
