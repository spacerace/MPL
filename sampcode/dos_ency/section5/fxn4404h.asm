        ;************************************************************;
        ;                                                            ;
        ;   Function 44H, Subfunctions 04H,05H:                      ;
        ;                 IOCTL Block Device Control                 ;
        ;                                                            ;
        ;   int ioctl_block_ctrl(recvflag,drive_ltr,pbuffer,nbytes)  ;
        ;       int   recvflag;                                      ;
        ;       int   drive_ltr;                                     ;
        ;       char *pbuffer;                                       ;
        ;       int   nbytes;                                        ;
        ;                                                            ;
        ;   Set recvflag = 0 to receive info, 1 to send.             ;
        ;                                                            ;
        ;   Returns -1 for error, otherwise returns number of        ;
        ;   bytes sent or received.                                  ;
        ;                                                            ;
        ;************************************************************;

cProc   ioctl_block_ctrl,PUBLIC,<ds>
parmB   recvflag
parmB   drive_ltr
parmDP  pbuffer
parmW   nbytes
cBegin
        mov     al,recvflag     ; Get recvflag.
        and     al,1            ; Keep only lsb.
        add     al,4            ; AL = 04H for receive, 05H for send.
        mov     bl,drive_ltr    ; Get drive letter.
        or      bl,bl           ; Leave 0 alone.
        jz      ibc
        and     bl,not 20h      ; Convert letter to uppercase.
        sub     bl,'A'-1        ; Convert to drive number: 'A' = 1,
                                ; 'B' = 2, etc.
ibc:
        mov     cx,nbytes       ; Get number of bytes to recv/send.
        loadDP  ds,dx,pbuffer   ; Get pointer to buffer.
        mov     ah,44h          ; Set function code.
        int     21h             ; Call MS-DOS.
        jnc     ibcx            ; Branch if no error.
        mov     ax,-1           ; Return -1 for all errors.
ibcx:
cEnd
