        ;************************************************************;
        ;                                                            ;
        ;           Function 03H: Auxiliary Input                    ;
        ;                                                            ;
        ;           int aux_in()                                     ;
        ;                                                            ;
        ;           Returns next character from AUX device.          ;
        ;                                                            ;
        ;************************************************************;

cProc   aux_in,PUBLIC
cBegin
        mov     ah,03h          ; Set function code.
        int     21h             ; Wait for character from AUX.
        mov     ah,0            ; Character is in AL
                                ; so clear high byte.
cEnd
