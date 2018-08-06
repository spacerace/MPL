        ;************************************************************;
        ;                                                            ;
        ;          Function 58H: Get/Set Allocation Strategy         ;
        ;                                                            ;
        ;          int alloc_strategy(func,strategy)                 ;
        ;              int func,strategy;                            ;
        ;                                                            ;
        ;          Strategies:                                       ;
        ;                  0: First fit                              ;
        ;                  1: Best fit                               ;
        ;                  2: Last fit                               ;
        ;                                                            ;
        ;          Returns -1 for all errors, otherwise              ;
        ;          returns the current strategy.                     ;
        ;                                                            ;
        ;************************************************************;

cProc   alloc_strategy,PUBLIC
parmB   func
parmW   strategy
cBegin
        mov     al,func         ; AL = get/set selector.
        mov     bx,strategy     ; BX = new strategy (for AL = 01H).
        mov     ah,58h          ; Set function code.
        int     21h             ; Call MS-DOS.
        jnb     no_err          ; Branch if no error.
        mov     ax,-1           ; Return -1 for all errors.
no_err:
cEnd
