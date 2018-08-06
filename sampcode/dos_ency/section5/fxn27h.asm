        ;************************************************************;
        ;                                                            ;
        ;      Function 27H: Random File Block Read, FCB-based       ;
        ;                                                            ;
        ;      int FCB_rblock(oXFCB,nrequest,nactual,start)          ;
        ;          char *oXFCB;                                      ;
        ;          int   nrequest;                                   ;
        ;          int  *nactual;                                    ;
        ;          long  start;                                      ;
        ;                                                            ;
        ;      Returns read status 0, 1, 2, or 3 and sets            ;
        ;      nactual to number of records actually read.           ;
        ;                                                            ;
        ;      If start is -1, the relative-record field is          ;
        ;      not changed, causing the block to be read starting    ;
        ;      at the current record.                                ;
        ;                                                            ;
        ;************************************************************;

cProc   FCB_rblock,PUBLIC,<ds,di>
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
        jcxz    rb_skip         ; If so, don't change relative-record
                                ; field.
        mov     [di+28h],ax     ; Otherwise, seek to start record.
        mov     [di+2ah],bx
rb_skip:
        mov     cx,nrequest     ; CX = number of records to read.
        mov     ah,27h          ; Get MS-DOS to read CX records,
        int     21h             ; placing them at DTA.
        loadDP  ds,bx,pnactual  ; DS:BX = address of nactual.
        mov     [bx],cx         ; Return number of records read.
        cbw                     ; Clear high byte.
cEnd
