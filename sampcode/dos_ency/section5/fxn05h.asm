        ;************************************************************;
        ;                                                            ;
        ;                Function 05H: Print Character               ;
        ;                                                            ;
        ;                int print_ch(c)                             ;
        ;                    char c;                                 ;
        ;                                                            ;
        ;                Returns 0.                                  ;
        ;                                                            ;
        ;************************************************************;

cProc   print_ch,PUBLIC
parmB   c
cBegin
        mov     dl,c            ; Get character into DL.
        mov     ah,05h          ; Set function code.
        int     21h             ; Write character to standard printer.
        xor     ax,ax           ; Return 0.
cEnd
