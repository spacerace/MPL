        ;************************************************************;
        ;                                                            ;
        ;       Function 11H: Find First File, FCB-based             ;
        ;                                                            ;
        ;       int FCB_first(puXFCB,attrib)                         ;
        ;           char *puXFCB;                                    ;
        ;           char  attrib;                                    ;
        ;                                                            ;
        ;       Returns 0 if match found, otherwise returns -1.      ;
        ;                                                            ;
        ;       Note: The FCB must have the drive and                ;
        ;       filename fields (bytes 07H through 12H) and          ;
        ;       the extension flag (byte 00H) set before             ;
        ;       the call to FCB_first (see Function 29H).            ;
        ;                                                            ;
        ;************************************************************;
 
cProc   FCB_first,PUBLIC,ds
parmDP  puXFCB
parmB   attrib
cBegin
        loadDP  ds,dx,puXFCB    ; Pointer to unopened extended FCB.
        mov     bx,dx           ; BX points at FCB, too.
        mov     al,attrib       ; Get search attribute.
        mov     [bx+6],al       ; Put attribute into extended FCB
                                ; area.
        mov     byte ptr [bx],0ffh ; Set flag for extended FCB.
        mov     ah,11h          ; Ask MS-DOS to find 1st matching
                                ; file in current directory.
        int     21h             ; If match found, directory entry can
                                ; be found at DTA address.
        cbw                     ; Set return value to 0 or -1.
cEnd
