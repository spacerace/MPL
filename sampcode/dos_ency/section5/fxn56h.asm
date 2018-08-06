        ;************************************************************;
        ;                                                            ;
        ;                Function 56H: Rename File                   ;
        ;                                                            ;
        ;                int rename(poldpath,pnewpath)               ;
        ;                    char *poldpath,*pnewpath;               ;
        ;                                                            ;
        ;                Returns 0 if file moved OK,                 ;
        ;                otherwise returns error code.               ;
        ;                                                            ;
        ;************************************************************;

cProc   rename,PUBLIC,<ds,di>
parmDP  poldpath
parmDP  pnewpath
cBegin
        loadDP  es,di,pnewpath  ; ES:DI = pointer to newpath.
        loadDP  ds,dx,poldpath  ; DS:DX = pointer to oldpath.
        mov     ah,56h          ; Set function code.
        int     21h             ; Ask MS-DOS to rename file.
        jb      rn_err          ; Branch on error.
        xor     ax,ax           ; Return 0 if no error.
rn_err:
cEnd
