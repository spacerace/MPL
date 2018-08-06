        ;************************************************************;
        ;                                                            ;
        ;             Function 2CH: Get Time                         ;
        ;                                                            ;
        ;             long get_time(phour,pmin,psec,phund)           ;
        ;                  char *phour,*pmin,*psec,*phund;           ;
        ;                                                            ;
        ;             Returns the time packed into a long:           ;
        ;                  low byte  = hundredths                    ;
        ;                  next byte = seconds                       ;
        ;                  next byte = minutes                       ;
        ;                  next byte = hours                         ;
        ;                                                            ;
        ;************************************************************;

cProc   get_time,PUBLIC,ds
parmDP  phour
parmDP  pmin
parmDP  psec
parmDP  phund
cBegin
        mov     ah,2ch          ; Set function code.
        int     21h             ; Get time from MS-DOS.
        loadDP  ds,bx,phour     ; DS:BX = pointer to hour.
        mov     [bx],ch         ; Return hour.
        loadDP  ds,bx,pmin      ; DS:BX = pointer to min.
        mov     [bx],cl         ; Return min.
        loadDP  ds,bx,psec      ; DS:BX = pointer to sec.
        mov     [bx],dh         ; Return sec.
        loadDP  ds,bx,phund     ; DS:BX = pointer to hund.
        mov     [bx],dl         ; Return hund.
        mov     ax,dx           ; Pack seconds, hundredths, ...
        mov     dx,cx           ; ... minutes, and hour into
                                ; return value.
cEnd
