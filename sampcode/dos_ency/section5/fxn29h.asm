        ;************************************************************;
        ;                                                            ;
        ;            Function 29H: Parse Filename into FCB           ;
        ;                                                            ;
        ;            int FCB_parse(uXFCB,name,ctrl)                  ;
        ;                char *uXFCB;                                ;
        ;                char *name;                                 ;
        ;                int ctrl;                                   ;
        ;                                                            ;
        ;            Returns -1 if error,                            ;
        ;                     0 if no wildcards found,               ;
        ;                     1 if wildcards found.                  ;
        ;                                                            ;
        ;************************************************************;

cProc   FCB_parse,PUBLIC,<ds,si,di>
parmDP  puXFCB
parmDP  pname
parmB   ctrl
cBegin
        loadDP  es,di,puXFCB    ; Pointer to unopened extended FCB.
        push    di              ; Save DI.
        xor     ax,ax           ; Fill all 22 (decimal) words of the
                                ; extended FCB with zeros.
        cld                     ; Make sure direction flag says UP.
        mov     cx,22d
        rep     stosw
        pop     di              ; Recover DI.
        mov     byte ptr [di],0ffh ; Set flag byte to mark this as an
                                   ; extended FCB.
        add     di,7            ; Advance pointer to start of regular
                                ; FCB.
        loadDP  ds,si,pname     ; Get pointer to filename into DS:SI.
        mov     al,ctrl         ; Get parse control byte.
        mov     ah,29h          ; Parse filename, please.
        int     21h
        cbw                     ; Set return parameter.
cEnd
