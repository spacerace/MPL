        ;************************************************************;
        ;                                                            ;
        ;              Function 24H: Set Relative Record             ;
        ;                                                            ;
        ;              int FCB_set_rrec(oXFCB)                       ;
        ;                  char *oXFCB;                              ;
        ;                                                            ;
        ;              Returns 0.                                    ;
        ;                                                            ;
        ;************************************************************;

cProc   FCB_set_rrec,PUBLIC,ds
parmDP  poXFCB
cBegin
        loadDP  ds,dx,poXFCB    ; Pointer to opened extended FCB.
        mov     bx,dx           ; BX points at FCB, too.
        mov     byte ptr [bx+2bh],0 ; Zero high byte of high word of
                                    ; relative-record field.
        mov     ah,24h          ; Ask MS-DOS to set relative record
                                ; to current record.
        int     21h
        xor     ax,ax           ; Return 0.
cEnd
