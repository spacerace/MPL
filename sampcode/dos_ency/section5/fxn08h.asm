        ;************************************************************;
        ;                                                            ;
        ;    Function 08H:  Unfiltered Character Input Without Echo  ;
        ;                                                            ;
        ;    int read_kbd()                                          ;
        ;                                                            ;
        ;    Returns next character from standard input.             ;
        ;                                                            ;
        ;************************************************************;

cProc   read_kbd,PUBLIC
cBegin
        mov     ah,08h          ; Set function code.
        int     21h             ; Wait for character, no echo.
        mov     ah,0            ; Clear high byte.
cEnd
