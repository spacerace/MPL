        ;************************************************************;
        ;                                                            ;
        ;      Function 57H: Get/Set Date/Time of File               ;
        ;                                                            ;
        ;      long file_date_time(handle,func,packdate,packtime)    ;
        ;           int handle,func,packdate,packtime;               ;
        ;                                                            ;
        ;      Returns a long -1 for all errors, otherwise packs     ;
        ;      date and time into a long integer,                    ;
        ;      date in high word, time in low word.                  ;
        ;                                                            ;
        ;************************************************************;

cProc   file_date_time,PUBLIC
parmW   handle
parmB   func
parmW   packdate
parmW   packtime
cBegin
        mov     bx,handle       ; Get handle.
        mov     al,func         ; Get function: 0 = read, 1 = write.
        mov     dx,packdate     ; Get date (if present).
        mov     cx,packtime     ; Get time (if present).
        mov     ah,57h          ; Set function code.
        int     21h             ; Call MS-DOS.
        mov     ax,cx           ; Set DX:AX = date/time, assuming no
                                ; error.
        jnb     dt_ok           ; Branch if no error.
        mov     ax,-1           ; Return -1 for errors.
        cwd                     ; Extend the -1 into DX.
dt_ok:
cEnd
