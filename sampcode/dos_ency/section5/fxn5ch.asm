        ;************************************************************;
        ;                                                            ;
        ;            Function 5CH: Lock/Unlock File Region           ;
        ;                                                            ;
        ;            int locks(handle,onoff,start,length)            ;
        ;                int handle,onoff;                           ;
        ;                long start,length;                          ;
        ;                                                            ;
        ;            Returns 0 if operation was successful,          ;
        ;            otherwise returns error code.                   ;
        ;                                                            ;
        ;************************************************************;

cProc   locks,PUBLIC,<si,di>
parmW   handle
parmB   onoff
parmD   start
parmD   length
cBegin
        mov     al,onoff        ; Get lock/unlock flag.
        mov     bx,handle       ; Get file handle.
        les     dx,start        ; Get low word of start.
        mov     cx,es           ; Get high word of start.
        les     di,length       ; Get low word of length.
        mov     si,es           ; Get high word of length.
        mov     ah,5ch          ; Set function code.
        int     21h             ; Make lock/unlock request.
        jb      lk_err          ; Branch on error.
        xor     ax,ax           ; Return 0 if no error.
lk_err:
cEnd
