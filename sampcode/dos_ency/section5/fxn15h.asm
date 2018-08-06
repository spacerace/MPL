        ;************************************************************;
        ;                                                            ;
        ;          Function 15H: Sequential Write, FCB-based         ;
        ;                                                            ;
        ;          int FCB_swrite(oXFCB)                             ;
        ;              char *oXFCB;                                  ;
        ;                                                            ;
        ;          Returns 0 if record read OK, otherwise            ;
        ;          returns error code 1 or 2.                        ;
        ;                                                            ;
        ;************************************************************;

cProc   FCB_swrite,PUBLIC,ds
parmDP  poXFCB
cBegin
        loadDP  ds,dx,poXFCB    ; Pointer to opened extended FCB.
        mov     ah,15h          ; Ask MS-DOS to write next record
                                ; from DTA to disk file.
        int     21h
        cbw                     ; Clear high byte for return value.
cEnd
