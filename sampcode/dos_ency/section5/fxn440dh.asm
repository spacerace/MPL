        ;************************************************************;
        ;                                                            ;
        ;    Function 44H, Subfunction 0DH:                          ;
        ;                  Generic IOCTL for Block Devices           ;
        ;                                                            ;
        ;    int ioctl_block_generic(drv_ltr,category,func,pbuffer)  ;
        ;        int   drv_ltr;                                      ;
        ;        int   category;                                     ;
        ;        int   func;                                         ;
        ;        char *pbuffer;                                      ;
        ;                                                            ;
        ;    Returns 0 for success, otherwise returns error code.    ;
        ;                                                            ;
        ;************************************************************;

cProc   ioctl_block_generic,PUBLIC,<ds>
parmB   drv_ltr
parmB   category
parmB   func
parmDP  pbuffer
cBegin
        mov     bl,drv_ltr      ; Get drive letter.
        or      bl,bl           ; Leave 0 alone.
        jz      ibg
        and     bl,not 20h      ; Convert letter to uppercase.
        sub     bl,'A'-1        ; Convert to drive number: 'A' = 1,
                                ; 'B' = 2, etc.
ibg:
        mov     ch,category     ; Get category
        mov     cl,func         ; and function.
        loadDP  ds,dx,pbuffer   ; Get pointer to data buffer.
        mov     ax,440dh        ; Set function code, Subfunction 0DH.
        int     21h             ; Call MS-DOS.
        jc      ibgx            ; Branch on error.
        xor     ax,ax
ibgx:
cEnd
