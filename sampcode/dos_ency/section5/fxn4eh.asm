        ;************************************************************;
        ;                                                            ;
        ;               Function 4EH: Find First File                ;
        ;                                                            ;
        ;               int find_first(ppathname,attr)               ;
        ;                   char *ppathname;                         ;
        ;                   int  attr;                               ;
        ;                                                            ;
        ;               Returns 0 if a match was found,              ;
        ;               otherwise returns error code.                ;
        ;                                                            ;
        ;************************************************************;

cProc   find_first,PUBLIC,ds
parmDP  ppathname
parmW   attr
cBegin
        loadDP  ds,dx,ppathname ; Get pointer to pathname.
        mov     cx,attr         ; Get search attributes.
        mov     ah,4eh          ; Set function code.
        int     21h             ; Ask MS-DOS to look for a match.
        jb      ff_err          ; Branch on error.
        xor     ax,ax           ; Return 0 if no error.
ff_err:
cEnd
