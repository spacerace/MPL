        ;************************************************************;
        ;                                                            ;
        ;             Function 0BH: Check Keyboard Status            ;
        ;                                                            ;
        ;             int key_ready()                                ;
        ;                                                            ;
        ;             Returns 1 if key is ready, 0 if not.           ;
        ;                                                            ;
        ;************************************************************;

cProc   key_ready,PUBLIC
cBegin
        mov     ah,0bh          ; Set function code.
        int     21h             ; Ask MS-DOS if key is available.
        and     ax,0001h        ; Keep least significant bit only.
cEnd
