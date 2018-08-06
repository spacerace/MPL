        ;************************************************************;
        ;                                                            ;
        ;         Function 07H: Unfiltered Character Input           ;
        ;                       Without Echo                         ;
        ;                                                            ;
        ;         int con_in()                                       ;
        ;                                                            ;
        ;         Returns next character from standard input.        ;
        ;                                                            ;
        ;************************************************************;

cProc   con_in,PUBLIC
cBegin
        mov     ah,07h          ; Set function code.
        int     21h             ; Wait for character, no echo.
        mov     ah,0            ; Clear high byte.
cEnd
