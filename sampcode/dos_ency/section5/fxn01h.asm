        ;************************************************************;
        ;                                                            ;
        ;           Function 01H: Character Input with Echo          ;
        ;                                                            ;
        ;           int read_kbd_echo()                              ;
        ;                                                            ;
        ;           Returns a character from standard input          ;
        ;           after sending it to standard output.             ;
        ;                                                            ;
        ;************************************************************;

cProc   read_kbd_echo,PUBLIC
cBegin
        mov     ah,01h          ; Set function code.
        int     21h             ; Wait for character.
        mov     ah,0            ; Character is in AL, so clear high
                                ; byte.
cEnd
