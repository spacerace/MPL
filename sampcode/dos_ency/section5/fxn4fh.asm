        ;************************************************************;
        ;                                                            ;
        ;               Function 4FH: Find Next File                 ;
        ;                                                            ;
        ;               int find_next()                              ;
        ;                                                            ;
        ;               Returns 0 if a match was found,              ;
        ;               otherwise returns error code.                ;
        ;                                                            ;
        ;************************************************************;

cProc   find_next,PUBLIC
cBegin
        mov     ah,4fh          ; Set function code.
        int     21h             ; Ask MS-DOS to look for the next
                                ; matching file.
        jb      fn_err          ; Branch on error.
        xor     ax,ax           ; Return 0 if no error.
fn_err:
cEnd
