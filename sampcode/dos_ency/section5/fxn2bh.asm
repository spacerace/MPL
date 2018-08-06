        ;************************************************************;
        ;                                                            ;
        ;             Function 2BH: Set Date                         ;
        ;                                                            ;
        ;             int set_date(month,day,year)                   ;
        ;                 char month,day;                            ;
        ;                 int year;                                  ;
        ;                                                            ;
        ;             Returns 0 if date was OK, -1 if not.           ;
        ;                                                            ;
        ;************************************************************;

cProc   set_date,PUBLIC
parmB   month
parmB   day
parmW   year
cBegin
        mov     dh,month        ; Get new month.
        mov     dl,day          ; Get new day.
        mov     cx,year         ; Get new year.
        mov     ah,2bh          ; Set function code.
        int     21h             ; Ask MS-DOS to change date.
        cbw                     ; Return 0 or -1.
cEnd
