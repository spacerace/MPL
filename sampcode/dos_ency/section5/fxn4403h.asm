        ;************************************************************;
        ;                                                            ;
        ;     Function 44H, Subfunctions 02H,03H:                    ;
        ;                   IOCTL Character Device Control           ;
        ;                                                            ;
        ;     int ioctl_char_ctrl(recvflag,handle,pbuffer,nbytes)    ;
        ;         int   recvflag;                                    ;
        ;         int   handle;                                      ;
        ;         char *pbuffer;                                     ;
        ;         int   nbytes;                                      ;
        ;                                                            ;
        ;     Set recvflag = 0 to get flags, 1 to set flags.         ;
        ;                                                            ;
        ;     Returns -1 for error, otherwise returns number of      ;
        ;     bytes sent or received.                                ;
        ;                                                            ;
        ;************************************************************;

cProc   ioctl_char_ctrl,PUBLIC,<ds>
parmB   recvflag
parmW   handle
parmDP  pbuffer
parmW   nbytes
cBegin
        mov     al,recvflag     ; Get recvflag.
        and     al,1            ; Keep only lsb.
        add     al,3            ; AL = 02H for receive, 03H for send.
        mov     bx,handle       ; Get character-device handle.
        mov     cx,nbytes       ; Get number of bytes to receive/send.
        loadDP  ds,dx,pbuffer   ; Get pointer to buffer.
        mov     ah,44h          ; Set function code.
        int     21h             ; Call MS-DOS.
        jnc     iccx            ; Branch if no error.
        mov     ax,-1           ; Return -1 for all errors.
iccx:
cEnd
