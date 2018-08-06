        ;************************************************************;
        ;                                                            ;
        ;             Function 5AH: Create Temporary File            ;
        ;                                                            ;
        ;             int create_temp(ppathname,attr)                ;
        ;                 char *ppathname;                           ;
        ;                 int attr;                                  ;
        ;                                                            ;
        ;             Returns -1 if file was not created,            ;
        ;             otherwise returns file handle.                 ;
        ;                                                            ;
        ;************************************************************;

cProc   create_temp,PUBLIC,ds
parmDP  ppathname
parmW   attr
cBegin
        loadDP  ds,dx,ppathname ; Get pointer to pathname.
        mov     cx,attr         ; Set function code.
        mov     ah,5ah          ; Ask MS-DOS to make a new file with
                                ; a unique name.
        int     21h             ; Ask MS-DOS to make a tmp file.
        jnb     ct_ok           ; Branch if MS-DOS returned handle.
        mov     ax,-1           ; Else return -1.
ct_ok:
cEnd
