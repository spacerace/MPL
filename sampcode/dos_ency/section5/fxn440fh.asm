        ;************************************************************;
        ;                                                            ;
        ;       Function 44H, Subfunctions 0EH, 0FH:                 ;
        ;                     IOCTL Get/Set Logical Drive Map        ;
        ;                                                            ;
        ;       int ioctl_drive_owner(setflag, drv_ ltr)             ;
        ;           int setflag;                                     ;
        ;           int drv_ltr;                                     ;
        ;                                                            ;
        ;       Set setflag = 1 to change drive's map, 0 to get      ;
        ;       current map.                                         ;
        ;                                                            ;
        ;       Returns -1 for all errors, otherwise returns         ;
        ;       the block device's current logical drive letter.     ;
        ;                                                            ;
        ;************************************************************;

cProc   ioctl_drive_owner,PUBLIC
parmB   setflag
parmB   drv_ltr
cBegin
        mov     al,setflag      ; Load setflag.
        and     al,1            ; Keep only lsb.
        add     al,0fh          ; AL = 0EH for get, 0FH for set.
        mov     bl,drv_ltr      ; Get drive letter.
        or      bl,bl           ; Leave 0 alone.
        jz      ido
        and     bl,not 20h      ; Convert letter to uppercase.
        sub     bl,'A'-1        ; Convert to drive number: 'A' = 1,
                                ; 'B' = 2, etc.
ido:
        mov     bh,0
        mov     ah,44h          ; Set function code.
        int     21h             ; Call MS-DOS.
        mov     ah,0            ; Clear high byte.
        jnc     idox            ; Branch if no error.
        mov     ax,-1-'A'       ; Return -1 for errors.
idox:
        add     ax,'A'          ; Return drive letter.
cEnd
