        ;************************************************************;
        ;                                                            ;
        ;            Function 43H: Get/Set File Attributes           ;
        ;                                                            ;
        ;            int file_attr(pfilepath,func,attr)              ;
        ;                char *pfilepath;                            ;
        ;                int func,attr;                              ;
        ;                                                            ;
        ;            Returns -1 for all errors,                      ;
        ;            otherwise returns file attribute.               ;
        ;                                                            ;
        ;************************************************************;

cProc   file_attr,PUBLIC,ds
parmDP  pfilepath
parmB   func
parmW   attr
cBegin
        loadDP  ds,dx,pfilepath ; Get pointer to pathname.
        mov     al,func         ; Get/set flag into AL.
        mov     cx,attr         ; Get new attr (if present).
        mov     ah,43h          ; Set code function.
        int     21h             ; Call MS-DOS.
        jnb     fa_ok           ; Branch if no error.
        mov     cx,-1           ; Else return -1.
fa_ok:
        mov     ax,cx           ; Return this value.

cEnd
