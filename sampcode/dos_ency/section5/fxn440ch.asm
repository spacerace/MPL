        ;************************************************************;
        ;                                                            ;
        ;   Function 44H, Subfunction 0CH:                           ;
        ;                 Generic IOCTL for Handles                  ;
        ;                                                            ;
        ;   int ioctl_char_generic(handle,category,function,pbuffer) ;
        ;       int   handle;                                        ;
        ;       int   category;                                      ;
        ;       int   function;                                      ;
        ;       int  *pbuffer;                                       ;
        ;                                                            ;
        ;   Returns 0 for success, otherwise returns error code.     ;
        ;                                                            ;
        ;************************************************************;

cProc   ioctl_char_generic,PUBLIC,<ds>
parmW   handle
parmB   category
parmB   function
parmDP  pbuffer
cBegin
        mov     bx,handle       ; Get device handle.
        mov     ch,category     ; Get category
        mov     cl,function     ; and function.
        loadDP  ds,dx,pbuffer   ; Get pointer to data buffer.
        mov     ax,440ch        ; Set function code, Subfunction 0CH.
        int     21h             ; Call MS-DOS.
        jc      icgx            ; Branch on error.
        xor     ax,ax
icgx:
cEnd
