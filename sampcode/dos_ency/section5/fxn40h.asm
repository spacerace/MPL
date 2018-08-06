        ;************************************************************;
        ;                                                            ;
        ;          Function 40H: Write File or Device                ;
        ;                                                            ;
        ;          int write(handle,pbuffer,nbytes)                  ;
        ;              int handle,nbytes;                            ;
        ;              char *pbuffer;                                ;
        ;                                                            ;
        ;          Returns -1 if there was a write error,            ;
        ;          otherwise returns number of bytes written.        ;
        ;                                                            ;
        ;************************************************************;

cProc   write,PUBLIC,ds
parmW   handle
parmDP  pbuffer
parmW   nbytes
cBegin
        mov     bx,handle       ; Get handle.
        loadDP  ds,dx,pbuffer   ; Get pointer to buffer.
        mov     cx,nbytes       ; Get number of bytes to write.
        mov     ah,40h          ; Set function code.
        int     21h             ; Ask MS-DOS to write CX bytes.
        jnb     wr_ok           ; Branch if write successful.
        mov     ax,-1           ; Else return -1.
wr_ok:
cEnd
