        ;************************************************************;
        ;                                                            ;
        ;         Function 4AH: Resize Memory Block                  ;
        ;                                                            ;
        ;         int modify_block(nparas,blockseg,pmaxparas)        ;
        ;             int nparas,blockseg,*pmaxparas;                ;
        ;                                                            ;
        ;         Returns 0 if modification was a success,           ;
        ;         otherwise returns error code with pmaxparas        ;
        ;         set to max number of paragraphs available.         ;
        ;                                                            ;
        ;************************************************************;

cProc   modify_block,PUBLIC,ds
parmW   nparas
parmW   blockseg
parmDP  pmaxparas
cBegin
        mov     es,blockseg     ; Get block address.
        mov     bx,nparas       ; Get nparas.
        mov     ah,4ah          ; Set function code.
        int     21h             ; Ask MS-DOS to change block size.
        mov     cx,bx           ; Save BX.
        loadDP  ds,bx,pmaxparas
        mov     [bx],cx         ; Set pmaxparas, assuming failure.
        jb      mb_exit         ; Branch if size change error.
        xor     ax,ax           ; Return 0 if successful.
mb_exit:
cEnd
