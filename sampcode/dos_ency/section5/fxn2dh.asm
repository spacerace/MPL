        ;************************************************************;
        ;                                                            ;
        ;             Function 2DH: Set Time                         ;
        ;                                                            ;
        ;             int set_time(hour,min,sec,hund)                ;
        ;                 char hour,min,sec,hund;                    ;
        ;                                                            ;
        ;             Returns 0 if time was OK, -1 if not.           ;
        ;                                                            ;
        ;************************************************************;

cProc   set_time,PUBLIC
parmB   hour
parmB   min
parmB   sec
parmB   hund
cBegin
        mov     ch,hour         ; Get new hour.
        mov     cl,min          ; Get new minutes.
        mov     dh,sec          ; Get new seconds.
        mov     dl,hund         ; Get new hundredths.
        mov     ah,2dh          ; Set function code.
        int     21h             ; Ask MS-DOS to change time.
        cbw                     ; Return 0 or -1.
cEnd
