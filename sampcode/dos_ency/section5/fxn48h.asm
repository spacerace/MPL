        ;************************************************************;
        ;                                                            ;
        ;         Function 48H: Allocate Memory Block                ;
        ;                                                            ;
        ;         int get_block(nparas,pblocksegp,pmaxparas)         ;
        ;             int nparas,*pblockseg,*pmaxparas;              ;
        ;                                                            ;
        ;         Returns 0 if nparas are allocated OK and           ;
        ;         pblockseg has segment address of block,            ;
        ;         otherwise returns error code with pmaxparas        ;
        ;         set to maximum block size available.               ;
        ;                                                            ;
        ;************************************************************;

cProc   get_block,PUBLIC,ds
parmW   nparas
parmDP  pblockseg
parmDP  pmaxparas
cBegin
        mov     bx,nparas       ; Get size request.
        mov     ah,48h          ; Set function code.
        int     21h             ; Ask MS-DOS for memory.
        mov     cx,bx           ; Save BX.
        loadDP  ds,bx,pmaxparas
        mov     [bx],cx         ; Return result, assuming failure.
        jb      gb_err          ; Exit if error, leaving error code
                                ; in AX.
        loadDP  ds,bx,pblockseg
        mov     [bx],ax         ; No error, so store address of block.
        xor     ax,ax           ; Return 0.
gb_err:
cEnd
