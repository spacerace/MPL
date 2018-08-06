        ;************************************************************;
        ;                                                            ;
        ;    Function 44H, Subfunction 0AH:                          ;
        ;                  IOCTL Remote Handle Query                 ;
        ;                                                            ;
        ;    int ioctl_char_redir(handle)                            ;
        ;        int handle;                                         ;
        ;                                                            ;
        ;    Returns -1 for all errors, 1 if device/file is remote   ;
        ;    (redirected), 0 if it is local.                         ;
        ;                                                            ;
        ;************************************************************;

cProc   ioctl_char_redir,PUBLIC
parmW   handle
cBegin
        mov     bx,handle       ; Get handle.
        mov     ax,440ah        ; Set function code, Subfunction 0AH.
        int     21h             ; Call MS-DOS.
        mov     ax,-1           ; Assume error.
        jc      icrx            ; Branch on error, returning -1.
        inc     ax              ; Set AX = 0.
        test    dh,80h          ; Is bit 15 set?
        jz      icrx            ; If not, device/file is local:
                                ; Return 0.
        inc     ax              ; Return 1 for remote.
icrx:
cEnd
