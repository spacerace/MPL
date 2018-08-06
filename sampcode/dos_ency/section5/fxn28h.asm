        ;************************************************************;
        ;                                                            ;
        ;       Function 28H: Random File Block Write, FCB-based     ;
        ;                                                            ;
        ;       int FCB_wblock(oXFCB,nrequest,nactual,start)         ;
        ;           char *oXFCB;                                     ;
        ;           int   nrequest;                                  ;
        ;           int  *nactual;                                   ;
        ;           long  start;                                     ;
        ;                                                            ;
        ;       Returns write status of 0, 1, or 2 and sets          ;
        ;       nactual to number of records actually written.       ;
        ;                                                            ;
        ;       If start is -1, the relative-record field is         ;
        ;       not changed, causing the block to be written         ;
        ;       starting at the current record.                      ;
        ;                                                            ;
        ;************************************************************;

cProc   FCB_wblock,PUBLIC,<ds,di>
parmDP  poXFCB
parmW   nrequest
parmDP  pnactual
parmD   start
cBegin
        loadDP  ds,dx,poXFCB    ; Pointer to opened extended FCB.
        mov     di,dx           ; DI points at FCB, too.
        mov     ax,word ptr (start) ; Get long value of start.
        mov     bx,word ptr (start+2)
        mov     cx,ax           ; Is start = -1?
        and     cx,bx
        inc     cx
        jcxz    wb_skip         ; If so, don't change relative-record
                                ; field.
        mov     [di+28h],ax     ; Otherwise, seek to start record.
        mov     [di+2ah],bx
wb_skip:
        mov     cx,nrequest     ; CX = number of records to write.
        mov     ah,28h          ; Get MS-DOS to write CX records
        int     21h             ; from DTA to file.
        loadDP  ds,bx,pnactual  ; DS:BX = address of nactual.
        mov     ds:[bx],cx      ; Return number of records written.
        cbw                     ; Clear high byte.
cEnd
