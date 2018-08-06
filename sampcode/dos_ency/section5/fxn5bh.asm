        ;************************************************************;
        ;                                                            ;
        ;          Function 5BH: Create New File                     ;
        ;                                                            ;
        ;          int create_new(ppathname,attr)                    ;
        ;              char *ppathname;                              ;
        ;              int attr;                                     ;
        ;                                                            ;
        ;          Returns -2 if file already exists,                ;
        ;                  -1 for all other errors,                  ;
        ;                  otherwise returns file handle.            ;
        ;                                                            ;
        ;************************************************************;

cProc   create_new,PUBLIC,ds
parmDP  ppathname
parmW   attr
cBegin
        loadDP  ds,dx,ppathname ; Get pointer to pathname.
        mov     cx,attr         ; Get new file's attribute.
        mov     ah,5bh          ; Set function code.
        int     21h             ; Ask MS-DOS to make a new file.
        jnb     cn_ok           ; Branch if MS-DOS returned handle.
        mov     bx,-2
        cmp     al,80           ; Did file already exist?
        jz      ae_err          ; Branch if so.
        inc     bx              ; Change -2 to -1.
ae_err:
        mov     ax,bx           ; Return error code.
cn_ok:
cEnd
