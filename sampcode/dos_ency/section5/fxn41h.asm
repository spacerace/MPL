        ;************************************************************;
        ;                                                            ;
        ;                Function 41H: Delete File                   ;
        ;                                                            ;
        ;                int delete(pfilepath)                       ;
        ;                    char *pfilepath;                        ;
        ;                                                            ;
        ;                Returns 0 if file deleted,                  ;
        ;                otherwise returns error code.               ;
        ;                                                            ;
        ;************************************************************;

cProc   delete,PUBLIC,ds
parmDP  pfilepath
cBegin
        loadDP  ds,dx,pfilepath ; Get pointer to pathname.
        mov     ah,41h          ; Set function code.
        int     21h             ; Ask MS-DOS to delete file.
        jb       dl_err         ; Branch if MS-DOS could not delete
                                ; file.
        xor     ax,ax           ; Else return 0.
dl_err:
cEnd
