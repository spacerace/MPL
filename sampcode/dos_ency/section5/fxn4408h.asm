        ;************************************************************;
        ;                                                            ;
        ;      Function 44H, Subfunction 08H:                        ;
        ;                    IOCTL Removable Block Device Query      ;
        ;                                                            ;
        ;      int ioctl_block_fixed(drive_ltr)                      ;
        ;          int drive_ltr;                                    ;
        ;                                                            ;
        ;      Returns -1 for all errors, 1 if disk is fixed (not    ;
        ;      removable), 0 if disk is not fixed.                   ;
        ;                                                            ;
        ;************************************************************;

cProc   ioctl_block_fixed,PUBLIC
parmB   drive_ltr
cBegin
        mov     bl,drive_ltr    ; Get drive letter.
        or      bl,bl           ; Leave 0 alone.
        jz      ibch
        and     bl,not 20h      ; Convert letter to uppercase.
        sub     bl,'A'-1        ; Convert to drive number: 'A' = 1,
                                ; 'B' = 2, etc.
ibch:
        mov     ax,4408h        ; Set function code, Subfunction 08H.
        int     21h             ; Call MS-DOS.
        jnc     ibchx           ; Branch if no error, AX = 0 or 1.
        cmp     ax,1            ; Treat error code of 1 as "disk is
                                ; fixed."
        je      ibchx
        mov     ax,-1           ; Return -1 for other errors.
ibchx:
cEnd
