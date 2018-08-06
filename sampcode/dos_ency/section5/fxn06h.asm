        ;************************************************************;
        ;                                                            ;
        ;          Function 06H: Direct Console I/O                  ;
        ;                                                            ;
        ;          int con_io(c)                                     ;
        ;              char c;                                       ;
        ;                                                            ;
        ;          Returns meaningless data if c is not 0FFH,        ;
        ;          otherwise returns next character from             ;
        ;          standard input.                                   ;
        ;                                                            ;
        ;************************************************************;

cProc   con_io,PUBLIC
parmB   c
cBegin
        mov     dl,c            ; Get character into DL.
        mov     ah,06h          ; Set function code.
        int     21h             ; This function does NOT wait in
                                ; input case (c = 0FFH)!
        mov     ah,0            ; Return the contents of AL.
cEnd
