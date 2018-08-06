        ;************************************************************;
        ;                                                            ;
        ;     Function 0EH: Select Disk                              ;
        ;                                                            ;
        ;     int select_drive(drive_ltr)                            ;
        ;         char drive_ltr;                                    ;
        ;                                                            ;
        ;     Returns number of logical drives present in system.    ;
        ;                                                            ;
        ;************************************************************;

cProc   select_drive,PUBLIC
parmB   disk_ltr
cBegin
        mov     dl,disk_ltr     ; Get new drive letter.
        and     dl,not 20h      ; Make sure letter is uppercase.
        sub     dl,'A'          ; Convert drive letter to number,
                                ; 'A' = 0, 'B' = 1, etc.
        mov     ah,0eh          ; Set function code.
        int     21h             ; Ask MS-DOS to set default drive.
        cbw                     ; Clear high byte of return value.
cEnd
