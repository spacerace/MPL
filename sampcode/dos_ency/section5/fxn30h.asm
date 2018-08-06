        ;************************************************************;
        ;                                                            ;
        ;           Function 30H: Get MS-DOS Version Number          ;
        ;                                                            ;
        ;           int DOS_version()                                ;
        ;                                                            ;
        ;           Returns number of MS-DOS version, with           ;
        ;              major version in high byte,                   ;
        ;              minor version in low byte.                    ;
        ;                                                            ;
        ;************************************************************;

cProc   DOS_version,PUBLIC
cBegin
        mov     ax,3000H        ; Set function code and clear AL.
        int     21h             ; Ask MS-DOS for version number.
        xchg    al,ah           ; Swap major and minor numbers.
cEnd
