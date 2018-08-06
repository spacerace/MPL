        ;************************************************************;
        ;                                                            ;
        ;                Function 1AH: Set DTA Address               ;
        ;                                                            ;
        ;                int set_DTA(pDTAbuffer)                     ;
        ;                    char far *pDTAbuffer;                   ;
        ;                                                            ;
        ;                Returns 0.                                  ;
        ;                                                            ;
        ;************************************************************;

cProc   set_DTA,PUBLIC,ds
parmD   pDTAbuffer
cBegin
        lds     dx,pDTAbuffer   ; DS:DX = pointer to buffer.
        mov     ah,1ah          ; Set function code.
        int     21h             ; Ask MS-DOS to change DTA address.
        xor     ax,ax           ; Return 0.
cEnd
