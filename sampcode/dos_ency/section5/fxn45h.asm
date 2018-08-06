        ;************************************************************;
        ;                                                            ;
        ;             Function 45H: Duplicate File Handle            ;
        ;                                                            ;
        ;             int dup_handle(handle)                         ;
        ;                 int handle;                                ;
        ;                                                            ;
        ;             Returns -1 for errors,                         ;
        ;             otherwise returns new handle.                  ;
        ;                                                            ;
        ;************************************************************;

cProc   dup_handle,PUBLIC
parmW   handle
cBegin
        mov     bx,handle       ; Get handle to copy.
        mov     ah,45h          ; Set function code.
        int     21h             ; Ask MS-DOS to duplicate handle.
        jnb     dup_ok          ; Branch if copy was successful.
        mov     ax,-1           ; Else return -1.
dup_ok:
cEnd
