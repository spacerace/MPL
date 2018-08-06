        ;************************************************************;
        ;                                                            ;
        ;        Function 44H, Subfunctions 00H,01H:                 ;
        ;                      Get/Set IOCTL Device Data             ;
        ;                                                            ;
        ;        int ioctl_char_flags(setflag,handle,newflags)       ;
        ;            int setflag;                                    ;
        ;            int handle;                                     ;
        ;            int newflags;                                   ;
        ;                                                            ;
        ;        Set setflag = 0 to get flags, 1 to set flags.       ;
        ;                                                            ;
        ;        Returns -1 for error, else returns flags.           ;
        ;                                                            ;
        ;************************************************************;

cProc   ioctl_char_flags,PUBLIC
parmB   setflag
parmW   handle
parmW   newflags
cBegin
        mov     al,setflag      ; Get setflag.
        and     al,1            ; Save only lsb.
        mov     bx,handle       ; Get handle to character device.
        mov     dx,newflags     ; Get new flags (they are used only
                                ; by "set" option).
        mov     ah,44h          ; Set function code.
        int     21h             ; Call MS-DOS.
        mov     ax,dx           ; Assume success - prepare to return
                                ; flags.
        jnc     iocfx           ; Branch if no error.
        mov     ax,-1           ; Else return error flag.
iocfx:
cEnd
