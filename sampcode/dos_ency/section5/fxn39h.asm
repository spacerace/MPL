        ;************************************************************;
        ;                                                            ;
        ;              Function 39H: Create Directory                ;
        ;                                                            ;
        ;              int make_dir(pdirpath)                        ;
        ;                  char *pdirpath;                           ;
        ;                                                            ;
        ;              Returns 0 if directory created OK,            ;
        ;              otherwise returns error code.                 ;
        ;                                                            ;
        ;************************************************************;

cProc   make_dir,PUBLIC,ds
parmDP  pdirpath
cBegin
        loadDP  ds,dx,pdirpath  ; Get pointer to pathname.
        mov     ah,39h          ; Set function code.
        int     21h             ; Ask MS-DOS to make new subdirectory.
        jb      md_err          ; Branch on error.
        xor     ax,ax           ; Else return 0.
md_err:
cEnd
