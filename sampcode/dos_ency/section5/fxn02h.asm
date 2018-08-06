        ;************************************************************;
        ;                                                            ;
        ;                Function 02H: Character Output              ;
        ;                                                            ;
        ;                int disp_ch(c)                              ;
        ;                    char c;                                 ;
        ;                                                            ;
        ;                Returns 0.                                  ;
        ;                                                            ;
        ;************************************************************;

cProc   disp_ch,PUBLIC
parmB   c
cBegin
        mov     dl,c            ; Get character into DL.
        mov     ah,02h          ; Set function code.
        int     21h             ; Send character.
        xor     ax,ax           ; Return 0.
cEnd
