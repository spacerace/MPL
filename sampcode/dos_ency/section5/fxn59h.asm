        ;************************************************************;
        ;                                                            ;
        ;         Function 59H: Get Extended Error Information       ;
        ;                                                            ;
        ;         int extended_error(err,class,action,locus)         ;
        ;             int *err;                                      ;
        ;             char *class,*action,*locus;                    ;
        ;                                                            ;
        ;         Return value is same as err.                       ;
        ;                                                            ;
        ;************************************************************;

cProc   extended_error,PUBLIC,<ds,si,di>
parmDP  perr
parmDP  pclass
parmDP  paction
parmDP  plocus
cBegin
        push    ds              ; Save DS.
        xor     bx,bx
        mov     ah,59h          ; Set function code.
        int     21h             ; Request error info from MS-DOS.
        pop     ds              ; Restore DS.
        loadDP  ds,si,perr      ; Get pointer to err.
        mov     [si],ax         ; Store err.
        loadDP  ds,si,pclass    ; Get pointer to class.
        mov     [si],bh         ; Store class.
        loadDP  ds,si,paction   ; Get pointer to action.
        mov     [si],bl         ; Store action.
        loadDP  ds,si,plocus    ; Get pointer to locus.
        mov     [si],ch         ; Store locus.
cEnd
