        ;************************************************************;
        ;                                                            ;
        ;            Function 2AH: Get Date                          ;
        ;                                                            ;
        ;            long get_date(pdow,pmonth,pday,pyear)           ;
        ;                 char *pdow,*pmonth,*pday;                  ;
        ;                 int *pyear;                                ;
        ;                                                            ;
        ;            Returns the date packed into a long:            ;
        ;                 low byte  = day of month                   ;
        ;                 next byte = month                          ;
        ;                 next word = year.                          ;
        ;                                                            ;
        ;************************************************************;

cProc   get_date,PUBLIC,ds
parmDP  pdow
parmDP  pmonth
parmDP  pday
parmDP  pyear
cBegin
        mov     ah,2ah          ; Set function code.
        int     21h             ; Get date info from MS-DOS.
        loadDP  ds,bx,pdow      ; DS:BX = pointer to dow.
        mov     [bx],al         ; Return dow.
        loadDP  ds,bx,pmonth    ; DS:BX = pointer to month.
        mov     [bx],dh         ; Return month.
        loadDP  ds,bx,pday      ; DS:BX = pointer to day.
        mov     [bx],dl         ; Return day.
        loadDP  ds,bx,pyear     ; DS:BX = pointer to year.
        mov     [bx],cx         ; Return year.
        mov     ax,dx           ; Pack day, month, ...
        mov     dx,cx           ; ... and year into return value.
cEnd
