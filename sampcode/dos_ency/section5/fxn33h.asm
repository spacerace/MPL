        ;************************************************************;
        ;                                                            ;
        ;          Function 33H: Get/Set Control-C Check Flag        ;
        ;                                                            ;
        ;          int controlC(func,state)                          ;
        ;              int func,state;                               ;
        ;                                                            ;
        ;          Returns current state of Ctrl-C flag.             ;
        ;                                                            ;
        ;************************************************************;

cProc   controlC,PUBLIC
parmB   func
parmB   state
cBegin
        mov     al,func         ; Get set/reset function.
        mov     dl,state        ; Get new value if present.
        mov     ah,33h          ; MS-DOS ^C check function.
        int     21h             ; Call MS-DOS.
        mov     al,dl           ; Return current state.
        cbw                     ; Clear high byte of return value.
cEnd
