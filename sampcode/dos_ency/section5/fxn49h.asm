        ;************************************************************;
        ;                                                            ;
        ;               Function 49H: Free Memory Block              ;
        ;                                                            ;
        ;               int free_block(blockseg)                     ;
        ;                   int blockseg;                            ;
        ;                                                            ;
        ;               Returns 0 if block freed OK,                 ;
        ;               otherwise returns error code.                ;
        ;                                                            ;
        ;************************************************************;

cProc   free_block,PUBLIC
parmW   blockseg
cBegin
        mov     es,blockseg     ; Get block address.
        mov     ah,49h          ; Set function code.
        int     21h             ; Ask MS-DOS to free memory.
        jb      fb_err          ; Branch on error.
        xor     ax,ax           ; Return 0 if successful.
fb_err:
cEnd
