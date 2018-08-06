        ;************************************************************;
        ;                                                            ;
        ;       Function 34H: Get Return Address of InDOS Flag       ;
        ;                                                            ;
        ;       char far *inDOS_ptr()                                ;
        ;                                                            ;
        ;       Returns a far pointer to the MS-DOS inDOS flag.      ;
        ;                                                            ;
        ;************************************************************;
 
cProc   inDOS_ptr,PUBLIC
cBegin
        mov     ah,34h          ; InDOS flag function.
        int     21h             ; Call MS-DOS.
        mov     ax,bx           ; Return offset in AX.
        mov     dx,es           ; Return segment in DX.
cEnd
