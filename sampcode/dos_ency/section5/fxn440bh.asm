        ;************************************************************;
        ;                                                            ;
        ;     Function 44H, Subfunction 0BH:                         ;
        ;                   IOCTL Change Sharing Retry Count         ;
        ;                                                            ;
        ;     int ioctl_set_retry(num_retries,wait_time)             ;
        ;         int num_retries;                                   ;
        ;         int wait_time;                                     ;
        ;                                                            ;
        ;     Returns 0 for success, otherwise returns error code.   ;
        ;                                                            ;
        ;************************************************************;

cProc   ioctl_set_retry,PUBLIC,<ds,si>
parmW   num_retries
parmW   wait_time
cBegin
        mov     dx,num_retries  ; Get parameters.
        mov     cx,wait_time
        mov     ax,440bh        ; Set function code, Subfunction 0BH.
        int     21h             ; Call MS-DOS.
        jc      isrx            ; Branch on error.
        xor     ax,ax
isrx:
cEnd
