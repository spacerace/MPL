        ;************************************************************;
        ;                                                            ;
        ;          Function 22H: Random File Write, FCB-based        ;
        ;                                                            ;
        ;          int FCB_rwrite(oXFCB,recnum)                      ;
        ;              char *oXFCB;                                  ;
        ;              long recnum;                                  ;
        ;                                                            ;
        ;          Returns 0 if record read OK, otherwise            ;
        ;          returns error code 1 or 2.                        ;
        ;                                                            ;
        ;************************************************************;

cProc   FCB_rwrite,PUBLIC,ds
parmDP  poXFCB
parmD   recnum
cBegin
        loadDP  ds,dx,poXFCB    ; Pointer to opened extended FCB.
        mov     bx,dx           ; BX points at FCB, too.
        mov     ax,word ptr (recnum)    ; Get low 16 bits of record
        mov     [bx+28h],ax             ; number and store in FCB.
        mov     ax,word ptr (recnum+2)  ; Get high 16 bits of record
        mov     [bx+2ah],ax             ; number and store in FCB.
        mov     ah,22h          ; Ask MS-DOS to write DTA to
        int     21h             ; recnum'th record of file.
        cbw                     ; Clear high byte for return value.
cEnd
