        ;************************************************************;
        ;                                                            ;
        ;       Function 12H: Find Next File, FCB-based              ;
        ;                                                            ;
        ;       int FCB_next(puXFCB)                                 ;
        ;           char *puXFCB;                                    ;
        ;                                                            ;
        ;       Returns 0 if match found, otherwise returns -1.      ;
        ;                                                            ;
        ;       Note: The FCB must have the drive and                ;
        ;       filename fields (bytes 07H through 12H) and          ;
        ;       the extension flag (byte 00H) set before             ;
        ;       the call to FCB_next (see Function 29H).             ;
        ;                                                            ;
        ;************************************************************;

cProc   FCB_next,PUBLIC,ds
parmDP  puXFCB
cBegin
        loadDP  ds,dx,puXFCB    ; Pointer to unopened extended FCB.
        mov     ah,12h          ; Ask MS-DOS to find next matching
                                ; file in current directory.
        int     21h             ; If match found, directory entry can
                                ; be found at DTA address.
        cbw                     ; Set return value to 0 or -1.
cEnd
