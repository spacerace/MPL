        ;************************************************************;
        ;                                                            ;
        ;           Function 19H: Get Current Disk                   ;
        ;                                                            ;
        ;           int cur_drive()                                  ;
        ;                                                            ;
        ;           Returns letter of current "logged" disk.         ;
        ;                                                            ;
        ;************************************************************;

cProc   cur_drive,PUBLIC
cBegin
        mov     ah,19h          ; Set function code.
        int     21h             ; Get number of logged disk.
        add     al,'A'          ; Convert number to letter.
        cbw                     ; Clear the high byte of return value.
cEnd
