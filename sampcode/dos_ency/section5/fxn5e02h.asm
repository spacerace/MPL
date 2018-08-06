        ;************************************************************;
        ;                                                            ;
        ;       Function 5EH Subfunction 02H:                        ;
        ;                     Set Printer Setup                      ;
        ;                                                            ;
        ;       int printer_setup(index,pstring,len)                 ;
        ;           int   index;                                     ;
        ;           char *pstring;                                   ;
        ;           int   len;                                       ;
        ;                                                            ;
        ;       Returns 0, otherwise returns -1 for all errors.      ;
        ;                                                            ;
        ;************************************************************;

cProc   printer_setup,PUBLIC,<ds,si>
parmW   index
parmDP  pstring
parmW   len
cBegin
        mov     bx,index        ; BX = index of a net printer.
        loadDP  ds,si,pstring   ; DS:SI = pointer to string.
        mov     cx,len          ; CX = length of string.
        mov     ax,5e02h        ; Set function code.
        int     21h             ; Set printer prefix string.
        mov     al,0            ; Assume no error.
        jnb     ps_ok           ; Branch if no error,
        mov     al,-1           ; Else return -1.
ps_ok:
        cbw
cEnd
