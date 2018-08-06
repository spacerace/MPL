        ;************************************************************;
        ;                                                            ;
        ;           Function 10H: Close file, FCB-based              ;
        ;                                                            ;
        ;           int FCB_close(oXFCB)                             ;
        ;               char *oXFCB;                                 ;
        ;                                                            ;
        ;           Returns 0 if file closed OK, otherwise           ;
        ;           returns -1.                                      ;
        ;                                                            ;
        ;************************************************************;

cProc   FCB_close,PUBLIC,ds
parmDP  poXFCB
cBegin
        loadDP  ds,dx,poXFCB    ; Pointer to opened extended FCB.
        mov     ah,10h          ; Ask MS-DOS to close file.
        int     21h
        cbw                     ; Set return value to 0 or -1.
cEnd
