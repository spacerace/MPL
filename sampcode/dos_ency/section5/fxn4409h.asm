        ;************************************************************;
        ;                                                            ;
        ;        Function 44H, Subfunction 09H:                      ;
        ;                      IOCTL Remote Block Device Query       ;
        ;                                                            ;
        ;        int ioctl_block_redir(drive_ltr)                    ;
        ;            int drive_ltr;                                  ;
        ;                                                            ;
        ;        Returns -1 for all errors, 1 if disk is remote      ;
        ;        (redirected), 0 if disk is local.                   ;
        ;                                                            ;
        ;************************************************************;

cProc   ioctl_block_redir,PUBLIC
parmB   drive_ltr
cBegin
        mov     bl,drive_ltr    ; Get drive letter.
        or      bl,bl           ; Leave 0 alone.
        jz      ibr
        and     bl,not 20h      ; Convert letter to uppercase.
        sub     bl,'A'-1        ; Convert to drive number: 'A' = 1,
                                ; 'B' = 2, etc.
ibr:
        mov     ax,4409h        ; Set function code, Subfunction 09H.
        int     21h             ; Call MS-DOS.
        mov     ax,-1           ; Assume error.
        jc      ibrx            ; Branch if error, returning -1.
        inc     ax              ; Set AX = 0.
        test    dh,10h          ; Is bit 12 set?
        jz      ibrx            ; If not, disk is local: Return 0.
        inc     ax              ; Return 1 for remote disk.
ibrx:
cEnd
