        ;************************************************************;
        ;                                                            ;
        ;          Function 46H: Force Duplicate File Handle         ;
        ;                                                            ;
        ;          int dup_handle2(existhandle,newhandle)            ;
        ;              int existhandle,newhandle;                    ;
        ;                                                            ;
        ;          Returns -1 for errors,                            ;
        ;          otherwise returns newhandle unchanged.            ;
        ;                                                            ;
        ;************************************************************;

cProc   dup_handle2,PUBLIC
parmW   existhandle
parmW   newhandle
cBegin
        mov     bx,existhandle  ; Get handle of existing file.
        mov     cx,newhandle    ; Get handle to copy into.
        mov     ah,46h          ; Close handle CX and then
        int     21h             ; duplicate BX's handle into CX.
        mov     ax,newhandle    ; Prepare return value.
        jnb     dup2_ok         ; Branch if close/copy was successful.
        mov     ax,-1           ; Else return -1.
dup2_ok:
cEnd
