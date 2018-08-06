        ;************************************************************;
        ;                                                            ;
        ;   Function 44H, Subfunctions 06H,07H:                      ;
        ;                 IOCTL Input/Output Status                  ;
        ;                                                            ;
        ;   int ioctl_char_status(outputflag,handle)                 ;
        ;       int outputflag;                                      ;
        ;       int handle;                                          ;
        ;                                                            ;
        ;   Set outputflag = 0 for input status, 1 for output status.;
        ;                                                            ;
        ;   Returns -1 for all errors, 0 for not ready,              ;
        ;   and 1 for ready.                                         ;
        ;                                                            ;
        ;************************************************************;

cProc   ioctl_char_status,PUBLIC
parmB   outputflag
parmW   handle
cBegin
        mov     al,outputflag   ; Get outputflag.
        and     al,1            ; Keep only lsb.
        add     al,6            ; AL = 06H for input status, 07H for output
                                ; status.
        mov     bx,handle       ; Get handle.
        mov     ah,44h          ; Set function code.
        int     21h             ; Call MS-DOS.
        jnc     isnoerr         ; Branch if no error.
        mov     ax,-1           ; Return error code.
        jmp     short isx
isnoerr:
        and     ax,1            ; Keep only lsb for return value.
isx:
cEnd
